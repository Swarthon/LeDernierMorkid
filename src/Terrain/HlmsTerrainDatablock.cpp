#include "Terrain/HlmsTerrain.h"
#include "Terrain/HlmsTerrainDatablock.h"

#include <OgreHlmsManager.h>
#include <OgreHlmsTextureManager.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>
#include <OgreLogManager.h>

using namespace Ogre;

const size_t HlmsTerrainDatablock::MaterialSizeInGpu          = 4 * 7 * 4;
const size_t HlmsTerrainDatablock::MaterialSizeInGpuAligned   = alignToNextMultiple(HlmsTerrainDatablock::MaterialSizeInGpu, 4 * 4 );

//-----------------------------------------------------------------------------------
HlmsTerrainDatablock::HlmsTerrainDatablock( IdString name, HlmsTerrain *creator,
		const HlmsMacroblock *macroblock,
		const HlmsBlendblock *blendblock,
		const HlmsParamVec &params ) :
			HlmsDatablock( name, creator, macroblock, blendblock, params ),
			mkDr( 0.318309886f ), mkDg( 0.318309886f ), mkDb( 0.318309886f ), //Max Diffuse = 1 / PI
			_padding0( 1 ),
			mBrdf( TerraBrdf::Default ) {
	mRoughness[0] = mRoughness[1] = 1.0f;
	mRoughness[2] = mRoughness[3] = 1.0f;
	mMetalness[0] = mMetalness[1] = 1.0f;
	mMetalness[2] = mMetalness[3] = 1.0f;

	for( size_t i=0; i<4; ++i )
		mDetailsOffsetScale[i] = Vector4( 0, 0, 1, 1 );

	memset( mTexIndices, 0, sizeof( mTexIndices ) );
	memset( mSamplerblocks, 0, sizeof( mSamplerblocks ) );

	for( size_t i=0; i<NUM_TERRAIN_TEXTURE_TYPES; ++i )
		mTexToBakedTextureIdx[i] = NUM_TERRAIN_TEXTURE_TYPES;

	calculateHash();

	creator->requestSlot( 0, this, false );
}
//-----------------------------------------------------------------------------------
HlmsTerrainDatablock::~HlmsTerrainDatablock() {
	if( mAssignedPool )
		static_cast<HlmsTerrain*>(mCreator)->releaseSlot( this );

	HlmsManager *hlmsManager = mCreator->getHlmsManager();
	if( hlmsManager ) {
		for( size_t i=0; i<NUM_TERRAIN_TEXTURE_TYPES; ++i )
			if( mSamplerblocks[i] )
				hlmsManager->destroySamplerblock( mSamplerblocks[i] );
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::calculateHash() {
	IdString hash;

	TerraBakedTextureArray::const_iterator itor = mBakedTextures.begin();
	TerraBakedTextureArray::const_iterator end  = mBakedTextures.end();

	while( itor != end ) {
		hash += IdString( itor->texture->getName() );
		hash += IdString( itor->samplerBlock->mId );
		++itor;
	}

	if( mTextureHash != hash.mHash ) {
		mTextureHash = hash.mHash;
		static_cast<HlmsTerrain*>(mCreator)->requestSlot( /*mTextureHash*/0, this, false );
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::scheduleConstBufferUpdate(void) {
	static_cast<HlmsTerrain*>(mCreator)->scheduleForUpdate( this );
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::uploadToConstBuffer( char *dstPtr ) {
	memcpy( dstPtr, &mkDr, MaterialSizeInGpu );
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::decompileBakedTextures( TerraBakedTexture outTextures[NUM_TERRAIN_TEXTURE_TYPES] ) {
	//Decompile the baked textures to know which texture is assigned to each type.
	for( size_t i=0; i<NUM_TERRAIN_TEXTURE_TYPES; ++i ) {
		uint8 idx = mTexToBakedTextureIdx[i];

		if( idx < NUM_TERRAIN_TEXTURE_TYPES )
			outTextures[i] = TerraBakedTexture( mBakedTextures[idx].texture, mSamplerblocks[i] );
		else
			outTextures[i] = TerraBakedTexture( TexturePtr(), mSamplerblocks[i] );
			//The texture may be null, but the samplerblock information may still be there.
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::bakeTextures( const TerraBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES] ) {
	mBakedTextures.clear();

	for( size_t i=0; i<NUM_TERRAIN_TEXTURE_TYPES; ++i ) {
		if( !textures[i].texture.isNull() ) {
			TerraBakedTextureArray::const_iterator itor = std::find( mBakedTextures.begin(),
																	 mBakedTextures.end(),
																	 textures[i] );

			if( itor == mBakedTextures.end() ) {
				mTexToBakedTextureIdx[i] = mBakedTextures.size();
				mBakedTextures.push_back( textures[i] );
			}
			else
				mTexToBakedTextureIdx[i] = itor - mBakedTextures.begin();
		}
		else
			mTexToBakedTextureIdx[i] = NUM_TERRAIN_TEXTURE_TYPES;
	}

	calculateHash();
	flushRenderables();
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setDiffuse( const Vector3 &diffuseColour ) {
	const float invPI = 0.318309886f;
	mkDr = diffuseColour.x * invPI;
	mkDg = diffuseColour.y * invPI;
	mkDb = diffuseColour.z * invPI;
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
Vector3 HlmsTerrainDatablock::getDiffuse(void) const {
	return Vector3( mkDr, mkDg, mkDb ) * Ogre::Math::PI;
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setRoughness( uint8 detailMapIdx, float roughness ) {
	mRoughness[detailMapIdx] = roughness;
	if( mRoughness[detailMapIdx] <= 1e-6f ) {
		LogManager::getSingleton().logMessage( "WARNING: TERRA Datablock '" +
			mName.getFriendlyText() + "' Very low roughness values can "
			"cause NaNs in the pixel shader!" );
	}
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
float HlmsTerrainDatablock::getRoughness( uint8 detailMapIdx ) const {
	return mRoughness[detailMapIdx];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setMetalness( uint8 detailMapIdx, float metalness ) {
	mMetalness[detailMapIdx] = metalness;
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
float HlmsTerrainDatablock::getMetalness( uint8 detailMapIdx ) const {
	return mMetalness[detailMapIdx];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::_setTextures( const PackedTexture packedTextures[NUM_TERRAIN_TEXTURE_TYPES] ) {
	TerraBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES];
	HlmsManager *hlmsManager = mCreator->getHlmsManager();

	for( int i=0; i<NUM_TERRAIN_TEXTURE_TYPES; ++i ) {
		if( mSamplerblocks[i] )
			hlmsManager->destroySamplerblock( mSamplerblocks[i] );

		mTexIndices[i] = packedTextures[i].xIdx;
		textures[i] = TerraBakedTexture( packedTextures[i].texture, packedTextures[i].samplerblock );

		if( !textures[i].texture.isNull() && !textures[i].samplerBlock ) {
			HlmsSamplerblock samplerBlockRef;
			if( i >= TERRAIN_DETAIL0 && i <= TERRAIN_DETAIL_METALNESS3 ) {
				//Detail maps default to wrap mode.
				samplerBlockRef.mU = TAM_WRAP;
				samplerBlockRef.mV = TAM_WRAP;
				samplerBlockRef.mW = TAM_WRAP;
			}

			textures[i].samplerBlock = hlmsManager->getSamplerblock( samplerBlockRef );
		}

		mSamplerblocks[i] = textures[i].samplerBlock;
	}

	bakeTextures( textures );
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setTexture( TerrainTextureTypes texType, uint16 arrayIndex,
		const TexturePtr &newTexture, const HlmsSamplerblock *refParams ) {
	TerraBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES];

	//Decompile the baked textures to know which texture is assigned to each type.
	decompileBakedTextures( textures );

	//Set the new samplerblock
	if( refParams ) {
		HlmsManager *hlmsManager = mCreator->getHlmsManager();
		const HlmsSamplerblock *oldSamplerblock = mSamplerblocks[texType];
		mSamplerblocks[texType] = hlmsManager->getSamplerblock( *refParams );

		if( oldSamplerblock )
			hlmsManager->destroySamplerblock( oldSamplerblock );
	}
	else if( !newTexture.isNull() && !mSamplerblocks[texType] ) {
		//Adding a texture, but the samplerblock doesn't exist. Create a default one.
		HlmsSamplerblock samplerBlockRef;
		if( texType >= TERRAIN_DETAIL0 && texType <= TERRAIN_DETAIL_METALNESS3 ) {
			//Detail maps default to wrap mode.
			samplerBlockRef.mU = TAM_WRAP;
			samplerBlockRef.mV = TAM_WRAP;
			samplerBlockRef.mW = TAM_WRAP;
		}

		HlmsManager *hlmsManager = mCreator->getHlmsManager();
		mSamplerblocks[texType] = hlmsManager->getSamplerblock( samplerBlockRef );
	}

	TerraBakedTexture oldTex = textures[texType];

	//Set the texture and make the samplerblock changes to take effect
	textures[texType].texture = newTexture;
	textures[texType].samplerBlock = mSamplerblocks[texType];
	mTexIndices[texType] = arrayIndex;

	if( oldTex == textures[texType] )
		scheduleConstBufferUpdate();	//Only the array index changed. Just update our constant buffer.
	else
		bakeTextures( textures );
}
//-----------------------------------------------------------------------------------
TexturePtr HlmsTerrainDatablock::getTexture( TerrainTextureTypes texType ) const {
	TexturePtr retVal;

	if( mTexToBakedTextureIdx[texType] < mBakedTextures.size() )
		retVal = mBakedTextures[mTexToBakedTextureIdx[texType]].texture;

	return retVal;
}
//-----------------------------------------------------------------------------------
TexturePtr HlmsTerrainDatablock::getTexture( size_t texType ) const {
	return getTexture( static_cast<TerrainTextureTypes>( texType ) );
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setSamplerblock( TerrainTextureTypes texType, const HlmsSamplerblock &params ) {
	const HlmsSamplerblock *oldSamplerblock = mSamplerblocks[texType];
	HlmsManager *hlmsManager = mCreator->getHlmsManager();
	mSamplerblocks[texType] = hlmsManager->getSamplerblock( params );

	if( oldSamplerblock )
		hlmsManager->destroySamplerblock( oldSamplerblock );

	if( oldSamplerblock != mSamplerblocks[texType] ) {
		TerraBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES];
		decompileBakedTextures( textures );
		bakeTextures( textures );
	}
}
//-----------------------------------------------------------------------------------
const HlmsSamplerblock* HlmsTerrainDatablock::getSamplerblock( TerrainTextureTypes texType ) const {
	return mSamplerblocks[texType];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setDetailMapOffsetScale( uint8 detailMap, const Vector4 &offsetScale ) {
	assert( detailMap < 8 );
	bool wasDisabled = mDetailsOffsetScale[detailMap] == Vector4( 0, 0, 1, 1 );

	mDetailsOffsetScale[detailMap] = offsetScale;

	if( wasDisabled != (mDetailsOffsetScale[detailMap] == Vector4( 0, 0, 1, 1 )) )
		flushRenderables();

	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
const Vector4& HlmsTerrainDatablock::getDetailMapOffsetScale( uint8 detailMap ) const {
	assert( detailMap < 8 );
	return mDetailsOffsetScale[detailMap];
}
//-----------------------------------------------------------------------------------
uint8 HlmsTerrainDatablock::getBakedTextureIdx( TerrainTextureTypes texType ) const {
	return mTexToBakedTextureIdx[texType];
}
//-----------------------------------------------------------------------------------
uint16 HlmsTerrainDatablock::_getTextureSliceArrayIndex( TerrainTextureTypes texType ) const {
	return mTexIndices[texType];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setAlphaTestThreshold( float threshold ) {
	OGRE_EXCEPT( Exception::ERR_NOT_IMPLEMENTED,
				 "Alpha testing not supported on Terra Hlms",
				 "HlmsTerrainDatablock::setAlphaTestThreshold" );

	HlmsDatablock::setAlphaTestThreshold( threshold );
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setBrdf( TerraBrdf::TerraBrdf brdf ) {
	if( mBrdf != brdf ) {
		mBrdf = brdf;
		flushRenderables();
	}
}
//-----------------------------------------------------------------------------------
uint32 HlmsTerrainDatablock::getBrdf(void) const {
	return mBrdf;
}
//-----------------------------------------------------------------------------------
