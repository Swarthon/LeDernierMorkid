#ifndef _HLMSTERRAIN_H_
#define _HLMSTERRAIN_H_

#include "HlmsTerrainPrerequisites.h"
#include "OgreHlmsBufferManager.h"
#include "OgreConstBufferPool.h"
#include "OgreMatrix4.h"
#include "OgreHeaderPrefix.h"

class HlmsTerrainDatablock;

class HlmsTerrain : public Ogre::HlmsBufferManager, public Ogre::ConstBufferPool {
public:
	enum ShadowFilter {
		PCF_2x2,
		PCF_3x3,
		PCF_4x4,

		NumShadowFilter
	};

	enum AmbientLightMode {
		AmbientAuto,
		AmbientFixed,
		AmbientHemisphere,
		AmbientNone
	};

protected:
	typedef Ogre::vector<Ogre::ConstBufferPacked*>::type ConstBufferPackedVec;
	typedef Ogre::vector<Ogre::HlmsDatablock*>::type HlmsDatablockVec;

	struct PassData {
		Ogre::FastArray<Ogre::Texture*> shadowMaps;
		Ogre::FastArray<float>    vertexShaderSharedBuffer;
		Ogre::FastArray<float>    pixelShaderSharedBuffer;
		Ogre::Matrix4 viewMatrix;
	};

	PassData                mPreparedPass;
	ConstBufferPackedVec    mPassBuffers;
	Ogre::HlmsSamplerblock const  *mShadowmapSamplerblock;    /// GL3+ only when not using depth textures
	Ogre::HlmsSamplerblock const  *mShadowmapCmpSamplerblock; /// For depth textures & D3D11
	Ogre::HlmsSamplerblock const  *mCurrentShadowmapSamplerblock;
	Ogre::HlmsSamplerblock const  *mTerrainSamplerblock;

	Ogre::uint32                  mCurrentPassBuffer;     /// Resets every to zero every new frame.

	Ogre::TexBufferPacked         *mGridBuffer;
	Ogre::TexBufferPacked         *mGlobalLightListBuffer;

	ConstBufferPool::BufferPool const *mLastBoundPool;

	Ogre::uint32 mLastTextureHash;
	Ogre::MovableObject const *mLastMovableObject;

	bool mDebugPssmSplits;

	ShadowFilter mShadowFilter;
	AmbientLightMode mAmbientLightMode;

	virtual const Ogre::HlmsCache* createShaderCacheEntry( Ogre::uint32 renderableHash,
		const Ogre::HlmsCache &passCache,
		Ogre::uint32 finalHash,
		const Ogre::QueuedRenderable &queuedRenderable);

	virtual Ogre::HlmsDatablock* createDatablockImpl( Ogre::IdString datablockName,
		const Ogre::HlmsMacroblock *macroblock,
		const Ogre::HlmsBlendblock *blendblock,
		const Ogre::HlmsParamVec &paramVec );

	void setDetailMapProperties( HlmsTerrainDatablock *datablock, Ogre::PiecesMap *inOutPieces );
	void setTextureProperty( const char *propertyName, HlmsTerrainDatablock *datablock, TerrainTextureTypes texType );
	void setDetailTextureProperty( const char *propertyName, HlmsTerrainDatablock *datablock, TerrainTextureTypes baseTexType, Ogre::uint8 detailIdx );

	virtual void calculateHashForPreCreate( Ogre::Renderable *renderable, Ogre::PiecesMap *inOutPieces );
	virtual void destroyAllBuffers(void);

	FORCEINLINE Ogre::uint32 fillBuffersFor( const Ogre::HlmsCache *cache,
		const Ogre::QueuedRenderable &queuedRenderable,
		bool casterPass, Ogre::uint32 lastCacheHash,
		Ogre::CommandBuffer *commandBuffer, bool isV1 );

public:
	HlmsTerrain( Ogre::Archive *dataFolder, Ogre::ArchiveVec *libraryFolders );
	virtual ~HlmsTerrain();

	virtual void _changeRenderSystem( Ogre::RenderSystem *newRs );

	virtual void setOptimizationStrategy( OptimizationStrategy optimizationStrategy ) {}

	virtual Ogre::HlmsCache preparePassHash( const Ogre::CompositorShadowNode *shadowNode, bool casterPass, bool dualParaboloid, Ogre::SceneManager *sceneManager );

	virtual Ogre::uint32 fillBuffersFor( const Ogre::HlmsCache *cache, const Ogre::QueuedRenderable &queuedRenderable,
		bool casterPass, Ogre::uint32 lastCacheHash,
		Ogre::uint32 lastTextureHash );

	virtual Ogre::uint32 fillBuffersForV1( const Ogre::HlmsCache *cache,
		const Ogre::QueuedRenderable &queuedRenderable,
		bool casterPass, Ogre::uint32 lastCacheHash,
		Ogre::CommandBuffer *commandBuffer );
	virtual Ogre::uint32 fillBuffersForV2( const Ogre::HlmsCache *cache,
		const Ogre::QueuedRenderable &queuedRenderable,
		bool casterPass, Ogre::uint32 lastCacheHash,
		Ogre::CommandBuffer *commandBuffer );

	virtual void frameEnded(void);

	void setDebugPssmSplits( bool bDebug );
	bool getDebugPssmSplits(void) const                 { return mDebugPssmSplits; }

	void setShadowSettings( ShadowFilter filter );
	ShadowFilter getShadowFilter(void) const            { return mShadowFilter; }

	void setAmbientLightMode( AmbientLightMode mode );
	AmbientLightMode getAmbientLightMode(void) const    { return mAmbientLightMode; }
};

struct TerrainProperty {
	static const Ogre::IdString HwGammaRead;
	static const Ogre::IdString HwGammaWrite;
	static const Ogre::IdString SignedIntTex;
	static const Ogre::IdString MaterialsPerBuffer;
	static const Ogre::IdString DebugPssmSplits;

	static const Ogre::IdString UseSkirts;

	static const Ogre::IdString NumTextures;
	static const char *DiffuseMap;
	static const char *EnvProbeMap;
	static const char *DetailWeightMap;
	static const char *DetailMapN;
	static const char *DetailMapNmN;
	static const char *RoughnessMap;
	static const char *MetalnessMap;

	static const Ogre::IdString FresnelScalar;
	static const Ogre::IdString MetallicWorkflow;

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

	static const Ogre::IdString *DetailOffsetsPtrs[4];
};


#include "OgreHeaderSuffix.h"

#endif // _HLMSTERRAIN_H_
