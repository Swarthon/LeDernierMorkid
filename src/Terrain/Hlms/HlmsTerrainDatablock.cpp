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

#include "Terrain/Hlms/HlmsTerrain.h"
#include "Terrain/Hlms/HlmsTerrainDatablock.h"

#include <OgreGpuProgram.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsTextureManager.h>
#include <OgreLogManager.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>

const size_t HlmsTerrainDatablock::MaterialSizeInGpu        = 4 * 7 * 4;
const size_t HlmsTerrainDatablock::MaterialSizeInGpuAligned = Ogre::alignToNextMultiple(
        HlmsTerrainDatablock::MaterialSizeInGpu,
        4 * 4);

//-----------------------------------------------------------------------------------
HlmsTerrainDatablock::HlmsTerrainDatablock(Ogre::IdString name, HlmsTerrain* creator, const Ogre::HlmsMacroblock* macroblock, const Ogre::HlmsBlendblock* blendblock, const Ogre::HlmsParamVec& params)
                : Ogre::HlmsDatablock(name, creator, macroblock, blendblock, params),
                  mkDr(0.318309886f),
                  mkDg(0.318309886f),
                  mkDb(0.318309886f), //Max Diffuse = 1 / PI
                  _padding0(1),
                  mBrdf(TerrainBrdf::Default) {
	mRoughness[0] = mRoughness[1] = 1.0f;
	mRoughness[2] = mRoughness[3] = 1.0f;
	mMetalness[0] = mMetalness[1] = 1.0f;
	mMetalness[2] = mMetalness[3] = 1.0f;

	for (size_t i                  = 0; i < 4; ++i)
		mDetailsOffsetScale[i] = Ogre::Vector4(0, 0, 1, 1);

	memset(mTexIndices, 0, sizeof(mTexIndices));
	memset(mSamplerblocks, 0, sizeof(mSamplerblocks));

	for (size_t i                    = 0; i < NUM_TERRAIN_TEXTURE_TYPES; ++i)
		mTexToBakedTextureIdx[i] = NUM_TERRAIN_TEXTURE_TYPES;

	calculateHash();

	creator->requestSlot(/*mTextureHash*/ 0, this, false);
}
//-----------------------------------------------------------------------------------
HlmsTerrainDatablock::~HlmsTerrainDatablock() {
	if (mAssignedPool)
		static_cast<HlmsTerrain*>(mCreator)->releaseSlot(this);

	Ogre::HlmsManager* hlmsManager = mCreator->getHlmsManager();
	if (hlmsManager) {
		for (size_t i = 0; i < NUM_TERRAIN_TEXTURE_TYPES; ++i) {
			if (mSamplerblocks[i])
				hlmsManager->destroySamplerblock(mSamplerblocks[i]);
		}
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::calculateHash() {
	Ogre::IdString hash;

	TerrainBakedTextureArray::const_iterator itor = mBakedTextures.begin();
	TerrainBakedTextureArray::const_iterator end  = mBakedTextures.end();

	while (itor != end) {
		hash += Ogre::IdString(itor->texture->getName());
		hash += Ogre::IdString(itor->samplerBlock->mId);

		++itor;
	}

	if (mTextureHash != hash.mHash) {
		mTextureHash = hash.mHash;
		static_cast<HlmsTerrain*>(mCreator)->requestSlot(/*mTextureHash*/ 0, this, false);
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::scheduleConstBufferUpdate(void) {
	static_cast<HlmsTerrain*>(mCreator)->scheduleForUpdate(this);
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::uploadToConstBuffer(char* dstPtr) {
	memcpy(dstPtr, &mkDr, MaterialSizeInGpu);
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::decompileBakedTextures(TerrainBakedTexture outTextures[NUM_TERRAIN_TEXTURE_TYPES]) {
	//Decompile the baked textures to know which texture is assigned to each type.
	for (size_t i = 0; i < NUM_TERRAIN_TEXTURE_TYPES; ++i) {
		Ogre::uint8 idx = mTexToBakedTextureIdx[i];

		if (idx < NUM_TERRAIN_TEXTURE_TYPES) {
			outTextures[i] = TerrainBakedTexture(mBakedTextures[idx].texture, mSamplerblocks[i]);
		}
		else {
			//The texture may be null, but the samplerblock information may still be there.
			outTextures[i] = TerrainBakedTexture(Ogre::TexturePtr(), mSamplerblocks[i]);
		}
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::bakeTextures(const TerrainBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES]) {
	//The shader might need to be recompiled (mTexToBakedTextureIdx changed).
	//We'll need to flush.
	//Most likely mTexIndices also changed, so we need to update the const buffers as well
	mBakedTextures.clear();

	for (size_t i = 0; i < NUM_TERRAIN_TEXTURE_TYPES; ++i) {
		if (!textures[i].texture.isNull()) {
			TerrainBakedTextureArray::const_iterator itor = std::find(mBakedTextures.begin(),
			                                                          mBakedTextures.end(),
			                                                          textures[i]);

			if (itor == mBakedTextures.end()) {
				mTexToBakedTextureIdx[i] = mBakedTextures.size();
				mBakedTextures.push_back(textures[i]);
			}
			else {
				mTexToBakedTextureIdx[i] = itor - mBakedTextures.begin();
			}
		}
		else {
			mTexToBakedTextureIdx[i] = NUM_TERRAIN_TEXTURE_TYPES;
		}
	}

	calculateHash();
	flushRenderables();
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
//    Ogre::TexturePtr HlmsTerrainDatablock::setTexture( const Ogre::String &name,
//                                             TerrainTextureTypes textureType )
//    {
//        const Ogre::HlmsTextureManager::TextureMapType texMapTypes[NUM_TERRAIN_TEXTURE_TYPES] =
//        {
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_NORMALS,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_MONOCHROME,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL_NORMAL_MAP,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL_NORMAL_MAP,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL_NORMAL_MAP,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_DETAIL_NORMAL_MAP,
//            Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP
//        };

//        Ogre::HlmsManager *hlmsManager = mCreator->getHlmsManager();
//        Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();
//        Ogre::HlmsTextureManager::TextureLocation texLocation = hlmsTextureManager->
//                                                    createOrRetrieveTexture( name,
//                                                                             texMapTypes[textureType] );

//        assert( texLocation.texture->isTextureTypeArray() || textureType == TERRAIN_REFLECTION );

//        //If HLMS texture manager failed to find a reflection texture, have look int standard texture manager
//        //NB we only do this for reflection textures as all other textures must be texture arrays for performance reasons
//        if (textureType == TERRAIN_REFLECTION && texLocation.texture == hlmsTextureManager->getBlankTexture().texture)
//        {
//            Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName(name);
//            if (tex.isNull() == false)
//            {
//                texLocation.texture = tex;
//                texLocation.xIdx = 0;
//                texLocation.yIdx = 0;
//                texLocation.divisor = 1;
//            }
//        }

//        mTexIndices[textureType] = texLocation.xIdx;

//        return texLocation.texture;
//    }
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setDiffuse(const Ogre::Vector3& diffuseColour) {
	const float invPI = 0.318309886f;
	mkDr              = diffuseColour.x * invPI;
	mkDg              = diffuseColour.y * invPI;
	mkDb              = diffuseColour.z * invPI;
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
Ogre::Vector3 HlmsTerrainDatablock::getDiffuse(void) const {
	return Ogre::Vector3(mkDr, mkDg, mkDb) * Ogre::Math::PI;
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setRoughness(Ogre::uint8 detailMapIdx, float roughness) {
	mRoughness[detailMapIdx] = roughness;
	if (mRoughness[detailMapIdx] <= 1e-6f) {
		Ogre::LogManager::getSingleton().logMessage("WARNING: Terrain Datablock '" +
		                                            mName.getFriendlyText() + "' Very low roughness values can "
		                                                                      "cause NaNs in the pixel shader!");
	}
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
float HlmsTerrainDatablock::getRoughness(Ogre::uint8 detailMapIdx) const {
	return mRoughness[detailMapIdx];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setMetalness(Ogre::uint8 detailMapIdx, float metalness) {
	mMetalness[detailMapIdx] = metalness;
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
float HlmsTerrainDatablock::getMetalness(Ogre::uint8 detailMapIdx) const {
	return mMetalness[detailMapIdx];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::_setTextures(const PackedTexture packedTextures[NUM_TERRAIN_TEXTURE_TYPES]) {
	TerrainBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES];

	Ogre::HlmsManager* hlmsManager = mCreator->getHlmsManager();

	for (int i = 0; i < NUM_TERRAIN_TEXTURE_TYPES; ++i) {
		if (mSamplerblocks[i])
			hlmsManager->destroySamplerblock(mSamplerblocks[i]);

		mTexIndices[i] = packedTextures[i].xIdx;
		textures[i]    = TerrainBakedTexture(packedTextures[i].texture, packedTextures[i].samplerblock);

		if (!textures[i].texture.isNull() && !textures[i].samplerBlock) {
			Ogre::HlmsSamplerblock samplerBlockRef;
			if (i >= TERRAIN_DETAIL0 && i <= TERRAIN_DETAIL_METALNESS3) {
				//Detail maps default to wrap mode.
				samplerBlockRef.mU = Ogre::TAM_WRAP;
				samplerBlockRef.mV = Ogre::TAM_WRAP;
				samplerBlockRef.mW = Ogre::TAM_WRAP;
			}

			textures[i].samplerBlock = hlmsManager->getSamplerblock(samplerBlockRef);
		}

		mSamplerblocks[i] = textures[i].samplerBlock;
	}

	bakeTextures(textures);
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setTexture(TerrainTextureTypes texType, Ogre::uint16 arrayIndex, const Ogre::TexturePtr& newTexture, const Ogre::HlmsSamplerblock* refParams) {
	TerrainBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES];

	//Decompile the baked textures to know which texture is assigned to each type.
	decompileBakedTextures(textures);

	//Set the new samplerblock
	if (refParams) {
		Ogre::HlmsManager*            hlmsManager     = mCreator->getHlmsManager();
		const Ogre::HlmsSamplerblock* oldSamplerblock = mSamplerblocks[texType];
		mSamplerblocks[texType]                       = hlmsManager->getSamplerblock(*refParams);

		if (oldSamplerblock)
			hlmsManager->destroySamplerblock(oldSamplerblock);
	}
	else if (!newTexture.isNull() && !mSamplerblocks[texType]) {
		//Adding a texture, but the samplerblock doesn't exist. Create a default one.
		Ogre::HlmsSamplerblock samplerBlockRef;
		if (texType >= TERRAIN_DETAIL0 && texType <= TERRAIN_DETAIL_METALNESS3) {
			//Detail maps default to wrap mode.
			samplerBlockRef.mU = Ogre::TAM_WRAP;
			samplerBlockRef.mV = Ogre::TAM_WRAP;
			samplerBlockRef.mW = Ogre::TAM_WRAP;
		}

		Ogre::HlmsManager* hlmsManager = mCreator->getHlmsManager();
		mSamplerblocks[texType]        = hlmsManager->getSamplerblock(samplerBlockRef);
	}

	TerrainBakedTexture oldTex = textures[texType];

	//Set the texture and make the samplerblock changes to take effect
	textures[texType].texture      = newTexture;
	textures[texType].samplerBlock = mSamplerblocks[texType];
	mTexIndices[texType]           = arrayIndex;

	if (oldTex == textures[texType]) {
		//Only the array index changed. Just update our constant buffer.
		scheduleConstBufferUpdate();
	}
	else {
		bakeTextures(textures);
	}
}
//-----------------------------------------------------------------------------------
Ogre::TexturePtr HlmsTerrainDatablock::getTexture(TerrainTextureTypes texType) const {
	Ogre::TexturePtr retVal;

	if (mTexToBakedTextureIdx[texType] < mBakedTextures.size())
		retVal = mBakedTextures[mTexToBakedTextureIdx[texType]].texture;

	return retVal;
}
//-----------------------------------------------------------------------------------
Ogre::TexturePtr HlmsTerrainDatablock::getTexture(size_t texType) const {
	return getTexture(static_cast<TerrainTextureTypes>(texType));
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setSamplerblock(TerrainTextureTypes texType, const Ogre::HlmsSamplerblock& params) {
	const Ogre::HlmsSamplerblock* oldSamplerblock = mSamplerblocks[texType];
	Ogre::HlmsManager*            hlmsManager     = mCreator->getHlmsManager();
	mSamplerblocks[texType]                       = hlmsManager->getSamplerblock(params);

	if (oldSamplerblock)
		hlmsManager->destroySamplerblock(oldSamplerblock);

	if (oldSamplerblock != mSamplerblocks[texType]) {
		TerrainBakedTexture textures[NUM_TERRAIN_TEXTURE_TYPES];
		decompileBakedTextures(textures);
		bakeTextures(textures);
	}
}
//-----------------------------------------------------------------------------------
const Ogre::HlmsSamplerblock* HlmsTerrainDatablock::getSamplerblock(TerrainTextureTypes texType) const {
	return mSamplerblocks[texType];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setDetailMapOffsetScale(Ogre::uint8 detailMap, const Ogre::Vector4& offsetScale) {
	assert(detailMap < 8);
	bool wasDisabled = mDetailsOffsetScale[detailMap] == Ogre::Vector4(0, 0, 1, 1);

	mDetailsOffsetScale[detailMap] = offsetScale;

	if (wasDisabled != (mDetailsOffsetScale[detailMap] == Ogre::Vector4(0, 0, 1, 1))) {
		flushRenderables();
	}

	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
const Ogre::Vector4& HlmsTerrainDatablock::getDetailMapOffsetScale(Ogre::uint8 detailMap) const {
	assert(detailMap < 8);
	return mDetailsOffsetScale[detailMap];
}
//-----------------------------------------------------------------------------------
Ogre::uint8 HlmsTerrainDatablock::getBakedTextureIdx(TerrainTextureTypes texType) const {
	return mTexToBakedTextureIdx[texType];
}
//-----------------------------------------------------------------------------------
Ogre::uint16 HlmsTerrainDatablock::_getTextureSliceArrayIndex(TerrainTextureTypes texType) const {
	return mTexIndices[texType];
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setAlphaTestThreshold(float threshold) {
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
	            "Alpha testing not supported on Terrain Hlms",
	            "HlmsTerrainDatablock::setAlphaTestThreshold");

	Ogre::HlmsDatablock::setAlphaTestThreshold(threshold);
	scheduleConstBufferUpdate();
}
//-----------------------------------------------------------------------------------
void HlmsTerrainDatablock::setBrdf(TerrainBrdf::TerrainBrdf brdf) {
	if (mBrdf != brdf) {
		mBrdf = brdf;
		flushRenderables();
	}
}
//-----------------------------------------------------------------------------------
Ogre::uint32 HlmsTerrainDatablock::getBrdf(void) const {
	return mBrdf;
}
//-----------------------------------------------------------------------------------
//    Ogre::HlmsTextureManager::TextureMapType HlmsTerrainDatablock::suggestMapTypeBasedOnTextureType(
//                                                                        TerrainTextureTypes type )
//    {
//        Ogre::HlmsTextureManager::TextureMapType retVal;
//        switch( type )
//        {
//        default:
//        case TERRAIN_DIFFUSE:
//        case TERRAIN_SPECULAR:
//        case TERRAIN_DETAIL_WEIGHT:
//        case TERRAIN_DETAIL0:
//        case TERRAIN_DETAIL1:
//        case TERRAIN_DETAIL2:
//        case TERRAIN_DETAIL3:
//            retVal = Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE;
//            break;

//        case TERRAIN_DETAIL0_NM:
//        case TERRAIN_DETAIL1_NM:
//        case TERRAIN_DETAIL2_NM:
//        case TERRAIN_DETAIL3_NM:
//            retVal = Ogre::HlmsTextureManager::TEXTURE_TYPE_NORMALS;
//            break;

//        case TERRAIN_DETAIL_ROUGHNESS:
//            retVal = Ogre::HlmsTextureManager::TEXTURE_TYPE_MONOCHROME;asd;
//            break;

//        case TERRAIN_REFLECTION:
//            retVal = Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP;
//            break;
//        }

//        return retVal;
//    }
