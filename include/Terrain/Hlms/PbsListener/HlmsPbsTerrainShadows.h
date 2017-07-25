/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

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

#ifndef _HLMSPBSTERRAINSHADOWS_H_
#define _HLMSPBSTERRAINSHADOWS_H_

#include <OgreGpuProgram.h>
#include <OgreHlmsListener.h>

class GraphicsTerrain;

class HlmsPbsTerrainShadows : public Ogre::HlmsListener {
	GraphicsTerrain*              mTerrain;
	Ogre::HlmsSamplerblock const* mTerrainSamplerblock;
#if OGRE_DEBUG_MODE
	Ogre::SceneManager* mSceneManager;
#endif

public:
	HlmsPbsTerrainShadows();
	~HlmsPbsTerrainShadows();

	void setTerrain(GraphicsTerrain* terra);

	virtual void shaderCacheEntryCreated(const Ogre::String&           shaderProfile,
	                                     const Ogre::HlmsCache*        hlmsCacheEntry,
	                                     const Ogre::HlmsCache&        passCache,
	                                     const Ogre::HlmsPropertyVec&  properties,
	                                     const Ogre::QueuedRenderable& queuedRenderable);

	virtual void preparePassHash(const Ogre::CompositorShadowNode* shadowNode,
	                             bool                              casterPass,
	                             bool                              dualParaboloid,
	                             Ogre::SceneManager*               sceneManager,
	                             Ogre::Hlms*                       hlms);

	virtual Ogre::uint32 getPassBufferSize(const Ogre::CompositorShadowNode* shadowNode, bool casterPass, bool dualParaboloid, Ogre::SceneManager* sceneManager) const;

	virtual float* preparePassBuffer(const Ogre::CompositorShadowNode* shadowNode, bool casterPass, bool dualParaboloid, Ogre::SceneManager* sceneManager, float* passBufferPtr);

	virtual void hlmsTypeChanged(bool casterPass, Ogre::CommandBuffer* commandBuffer, const Ogre::HlmsDatablock* datablock);
};

struct PbsTerrainProperty {
	static const Ogre::IdString TerrainEnabled;
};

#endif
