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
#ifndef _HLMSTERRAIN_H_
#define _HLMSTERRAIN_H_

#include <OgreConstBufferPool.h>
#include <OgreHeaderPrefix.h>
#include <OgreHlmsBufferManager.h>
#include <OgreHlmsDatablock.h>
#include <OgreMatrix4.h>
#include <OgreConstBufferPool.h>
#include <Vao/OgreConstBufferPacked.h>

#include "Terrain/Hlms/HlmsTerrainPrerequisites.h"

namespace Ogre {
	class CompositorShadowNode;
	typedef vector<ConstBufferPacked*>::type ConstBufferPackedVec;
	typedef vector<HlmsDatablock*>::type     HlmsDatablockVec;
	struct QueuedRenderable;
}

/** \addtogroup Component
    *  @{
    */
/** \addtogroup Material
    *  @{
    */

class HlmsTerrainDatablock;

/** Physically based shading implementation specfically designed for
        OpenGL 3+, D3D11 and other RenderSystems which support uniform buffers.
    */
class HlmsTerrain : public Ogre::HlmsBufferManager, public Ogre::ConstBufferPool {
public:
	enum ShadowFilter {
		/// Standard quality. Very fast.
		PCF_2x2,

		/// Good quality. Still quite fast on most modern hardware.
		PCF_3x3,

		/// High quality. Very slow in old hardware (i.e. DX10 level hw and below)
		/// Use RSC_TEXTURE_GATHER to check whether it will be slow or not.
		PCF_4x4,

		NumShadowFilter
	};

	enum AmbientLightMode {
		/// Use fixed-colour ambient lighting when upper hemisphere = lower hemisphere,
		/// use hemisphere lighting when they don't match.
		/// Disables ambient lighting if the colours are black.
		AmbientAuto,

		/// Force fixed-colour ambient light. Only uses the upper hemisphere paramter.
		AmbientFixed,

		/// Force hemisphere ambient light. Useful if you plan on adjusting the colours
		/// dynamically very often and this might cause swapping shaders.
		AmbientHemisphere,

		/// Disable ambient lighting.
		AmbientNone
	};

protected:
	struct PassData {
		Ogre::FastArray<Ogre::Texture*> shadowMaps;
		Ogre::FastArray<float>          vertexShaderSharedBuffer;
		Ogre::FastArray<float>          pixelShaderSharedBuffer;

		Ogre::Matrix4 viewMatrix;
	};

	PassData                      mPreparedPass;
	Ogre::ConstBufferPackedVec    mPassBuffers;
	Ogre::HlmsSamplerblock const* mShadowmapSamplerblock;    /// GL3+ only when not using depth textures
	Ogre::HlmsSamplerblock const* mShadowmapCmpSamplerblock; /// For depth textures & D3D11
	Ogre::HlmsSamplerblock const* mCurrentShadowmapSamplerblock;
	Ogre::HlmsSamplerblock const* mTerrainSamplerblock;

	Ogre::uint32 mCurrentPassBuffer; /// Resets every to zero every new frame.

	Ogre::TexBufferPacked* mGridBuffer;
	Ogre::TexBufferPacked* mGlobalLightListBuffer;

	Ogre::ConstBufferPool::BufferPool const* mLastBoundPool;

	Ogre::uint32               mLastTextureHash;
	Ogre::MovableObject const* mLastMovableObject;

	bool mDebugPssmSplits;

	ShadowFilter     mShadowFilter;
	AmbientLightMode mAmbientLightMode;

	virtual const Ogre::HlmsCache* createShaderCacheEntry(Ogre::uint32                  renderableHash,
	                                                      const Ogre::HlmsCache&        passCache,
	                                                      Ogre::uint32                  finalHash,
	                                                      const Ogre::QueuedRenderable& queuedRenderable);

	virtual Ogre::HlmsDatablock* createDatablockImpl(Ogre::IdString              datablockName,
	                                                 const Ogre::HlmsMacroblock* macroblock,
	                                                 const Ogre::HlmsBlendblock* blendblock,
	                                                 const Ogre::HlmsParamVec&   paramVec);

	void setDetailMapProperties(HlmsTerrainDatablock* datablock, Ogre::PiecesMap* inOutPieces);
	void setTextureProperty(const char* propertyName, HlmsTerrainDatablock* datablock, TerrainTextureTypes texType);
	void setDetailTextureProperty(const char* propertyName, HlmsTerrainDatablock* datablock, TerrainTextureTypes baseTexType, Ogre::uint8 detailIdx);

	virtual void calculateHashForPreCreate(Ogre::Renderable* renderable, Ogre::PiecesMap* inOutPieces);

	virtual void destroyAllBuffers(void);

	FORCEINLINE Ogre::uint32 fillBuffersFor(const Ogre::HlmsCache*        cache,
	                                        const Ogre::QueuedRenderable& queuedRenderable,
	                                        bool                          casterPass,
	                                        Ogre::uint32                  lastCacheHash,
	                                        Ogre::CommandBuffer*          commandBuffer,
	                                        bool                          isV1);

public:
	HlmsTerrain(Ogre::Archive* dataFolder, Ogre::ArchiveVec* libraryFolders);
	virtual ~HlmsTerrain();

	virtual void _changeRenderSystem(Ogre::RenderSystem* newRs);

	/// Not supported
	virtual void setOptimizationStrategy(Ogre::ConstBufferPool::OptimizationStrategy optimizationStrategy) {}

	virtual Ogre::HlmsCache preparePassHash(const Ogre::CompositorShadowNode* shadowNode,
	                                        bool                              casterPass,
	                                        bool                              dualParaboloid,
	                                        Ogre::SceneManager*               sceneManager);

	virtual Ogre::uint32 fillBuffersFor(const Ogre::HlmsCache* cache, const Ogre::QueuedRenderable& queuedRenderable, bool casterPass, Ogre::uint32 lastCacheHash, Ogre::uint32 lastTextureHash);

	virtual Ogre::uint32 fillBuffersForV1(const Ogre::HlmsCache*        cache,
	                                      const Ogre::QueuedRenderable& queuedRenderable,
	                                      bool                          casterPass,
	                                      Ogre::uint32                  lastCacheHash,
	                                      Ogre::CommandBuffer*          commandBuffer);
	virtual Ogre::uint32 fillBuffersForV2(const Ogre::HlmsCache*        cache,
	                                      const Ogre::QueuedRenderable& queuedRenderable,
	                                      bool                          casterPass,
	                                      Ogre::uint32                  lastCacheHash,
	                                      Ogre::CommandBuffer*          commandBuffer);

	virtual void frameEnded(void);

	void setDebugPssmSplits(bool bDebug);
	bool getDebugPssmSplits(void) const { return mDebugPssmSplits; }

	void setShadowSettings(ShadowFilter filter);
	ShadowFilter getShadowFilter(void) const { return mShadowFilter; }

	void setAmbientLightMode(AmbientLightMode mode);
	AmbientLightMode getAmbientLightMode(void) const { return mAmbientLightMode; }
};

struct TerrainProperty {
	static const Ogre::IdString HwGammaRead;
	static const Ogre::IdString HwGammaWrite;
	static const Ogre::IdString SignedIntTex;
	static const Ogre::IdString MaterialsPerBuffer;
	static const Ogre::IdString DebugPssmSplits;

	static const Ogre::IdString UseSkirts;

	static const Ogre::IdString NumTextures;
	static const char*          DiffuseMap;
	static const char*          EnvProbeMap;
	static const char*          DetailWeightMap;
	static const char*          DetailMapN;
	static const char*          DetailMapNmN;
	static const char*          RoughnessMap;
	static const char*          MetalnessMap;

	static const Ogre::IdString FresnelScalar;
	static const Ogre::IdString MetallicWorkflow;
	static const Ogre::IdString ReceiveShadows;

	static const Ogre::IdString DetailOffsets0;
	static const Ogre::IdString DetailOffsets1;
	static const Ogre::IdString DetailOffsets2;
	static const Ogre::IdString DetailOffsets3;

	static const Ogre::IdString DetailMapsDiffuse;
	static const Ogre::IdString DetailMapsNormal;
	static const Ogre::IdString FirstValidDetailMapNm;

	static const Ogre::IdString Pcf3x3;
	static const Ogre::IdString Pcf4x4;
	static const Ogre::IdString PcfIterations;

	static const Ogre::IdString EnvMapScale;
	static const Ogre::IdString AmbientFixed;
	static const Ogre::IdString AmbientHemisphere;

	static const Ogre::IdString BrdfDefault;
	static const Ogre::IdString BrdfCookTorrance;
	static const Ogre::IdString FresnelSeparateDiffuse;
	static const Ogre::IdString GgxHeightCorrelated;

	static const Ogre::IdString* DetailOffsetsPtrs[4];
};

/** @} */
/** @} */

#include "OgreHeaderSuffix.h"

#endif
