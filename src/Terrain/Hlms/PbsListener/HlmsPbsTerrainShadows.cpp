/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2016 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "Terrain/Hlms/PbsListener/HlmsPbsTerrainShadows.h"
#include "Terrain/GraphicsTerrain.h"

#include <CommandBuffer/OgreCbTexture.h>
#include <CommandBuffer/OgreCommandBuffer.h>

#include <OgreHlms.h>
#include <OgreHlmsManager.h>
#include <OgreRoot.h>

const Ogre::IdString PbsTerrainProperty::TerrainEnabled = Ogre::IdString("TERRAIN_enabled");

HlmsPbsTerrainShadows::HlmsPbsTerrainShadows()
                : mTerrain(0), mTerrainSamplerblock(0)
#if OGRE_DEBUG_MODE
                  ,
                  mSceneManager(0)
#endif
{
}
//-----------------------------------------------------------------------------------
HlmsPbsTerrainShadows::~HlmsPbsTerrainShadows() {
	if (mTerrainSamplerblock) {
		Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();
		hlmsManager->destroySamplerblock(mTerrainSamplerblock);
		mTerrainSamplerblock = 0;
	}
}
//-----------------------------------------------------------------------------------
void HlmsPbsTerrainShadows::setTerrain(GraphicsTerrain* Terrain) {
	mTerrain = Terrain;
	if (!mTerrainSamplerblock) {
		Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();
		mTerrainSamplerblock           = hlmsManager->getSamplerblock(Ogre::HlmsSamplerblock());
	}
}
//-----------------------------------------------------------------------------------
void HlmsPbsTerrainShadows::shaderCacheEntryCreated(const Ogre::String&           shaderProfile,
                                                    const Ogre::HlmsCache*        hlmsCacheEntry,
                                                    const Ogre::HlmsCache&        passCache,
                                                    const Ogre::HlmsPropertyVec&  properties,
                                                    const Ogre::QueuedRenderable& queuedRenderable) {
	if (shaderProfile != "hlsl" &&
	    Ogre::Hlms::getProperty(passCache.setProperties, Ogre::HlmsBaseProp::ShadowCaster) == 0) {
		if (!hlmsCacheEntry->pso.vertexShader.isNull()) {
			Ogre::GpuProgramParametersSharedPtr vsParams = hlmsCacheEntry->pso.vertexShader->getDefaultParameters();

			//The slot 12 is arbitrary. Should be high enough enough to not mess
			//with most materials (it could conflict if a material uses *A LOT* of
			//textures and they're in different arrays).
			//Note OpenGL has very low limits (usually 15-16)
			vsParams->setNamedConstant("terrainShadows", 12);
		}
	}
}
//-----------------------------------------------------------------------------------
void HlmsPbsTerrainShadows::preparePassHash(const Ogre::CompositorShadowNode* shadowNode,
                                            bool                              casterPass,
                                            bool                              dualParaboloid,
                                            Ogre::SceneManager*               sceneManager,
                                            Ogre::Hlms*                       hlms) {
	if (!casterPass) {
#if OGRE_DEBUG_MODE
		mSceneManager = sceneManager;
#endif

		if (mTerrain && hlms->_getProperty(Ogre::HlmsBaseProp::LightsDirNonCaster) > 0) {
			//First directional light always cast shadows thanks to our terrain shadows.
			Ogre::int32 shadowCasterDirectional = hlms->_getProperty(Ogre::HlmsBaseProp::LightsDirectional);
			shadowCasterDirectional             = std::max(shadowCasterDirectional, 1);
			hlms->_setProperty(Ogre::HlmsBaseProp::LightsDirectional, shadowCasterDirectional);
		}

		hlms->_setProperty(PbsTerrainProperty::TerrainEnabled, mTerrain != 0);
	}
}
//-----------------------------------------------------------------------------------
Ogre::uint32 HlmsPbsTerrainShadows::getPassBufferSize(const Ogre::CompositorShadowNode* shadowNode,
                                                      bool                              casterPass,
                                                      bool                              dualParaboloid,
                                                      Ogre::SceneManager*               sceneManager) const {
	return (!casterPass && mTerrain) ? 32u : 0u;
}
//-----------------------------------------------------------------------------------
float* HlmsPbsTerrainShadows::preparePassBuffer(const Ogre::CompositorShadowNode* shadowNode,
                                                bool                              casterPass,
                                                bool                              dualParaboloid,
                                                Ogre::SceneManager*               sceneManager,
                                                float*                            passBufferPtr) {
	if (!casterPass && mTerrain) {
		const float          invHeight       = 1.0f / mTerrain->getHeight();
		const Ogre::Vector3& terrainOrigin   = mTerrain->getTerrainOrigin();
		const Ogre::Vector2& terrainXZInvDim = mTerrain->getXZInvDimensions();
		*passBufferPtr++                     = -terrainOrigin.x * terrainXZInvDim.x;
		*passBufferPtr++                     = -terrainOrigin.y * invHeight;
		*passBufferPtr++                     = -terrainOrigin.z * terrainXZInvDim.y;
		*passBufferPtr++                     = 1.0f;

		*passBufferPtr++ = terrainXZInvDim.x;
		*passBufferPtr++ = invHeight;
		*passBufferPtr++ = terrainXZInvDim.y;
		*passBufferPtr++ = 1.0f;
	}

	return passBufferPtr;
}
//-----------------------------------------------------------------------------------
void HlmsPbsTerrainShadows::hlmsTypeChanged(bool casterPass, Ogre::CommandBuffer* commandBuffer, const Ogre::HlmsDatablock* datablock) {
	if (!casterPass && mTerrain) {
		Ogre::TexturePtr terraShadowTex = mTerrain->_getShadowMapTex();

		//Bind the shadows' texture. Tex. slot must match with
		//the one in HlmsPbsTerrainShadows::shaderCacheEntryCreated
		*commandBuffer->addCommand<Ogre::CbTexture>() = Ogre::CbTexture(12u, true, terraShadowTex.get(), mTerrainSamplerblock);

#if OGRE_DEBUG_MODE
		const CompositorTextureVec&          compositorTextures = mSceneManager->getCompositorTextures();
		CompositorTextureVec::const_iterator itor               = compositorTextures.begin();
		CompositorTextureVec::const_iterator end                = compositorTextures.end();

		while (itor != end && (*itor->textures)[0] != terraShadowTex)
			++itor;

		if (itor == end) {
			assert("Hazard Detected! You should expose this Terrain's shadow map texture"
			       " to the compositor pass so Ogre can place the proper Barriers" &&
			       false);
		}
#endif
	}
}
