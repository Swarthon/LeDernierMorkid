#ifndef _HLMSTERRADATABLOCK_H_
#define _HLMSTERRADATABLOCK_H_

#include "HlmsTerrainPrerequisites.h"
#include "OgreHlmsDatablock.h"
#include "OgreHlmsTextureManager.h"
#include "OgreConstBufferPool.h"
#include "OgreVector4.h"
#include "OgreHeaderPrefix.h"

struct PackedTexture {
	Ogre::TexturePtr  texture;
	Ogre::uint16      xIdx;
	Ogre::HlmsSamplerblock const * samplerblock;
	PackedTexture() : xIdx( NUM_TERRAIN_TEXTURE_TYPES ), samplerblock( 0 ) {}
};

struct TerraBakedTexture {
	Ogre::TexturePtr              texture;
	Ogre::HlmsSamplerblock const *samplerBlock;

	TerraBakedTexture() : samplerBlock( 0 ) {}
	TerraBakedTexture( const Ogre::TexturePtr tex, const Ogre::HlmsSamplerblock *_samplerBlock ) :
		texture( tex ), samplerBlock( _samplerBlock ) {}

	bool operator == ( const TerraBakedTexture &_r ) const {
		return texture == _r.texture && samplerBlock == _r.samplerBlock;
	}
};

namespace TerraBrdf {
	enum TerraBrdf {
		FLAG_UNCORRELATED                           = 0x80000000,
		FLAG_SPERATE_DIFFUSE_FRESNEL                = 0x40000000,
		BRDF_MASK                                   = 0x00000FFF,
		Default         = 0x00000000,
		CookTorrance    = 0x00000001,
		DefaultUncorrelated             = Default|FLAG_UNCORRELATED,
		DefaultSeparateDiffuseFresnel   = Default|FLAG_SPERATE_DIFFUSE_FRESNEL,
		CookTorranceSeparateDiffuseFresnel  = CookTorrance|FLAG_SPERATE_DIFFUSE_FRESNEL,
	};
}

typedef Ogre::FastArray<TerraBakedTexture> TerraBakedTextureArray;

class HlmsTerrainDatablock : public Ogre::HlmsDatablock, public Ogre::ConstBufferPoolUser {
	friend class HlmsTerrain;

protected:
	float   mkDr, mkDg, mkDb;                   //kD
	float   _padding0;
	float   mRoughness[4];
	float   mMetalness[4];
	Ogre::Vector4 mDetailsOffsetScale[4];
	Ogre::uint16  mTexIndices[NUM_TERRAIN_TEXTURE_TYPES];

	TerraBakedTextureArray mBakedTextures;
	Ogre::uint8   mTexToBakedTextureIdx[NUM_TERRAIN_TEXTURE_TYPES];

	Ogre::HlmsSamplerblock const  *mSamplerblocks[NUM_TERRAIN_TEXTURE_TYPES];

	Ogre::uint32  mBrdf;

	void scheduleConstBufferUpdate(void);
	virtual void uploadToConstBuffer( char *dstPtr );

	Ogre::TexturePtr setTexture( const Ogre::String &name, TerrainTextureTypes textureType );

	void decompileBakedTextures( TerraBakedTexture outTextures[NUM_TERRAIN_TEXTURE_TYPES] );
	void bakeTextures( const TerraBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES] );

public:
	HlmsTerrainDatablock( Ogre::IdString name, HlmsTerrain *creator,
					  const Ogre::HlmsMacroblock *macroblock,
					  const Ogre::HlmsBlendblock *blendblock,
					  const Ogre::HlmsParamVec &params );
	virtual ~HlmsTerrainDatablock();

	void setDiffuse( const Ogre::Vector3 &diffuseColour );
	Ogre::Vector3 getDiffuse(void) const;

	void setRoughness( Ogre::uint8 detailMapIdx, float roughness );
	float getRoughness( Ogre::uint8 detailMapIdx ) const;

	void setMetalness( Ogre::uint8 detailMapIdx, float metalness );
	float getMetalness( Ogre::uint8 detailMapIdx ) const;

	void _setTextures( const PackedTexture packedTextures[] );

	void setTexture( TerrainTextureTypes texType, Ogre::uint16 arrayIndex, const Ogre::TexturePtr &newTexture, const Ogre::HlmsSamplerblock *refParams=0 );

	Ogre::TexturePtr getTexture( TerrainTextureTypes texType ) const;
	Ogre::TexturePtr getTexture( size_t texType ) const;

	Ogre::uint16 _getTextureIdx( TerrainTextureTypes texType ) const          { return mTexIndices[texType]; }

	void setSamplerblock( TerrainTextureTypes texType, const Ogre::HlmsSamplerblock &params );

	const Ogre::HlmsSamplerblock* getSamplerblock( TerrainTextureTypes texType ) const;

	void setDetailMapOffsetScale( Ogre::uint8 detailMap, const Ogre::Vector4 &offsetScale );
	const Ogre::Vector4& getDetailMapOffsetScale( Ogre::uint8 detailMap ) const;

	Ogre::uint8 getBakedTextureIdx( TerrainTextureTypes texType ) const;

	Ogre::uint16 _getTextureSliceArrayIndex( TerrainTextureTypes texType ) const;

	virtual void setAlphaTestThreshold( float threshold );

	void setBrdf( TerraBrdf::TerraBrdf brdf );
	Ogre::uint32 getBrdf(void) const;

	static Ogre::HlmsTextureManager::TextureMapType suggestMapTypeBasedOnTextureType( TerrainTextureTypes type );

	virtual void calculateHash();

	static const size_t MaterialSizeInGpu;
	static const size_t MaterialSizeInGpuAligned;
};


#include "OgreHeaderSuffix.h"

#endif // _HLMSTERRADATABLOCK_H_
