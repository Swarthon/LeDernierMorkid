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

#include <OgreGpuProgram.h>
#include <OgreHlmsListener.h>
#include <OgreHlmsManager.h>
#include <OgreLwString.h>
#include <Terrain/Hlms/HlmsTerrainDatablock.h>

#include <OgreCamera.h>
#include <OgreForward3D.h>
#include <OgreHighLevelGpuProgram.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreRenderTarget.h>
#include <OgreViewport.h>

#include <Compositor/OgreCompositorShadowNode.h>
#include <OgreSceneManager.h>
#include <Vao/OgreConstBufferPacked.h>
#include <Vao/OgreVaoManager.h>

#include <CommandBuffer/OgreCbShaderBuffer.h>
#include <CommandBuffer/OgreCbTexture.h>
#include <CommandBuffer/OgreCommandBuffer.h>

#include "Terrain/GraphicsTerrain.h"

const Ogre::IdString TerrainProperty::HwGammaRead        = Ogre::IdString("hw_gamma_read");
const Ogre::IdString TerrainProperty::HwGammaWrite       = Ogre::IdString("hw_gamma_write");
const Ogre::IdString TerrainProperty::SignedIntTex       = Ogre::IdString("signed_int_textures");
const Ogre::IdString TerrainProperty::MaterialsPerBuffer = Ogre::IdString("materials_per_buffer");
const Ogre::IdString TerrainProperty::DebugPssmSplits    = Ogre::IdString("debug_pssm_splits");

const Ogre::IdString TerrainProperty::UseSkirts = Ogre::IdString("use_skirts");

const Ogre::IdString TerrainProperty::NumTextures     = Ogre::IdString("num_textures");
const char*          TerrainProperty::DiffuseMap      = "diffuse_map";
const char*          TerrainProperty::EnvProbeMap     = "envprobe_map";
const char*          TerrainProperty::DetailWeightMap = "detail_weight_map";
const char*          TerrainProperty::DetailMapN      = "detail_map";    //detail_map0-4
const char*          TerrainProperty::DetailMapNmN    = "detail_map_nm"; //detail_map_nm0-4
const char*          TerrainProperty::RoughnessMap    = "roughness_map";
const char*          TerrainProperty::MetalnessMap    = "metalness_map";

const Ogre::IdString TerrainProperty::FresnelScalar    = Ogre::IdString("fresnel_scalar");
const Ogre::IdString TerrainProperty::MetallicWorkflow = Ogre::IdString("metallic_workflow");
const Ogre::IdString TerrainProperty::ReceiveShadows   = Ogre::IdString("receive_shadows");

const Ogre::IdString TerrainProperty::DetailOffsets0 = Ogre::IdString("detail_offsets0");
const Ogre::IdString TerrainProperty::DetailOffsets1 = Ogre::IdString("detail_offsets1");
const Ogre::IdString TerrainProperty::DetailOffsets2 = Ogre::IdString("detail_offsets2");
const Ogre::IdString TerrainProperty::DetailOffsets3 = Ogre::IdString("detail_offsets3");

const Ogre::IdString TerrainProperty::DetailMapsDiffuse     = Ogre::IdString("detail_maps_diffuse");
const Ogre::IdString TerrainProperty::DetailMapsNormal      = Ogre::IdString("detail_maps_normal");
const Ogre::IdString TerrainProperty::FirstValidDetailMapNm = Ogre::IdString("first_valid_detail_map_nm");

const Ogre::IdString TerrainProperty::Pcf3x3        = Ogre::IdString("pcf_3x3");
const Ogre::IdString TerrainProperty::Pcf4x4        = Ogre::IdString("pcf_4x4");
const Ogre::IdString TerrainProperty::PcfIterations = Ogre::IdString("pcf_iterations");

const Ogre::IdString TerrainProperty::EnvMapScale       = Ogre::IdString("envmap_scale");
const Ogre::IdString TerrainProperty::AmbientFixed      = Ogre::IdString("ambient_fixed");
const Ogre::IdString TerrainProperty::AmbientHemisphere = Ogre::IdString("ambient_hemisphere");

const Ogre::IdString TerrainProperty::BrdfDefault            = Ogre::IdString("BRDF_Default");
const Ogre::IdString TerrainProperty::BrdfCookTorrance       = Ogre::IdString("BRDF_CookTorrance");
const Ogre::IdString TerrainProperty::FresnelSeparateDiffuse = Ogre::IdString("fresnel_separate_diffuse");
const Ogre::IdString TerrainProperty::GgxHeightCorrelated    = Ogre::IdString("GGX_height_correlated");

const Ogre::IdString* TerrainProperty::DetailOffsetsPtrs[4] =
        {
                &TerrainProperty::DetailOffsets0,
                &TerrainProperty::DetailOffsets1,
                &TerrainProperty::DetailOffsets2,
                &TerrainProperty::DetailOffsets3};

HlmsTerrain::HlmsTerrain(Ogre::Archive* dataFolder, Ogre::ArchiveVec* libraryFolders)
                : Ogre::HlmsBufferManager(Ogre::HLMS_USER3, "Terrain", dataFolder, libraryFolders),
                  Ogre::ConstBufferPool(HlmsTerrainDatablock::MaterialSizeInGpuAligned,
                                        Ogre::ConstBufferPool::ExtraBufferParams()),
                  mShadowmapSamplerblock(0),
                  mShadowmapCmpSamplerblock(0),
                  mCurrentShadowmapSamplerblock(0),
                  mTerrainSamplerblock(0),
                  mCurrentPassBuffer(0),
                  mLastBoundPool(0),
                  mLastTextureHash(0),
                  mLastMovableObject(0),
                  mDebugPssmSplits(false),
                  mShadowFilter(PCF_3x3),
                  mAmbientLightMode(AmbientAuto) {
	//Override defaults
	mLightGatheringMode = LightGatherForwardPlus;
}
//-----------------------------------------------------------------------------------
HlmsTerrain::~HlmsTerrain() {
	destroyAllBuffers();
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::_changeRenderSystem(Ogre::RenderSystem* newRs) {
	Ogre::ConstBufferPool::_changeRenderSystem(newRs);

	//Force the pool to contain only 1 entry.
	mSlotsPerPool = 1u;
	mBufferSize   = HlmsTerrainDatablock::MaterialSizeInGpuAligned;

	Ogre::HlmsBufferManager::_changeRenderSystem(newRs);

	if (newRs) {
		HlmsDatablockMap::const_iterator itor = mDatablocks.begin();
		HlmsDatablockMap::const_iterator end  = mDatablocks.end();

		while (itor != end) {
			assert(dynamic_cast<HlmsTerrainDatablock*>(itor->second.datablock));
			HlmsTerrainDatablock* datablock = static_cast<HlmsTerrainDatablock*>(itor->second.datablock);

			requestSlot(datablock->mTextureHash, datablock, false);
			++itor;
		}

		Ogre::HlmsSamplerblock samplerblock;
		samplerblock.mU            = Ogre::TAM_BORDER;
		samplerblock.mV            = Ogre::TAM_BORDER;
		samplerblock.mW            = Ogre::TAM_CLAMP;
		samplerblock.mBorderColour = Ogre::ColourValue(std::numeric_limits<float>::max(),
		                                               std::numeric_limits<float>::max(),
		                                               std::numeric_limits<float>::max(),
		                                               std::numeric_limits<float>::max());

		if (mShaderProfile != "hlsl") {
			samplerblock.mMinFilter = Ogre::FO_POINT;
			samplerblock.mMagFilter = Ogre::FO_POINT;
			samplerblock.mMipFilter = Ogre::FO_NONE;

			if (!mShadowmapSamplerblock)
				mShadowmapSamplerblock = mHlmsManager->getSamplerblock(samplerblock);
		}

		samplerblock.mMinFilter       = Ogre::FO_LINEAR;
		samplerblock.mMagFilter       = Ogre::FO_LINEAR;
		samplerblock.mMipFilter       = Ogre::FO_NONE;
		samplerblock.mCompareFunction = Ogre::CMPF_LESS_EQUAL;

		if (!mShadowmapCmpSamplerblock)
			mShadowmapCmpSamplerblock = mHlmsManager->getSamplerblock(samplerblock);

		if (!mTerrainSamplerblock)
			mTerrainSamplerblock = mHlmsManager->getSamplerblock(Ogre::HlmsSamplerblock());
	}
}
//-----------------------------------------------------------------------------------
const Ogre::HlmsCache* HlmsTerrain::createShaderCacheEntry(Ogre::uint32                  renderableHash,
                                                           const Ogre::HlmsCache&        passCache,
                                                           Ogre::uint32                  finalHash,
                                                           const Ogre::QueuedRenderable& queuedRenderable) {
	const Ogre::HlmsCache* retVal = Hlms::createShaderCacheEntry(renderableHash, passCache, finalHash, queuedRenderable);

	if (mShaderProfile == "hlsl") {
		mListener->shaderCacheEntryCreated(mShaderProfile, retVal, passCache, mSetProperties, queuedRenderable);
		return retVal; //D3D embeds the texture slots in the shader.
	}

	//Set samplers.
	if (!retVal->pso.pixelShader.isNull()) {
		Ogre::GpuProgramParametersSharedPtr psParams = retVal->pso.pixelShader->getDefaultParameters();

		int texUnit = 3; //Vertex shader consumes 1 slot with its heightmap,
		                 //PS consumes 2 slot with its normalmap & shadow texture.

		psParams->setNamedConstant("terrainNormals", 1);
		psParams->setNamedConstant("terrainShadows", 2);

		//Forward3D consumes 2 more slots.
		if (mGridBuffer) {
			psParams->setNamedConstant("f3dGrid", 3);
			psParams->setNamedConstant("f3dLightList", 4);
			texUnit += 2;
		}

		if (!mPreparedPass.shadowMaps.empty()) {
			char           tmpData[32];
			Ogre::LwString texName   = Ogre::LwString::FromEmptyPointer(tmpData, sizeof(tmpData));
			texName                  = "texShadowMap";
			const size_t baseTexSize = texName.size();

			Ogre::vector<int>::type shadowMaps;
			shadowMaps.reserve(mPreparedPass.shadowMaps.size());
			for (size_t i = 0; i < mPreparedPass.shadowMaps.size(); ++i) {
				texName.resize(baseTexSize);
				texName.a((Ogre::uint32) i); //texShadowMap0
				psParams->setNamedConstant(texName.c_str(), &texUnit, 1, 1);
				shadowMaps.push_back(texUnit++);
			}
		}

		assert(dynamic_cast<const HlmsTerrainDatablock*>(queuedRenderable.renderable->getDatablock()));
		const HlmsTerrainDatablock* datablock = static_cast<const HlmsTerrainDatablock*>(
		        queuedRenderable.renderable->getDatablock());

		int numTextures = getProperty(TerrainProperty::NumTextures);
		for (int i = 0; i < numTextures; ++i) {
			psParams->setNamedConstant("textureMaps[" + Ogre::StringConverter::toString(i) + "]",
			                           texUnit++);
		}

		if (getProperty(TerrainProperty::EnvProbeMap)) {
			assert(!datablock->getTexture(TERRAIN_REFLECTION).isNull());
			psParams->setNamedConstant("texEnvProbeMap", texUnit++);
		}
	}

	Ogre::GpuProgramParametersSharedPtr vsParams = retVal->pso.vertexShader->getDefaultParameters();
	vsParams->setNamedConstant("heightMap", 0);

	mListener->shaderCacheEntryCreated(mShaderProfile, retVal, passCache, mSetProperties, queuedRenderable);

	mRenderSystem->_setPipelineStateObject(&retVal->pso);

	mRenderSystem->bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM, vsParams, Ogre::GPV_ALL);
	if (!retVal->pso.pixelShader.isNull()) {
		Ogre::GpuProgramParametersSharedPtr psParams = retVal->pso.pixelShader->getDefaultParameters();
		mRenderSystem->bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM, psParams, Ogre::GPV_ALL);
	}

	return retVal;
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::setDetailMapProperties(HlmsTerrainDatablock* datablock, Ogre::PiecesMap* inOutPieces) {
	Ogre::uint32 minNormalMap   = 4;
	bool         hasDiffuseMaps = false;
	bool         hasNormalMaps  = false;
	for (size_t i = 0; i < 4; ++i) {
		setDetailTextureProperty(TerrainProperty::DetailMapN, datablock, TERRAIN_DETAIL0, i);
		setDetailTextureProperty(TerrainProperty::DetailMapNmN, datablock, TERRAIN_DETAIL0_NM, i);
		setDetailTextureProperty(TerrainProperty::RoughnessMap, datablock, TERRAIN_DETAIL_ROUGHNESS0, i);
		setDetailTextureProperty(TerrainProperty::MetalnessMap, datablock, TERRAIN_DETAIL_METALNESS0, i);

		if (!datablock->getTexture(TERRAIN_DETAIL0 + i).isNull()) {
			hasDiffuseMaps = true;
		}

		if (!datablock->getTexture(TERRAIN_DETAIL0_NM + i).isNull()) {
			minNormalMap  = std::min<Ogre::uint32>(minNormalMap, i);
			hasNormalMaps = true;
		}

		if (datablock->mDetailsOffsetScale[i] != Ogre::Vector4(0, 0, 1, 1))
			setProperty(*TerrainProperty::DetailOffsetsPtrs[i], 1);
	}

	if (hasDiffuseMaps)
		setProperty(TerrainProperty::DetailMapsDiffuse, 4);

	if (hasNormalMaps)
		setProperty(TerrainProperty::DetailMapsNormal, 4);

	setProperty(TerrainProperty::FirstValidDetailMapNm, minNormalMap);
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::setTextureProperty(const char* propertyName, HlmsTerrainDatablock* datablock, TerrainTextureTypes texType) {
	const Ogre::uint8 idx = datablock->getBakedTextureIdx(texType);
	if (idx != NUM_TERRAIN_TEXTURE_TYPES) {
		char           tmpData[64];
		Ogre::LwString propName = Ogre::LwString::FromEmptyPointer(tmpData, sizeof(tmpData));

		propName = propertyName; //diffuse_map

		//In the template the we subtract the "+1" for the index.
		//We need to increment it now otherwise @property( diffuse_map )
		//can translate to @property( 0 ) which is not what we want.
		setProperty(propertyName, idx + 1);

		propName.a("_idx"); //diffuse_map_idx
		setProperty(propName.c_str(), idx);

		propName.a("_slice"); //diffuse_map_idx_slice
		setProperty(propName.c_str(), datablock->_getTextureSliceArrayIndex(texType));
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::setDetailTextureProperty(const char* propertyName, HlmsTerrainDatablock* datablock, TerrainTextureTypes baseTexType, Ogre::uint8 detailIdx) {
	const TerrainTextureTypes texType = static_cast<TerrainTextureTypes>(baseTexType + detailIdx);
	const Ogre::uint8         idx     = datablock->getBakedTextureIdx(texType);
	if (idx != NUM_TERRAIN_TEXTURE_TYPES) {
		char           tmpData[64];
		Ogre::LwString propName = Ogre::LwString::FromEmptyPointer(tmpData, sizeof(tmpData));

		propName.a(propertyName, detailIdx); //detail_map0

		//In the template the we subtract the "+1" for the index.
		//We need to increment it now otherwise @property( diffuse_map )
		//can translate to @property( 0 ) which is not what we want.
		setProperty(propName.c_str(), idx + 1);

		propName.a("_idx"); //detail_map0_idx
		setProperty(propName.c_str(), idx);

		propName.a("_slice"); //detail_map0_idx_slice
		setProperty(propName.c_str(), datablock->_getTextureSliceArrayIndex(texType));
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::calculateHashForPreCreate(Ogre::Renderable* renderable, Ogre::PiecesMap* inOutPieces) {
	assert(dynamic_cast<TerrainCell*>(renderable) &&
	       "This Hlms can only be used on a Terrain object!");

	TerrainCell* terrainCell = static_cast<TerrainCell*>(renderable);
	setProperty(TerrainProperty::UseSkirts, terrainCell->getUseSkirts());

	assert(dynamic_cast<HlmsTerrainDatablock*>(renderable->getDatablock()));
	HlmsTerrainDatablock* datablock = static_cast<HlmsTerrainDatablock*>(
	        renderable->getDatablock());

	setProperty(TerrainProperty::ReceiveShadows, 1);

	Ogre::uint32 brdf = datablock->getBrdf();
	if ((brdf & TerrainBrdf::BRDF_MASK) == TerrainBrdf::Default) {
		setProperty(TerrainProperty::BrdfDefault, 1);

		if (!(brdf & TerrainBrdf::FLAG_UNCORRELATED))
			setProperty(TerrainProperty::GgxHeightCorrelated, 1);
	}
	else if ((brdf & TerrainBrdf::BRDF_MASK) == TerrainBrdf::CookTorrance)
		setProperty(TerrainProperty::BrdfCookTorrance, 1);

	if (brdf & TerrainBrdf::FLAG_SPERATE_DIFFUSE_FRESNEL)
		setProperty(TerrainProperty::FresnelSeparateDiffuse, 1);

	{
		size_t validDetailMaps = 0;
		for (size_t i = 0; i < 4; ++i) {
			if (!datablock->getTexture(TERRAIN_DETAIL0_NM + i).isNull())
				++validDetailMaps;
		}
	}

	setDetailMapProperties(datablock, inOutPieces);

	bool envMap = datablock->getBakedTextureIdx(TERRAIN_REFLECTION) != NUM_TERRAIN_TEXTURE_TYPES;

	setProperty(TerrainProperty::NumTextures, datablock->mBakedTextures.size() - envMap);

	setTextureProperty(TerrainProperty::DiffuseMap, datablock, TERRAIN_DIFFUSE);
	setTextureProperty(TerrainProperty::EnvProbeMap, datablock, TERRAIN_REFLECTION);
	setTextureProperty(TerrainProperty::DetailWeightMap, datablock, TERRAIN_DETAIL_WEIGHT);

	Ogre::String slotsPerPoolStr                                         = Ogre::StringConverter::toString(mSlotsPerPool);
	inOutPieces[Ogre::VertexShader][TerrainProperty::MaterialsPerBuffer] = slotsPerPoolStr;
	inOutPieces[Ogre::PixelShader][TerrainProperty::MaterialsPerBuffer]  = slotsPerPoolStr;
}
//-----------------------------------------------------------------------------------
Ogre::HlmsCache HlmsTerrain::preparePassHash(const Ogre::CompositorShadowNode* shadowNode, bool casterPass, bool dualParaboloid, Ogre::SceneManager* sceneManager) {
	mSetProperties.clear();

	if (casterPass) {
		Ogre::HlmsCache retVal = Ogre::Hlms::preparePassHashBase(shadowNode, casterPass, dualParaboloid, sceneManager);
		return retVal;
	}

	//The properties need to be set before preparePassHash so that
	//they are considered when building the Ogre::HlmsCache's hash.
	if (shadowNode && !casterPass) {
		//Shadow receiving can be improved in performance by using gather sampling.
		//(it's the only feature so far that uses gather)
		const Ogre::RenderSystemCapabilities* capabilities = mRenderSystem->getCapabilities();
		if (capabilities->hasCapability(Ogre::RSC_TEXTURE_GATHER))
			setProperty(Ogre::HlmsBaseProp::TexGather, 1);

		if (mShadowFilter == PCF_3x3) {
			setProperty(TerrainProperty::Pcf3x3, 1);
			setProperty(TerrainProperty::PcfIterations, 4);
		}
		else if (mShadowFilter == PCF_4x4) {
			setProperty(TerrainProperty::Pcf4x4, 1);
			setProperty(TerrainProperty::PcfIterations, 9);
		}
		else {
			setProperty(TerrainProperty::PcfIterations, 1);
		}

		if (mDebugPssmSplits) {
			Ogre::int32                           numPssmSplits = 0;
			const Ogre::vector<Ogre::Real>::type* pssmSplits    = shadowNode->getPssmSplits(0);
			if (pssmSplits) {
				numPssmSplits = static_cast<Ogre::int32>(pssmSplits->size() - 1);
				if (numPssmSplits > 0)
					setProperty(TerrainProperty::DebugPssmSplits, 1);
			}
		}
	}

	AmbientLightMode  ambientMode     = mAmbientLightMode;
	Ogre::ColourValue upperHemisphere = sceneManager->getAmbientLightUpperHemisphere();
	Ogre::ColourValue lowerHemisphere = sceneManager->getAmbientLightLowerHemisphere();

	const float envMapScale = upperHemisphere.a;
	//Ignore alpha channel
	upperHemisphere.a = lowerHemisphere.a = 1.0;

	if (!casterPass) {
		if (mAmbientLightMode == AmbientAuto) {
			if (upperHemisphere == lowerHemisphere) {
				if (upperHemisphere == Ogre::ColourValue::Black)
					ambientMode = AmbientNone;
				else
					ambientMode = AmbientFixed;
			}
			else {
				ambientMode = AmbientHemisphere;
			}
		}

		if (ambientMode == AmbientFixed)
			setProperty(TerrainProperty::AmbientFixed, 1);
		if (ambientMode == AmbientHemisphere)
			setProperty(TerrainProperty::AmbientHemisphere, 1);

		if (envMapScale != 1.0f)
			setProperty(TerrainProperty::EnvMapScale, 1);
	}

	setProperty(TerrainProperty::FresnelScalar, 1);
	setProperty(TerrainProperty::MetallicWorkflow, 1);

	Ogre::HlmsCache retVal = Hlms::preparePassHashBase(shadowNode, casterPass, dualParaboloid, sceneManager);

	if (getProperty(Ogre::HlmsBaseProp::LightsDirNonCaster) > 0) {
		//First directional light always cast shadows thanks to our terrain shadows.
		Ogre::int32 shadowCasterDirectional = getProperty(Ogre::HlmsBaseProp::LightsDirectional);
		shadowCasterDirectional             = std::max(shadowCasterDirectional, 1);
		setProperty(Ogre::HlmsBaseProp::LightsDirectional, shadowCasterDirectional);
	}

	Ogre::RenderTarget* renderTarget = sceneManager->getCurrentViewport()->getTarget();

	const Ogre::RenderSystemCapabilities* capabilities = mRenderSystem->getCapabilities();
	setProperty(TerrainProperty::HwGammaRead, capabilities->hasCapability(Ogre::RSC_HW_GAMMA));
	setProperty(TerrainProperty::HwGammaWrite, capabilities->hasCapability(Ogre::RSC_HW_GAMMA) && renderTarget->isHardwareGammaEnabled());
	setProperty(TerrainProperty::SignedIntTex, capabilities->hasCapability(Ogre::RSC_TEXTURE_SIGNED_INT));
	retVal.setProperties = mSetProperties;

	Ogre::Camera* camera     = sceneManager->getCameraInProgress();
	Ogre::Matrix4 viewMatrix = camera->getViewMatrix(true);

	Ogre::Matrix4 projectionMatrix = camera->getProjectionMatrixWithRSDepth();

	if (renderTarget->requiresTextureFlipping()) {
		projectionMatrix[1][0] = -projectionMatrix[1][0];
		projectionMatrix[1][1] = -projectionMatrix[1][1];
		projectionMatrix[1][2] = -projectionMatrix[1][2];
		projectionMatrix[1][3] = -projectionMatrix[1][3];
	}

	Ogre::int32 numLights            = getProperty(Ogre::HlmsBaseProp::LightsSpot);
	Ogre::int32 numDirectionalLights = getProperty(Ogre::HlmsBaseProp::LightsDirNonCaster);
	Ogre::int32 numShadowMapLights   = getProperty(Ogre::HlmsBaseProp::NumShadowMapLights);
	Ogre::int32 numPssmSplits        = getProperty(Ogre::HlmsBaseProp::PssmSplits);

	//mat4 viewProj + mat4 view;
	size_t mapSize = (16 + 16) * 4;

	mGridBuffer            = 0;
	mGlobalLightListBuffer = 0;

	Ogre::ForwardPlusBase* forwardPlus = sceneManager->_getActivePassForwardPlus();
	if (forwardPlus) {
		mapSize += forwardPlus->getConstBufferSize();
		mGridBuffer            = forwardPlus->getGridBuffer(camera);
		mGlobalLightListBuffer = forwardPlus->getGlobalLightListBuffer(camera);
	}

	//mat4 shadowRcv[numShadowMapLights].texViewProj +
	//              vec2 shadowRcv[numShadowMapLights].shadowDepthRange +
	//              vec2 padding +
	//              vec4 shadowRcv[numShadowMapLights].invShadowMapSize +
	//mat3 invViewMatCubemap (upgraded to three vec4)
	mapSize += ((16 + 2 + 2 + 4) * numShadowMapLights + 4 * 3) * 4;

	//vec3 ambientUpperHemi + float envMapScale
	if (ambientMode == AmbientFixed || ambientMode == AmbientHemisphere || envMapScale != 1.0f)
		mapSize += 4 * 4;

	//vec3 ambientLowerHemi + padding + vec3 ambientHemisphereDir + padding
	if (ambientMode == AmbientHemisphere)
		mapSize += 8 * 4;

	//float pssmSplitPoints N times.
	mapSize += numPssmSplits * 4;
	mapSize = Ogre::alignToNextMultiple(mapSize, 16);

	if (shadowNode) {
		//Six variables * 4 (padded vec3) * 4 (bytes) * numLights
		mapSize += (6 * 4 * 4) * numLights;
	}
	else {
		//Three variables * 4 (padded vec3) * 4 (bytes) * numDirectionalLights
		mapSize += (3 * 4 * 4) * numDirectionalLights;
	}

	mapSize += mListener->getPassBufferSize(shadowNode, casterPass, dualParaboloid, sceneManager);

	//Arbitrary 8kb, should be enough.
	const size_t maxBufferSize = 8 * 1024;

	assert(mapSize <= maxBufferSize);

	if (mCurrentPassBuffer >= mPassBuffers.size()) {
		mPassBuffers.push_back(mVaoManager->createConstBuffer(maxBufferSize, Ogre::BT_DYNAMIC_PERSISTENT, 0, false));
	}

	Ogre::ConstBufferPacked* passBuffer    = mPassBuffers[mCurrentPassBuffer++];
	float*                   passBufferPtr = reinterpret_cast<float*>(passBuffer->map(0, mapSize));

#ifndef NDEBUG
	const float* startupPtr = passBufferPtr;
#endif

	//---------------------------------------------------------------------------
	//                          ---- VERTEX SHADER ----
	//---------------------------------------------------------------------------

	//mat4 viewProj;
	Ogre::Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
	for (size_t i = 0; i < 16; ++i)
		*passBufferPtr++ = (float) viewProjMatrix[0][i];

	mPreparedPass.viewMatrix = viewMatrix;

	mPreparedPass.shadowMaps.clear();

	//mat4 view;
	for (size_t i = 0; i < 16; ++i)
		*passBufferPtr++ = (float) viewMatrix[0][i];

	size_t                  shadowMapTexIdx        = 0;
	const Ogre::TextureVec& contiguousShadowMapTex = shadowNode->getContiguousShadowMapTex();

	for (Ogre::int32 i = 0; i < numShadowMapLights; ++i) {
		//Skip inactive lights (e.g. no directional lights are available
		//and there's a shadow map that only accepts dir lights)
		while (!shadowNode->isShadowMapIdxActive(shadowMapTexIdx))
			++shadowMapTexIdx;

		//mat4 shadowRcv[numShadowMapLights].texViewProj
		Ogre::Matrix4 viewProjTex = shadowNode->getViewProjectionMatrix(shadowMapTexIdx);
		for (size_t j = 0; j < 16; ++j)
			*passBufferPtr++ = (float) viewProjTex[0][j];

		//vec2 shadowRcv[numShadowMapLights].shadowDepthRange
		Ogre::Real fNear, fFar;
		shadowNode->getMinMaxDepthRange(shadowMapTexIdx, fNear, fFar);
		const Ogre::Real depthRange = fFar - fNear;
		*passBufferPtr++            = fNear;
		*passBufferPtr++            = 1.0f / depthRange;
		++passBufferPtr; //Padding
		++passBufferPtr; //Padding

		//vec2 shadowRcv[numShadowMapLights].invShadowMapSize
		size_t shadowMapTexContigIdx =
		        shadowNode->getIndexToContiguousShadowMapTex((size_t) shadowMapTexIdx);
		Ogre::uint32 texWidth  = contiguousShadowMapTex[shadowMapTexContigIdx]->getWidth();
		Ogre::uint32 texHeight = contiguousShadowMapTex[shadowMapTexContigIdx]->getHeight();
		*passBufferPtr++       = 1.0f / texWidth;
		*passBufferPtr++       = 1.0f / texHeight;
		*passBufferPtr++       = static_cast<float>(texWidth);
		*passBufferPtr++       = static_cast<float>(texHeight);

		++shadowMapTexIdx;
	}

	//---------------------------------------------------------------------------
	//                          ---- PIXEL SHADER ----
	//---------------------------------------------------------------------------

	Ogre::Matrix3 viewMatrix3, invViewMatrix3;
	viewMatrix.extract3x3Matrix(viewMatrix3);
	invViewMatrix3 = viewMatrix3.Inverse();

	//mat3 invViewMatCubemap
	for (size_t i = 0; i < 9; ++i) {
#ifdef OGRE_GLES2_WORKAROUND_2
		Ogre::Matrix3 xRot(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
		xRot             = xRot * invViewMatrix3;
		*passBufferPtr++ = (float) xRot[0][i];
#else
		*passBufferPtr++ = (float) invViewMatrix3[0][i];
#endif

		//Alignment: each row/column is one vec4, despite being 3x3
		if (!((i + 1) % 3))
			++passBufferPtr;
	}

	//vec3 ambientUpperHemi + padding
	if (ambientMode == AmbientFixed || ambientMode == AmbientHemisphere || envMapScale != 1.0f) {
		*passBufferPtr++ = static_cast<float>(upperHemisphere.r);
		*passBufferPtr++ = static_cast<float>(upperHemisphere.g);
		*passBufferPtr++ = static_cast<float>(upperHemisphere.b);
		*passBufferPtr++ = envMapScale;
	}

	//vec3 ambientLowerHemi + padding + vec3 ambientHemisphereDir + padding
	if (ambientMode == AmbientHemisphere) {
		*passBufferPtr++ = static_cast<float>(lowerHemisphere.r);
		*passBufferPtr++ = static_cast<float>(lowerHemisphere.g);
		*passBufferPtr++ = static_cast<float>(lowerHemisphere.b);
		*passBufferPtr++ = 1.0f;

		Ogre::Vector3 hemisphereDir = viewMatrix3 * sceneManager->getAmbientLightHemisphereDir();
		hemisphereDir.normalise();
		*passBufferPtr++ = static_cast<float>(hemisphereDir.x);
		*passBufferPtr++ = static_cast<float>(hemisphereDir.y);
		*passBufferPtr++ = static_cast<float>(hemisphereDir.z);
		*passBufferPtr++ = 1.0f;
	}

	//float pssmSplitPoints
	for (Ogre::int32 i = 0; i < numPssmSplits; ++i)
		*passBufferPtr++ = (*shadowNode->getPssmSplits(0))[i + 1];

	passBufferPtr += Ogre::alignToNextMultiple(numPssmSplits, 4) - numPssmSplits;

	if (numShadowMapLights > 0) {
		//All directional lights (caster and non-caster) are sent.
		//Then non-directional shadow-casting shadow lights are sent.
		size_t                         shadowLightIdx    = 0;
		size_t                         nonShadowLightIdx = 0;
		const Ogre::LightListInfo&     globalLightList   = sceneManager->getGlobalLightList();
		const Ogre::LightClosestArray& lights            = shadowNode->getShadowCastingLights();

		const Ogre::CompositorShadowNode::LightsBitSet& affectedLights =
		        shadowNode->getAffectedLightsBitSet();

		Ogre::int32 shadowCastingDirLights = getProperty(Ogre::HlmsBaseProp::LightsDirectional);

		for (Ogre::int32 i = 0; i < numLights; ++i) {
			Ogre::Light const* light = 0;

			if (i >= shadowCastingDirLights && i < numDirectionalLights) {
				while (affectedLights[nonShadowLightIdx])
					++nonShadowLightIdx;

				light = globalLightList.lights[nonShadowLightIdx++];

				assert(light->getType() == Ogre::Light::LT_DIRECTIONAL);
			}
			else {
				//Skip inactive lights (e.g. no directional lights are available
				//and there's a shadow map that only accepts dir lights)
				while (!lights[shadowLightIdx].light)
					++shadowLightIdx;
				light = lights[shadowLightIdx++].light;
			}

			Ogre::Vector4 lightPos4 = light->getAs4DVector();
			Ogre::Vector3 lightPos;

			if (light->getType() == Ogre::Light::LT_DIRECTIONAL)
				lightPos = viewMatrix3 * Ogre::Vector3(lightPos4.x, lightPos4.y, lightPos4.z);
			else
				lightPos = viewMatrix * Ogre::Vector3(lightPos4.x, lightPos4.y, lightPos4.z);

			//vec3 lights[numLights].position
			*passBufferPtr++ = lightPos.x;
			*passBufferPtr++ = lightPos.y;
			*passBufferPtr++ = lightPos.z;
			++passBufferPtr;

			//vec3 lights[numLights].diffuse
			Ogre::ColourValue colour = light->getDiffuseColour() *
			                           light->getPowerScale();
			*passBufferPtr++ = colour.r;
			*passBufferPtr++ = colour.g;
			*passBufferPtr++ = colour.b;
			++passBufferPtr;

			//vec3 lights[numLights].specular
			colour           = light->getSpecularColour() * light->getPowerScale();
			*passBufferPtr++ = colour.r;
			*passBufferPtr++ = colour.g;
			*passBufferPtr++ = colour.b;
			++passBufferPtr;

			//vec3 lights[numLights].attenuation;
			Ogre::Real attenRange     = light->getAttenuationRange();
			Ogre::Real attenLinear    = light->getAttenuationLinear();
			Ogre::Real attenQuadratic = light->getAttenuationQuadric();
			*passBufferPtr++          = attenRange;
			*passBufferPtr++          = attenLinear;
			*passBufferPtr++          = attenQuadratic;
			++passBufferPtr;

			//vec3 lights[numLights].spotDirection;
			Ogre::Vector3 spotDir = viewMatrix3 * light->getDerivedDirection();
			*passBufferPtr++      = spotDir.x;
			*passBufferPtr++      = spotDir.y;
			*passBufferPtr++      = spotDir.z;
			++passBufferPtr;

			//vec3 lights[numLights].spotParams;
			Ogre::Radian innerAngle = light->getSpotlightInnerAngle();
			Ogre::Radian outerAngle = light->getSpotlightOuterAngle();
			*passBufferPtr++        = 1.0f / (cosf(innerAngle.valueRadians() * 0.5f) -
			                           cosf(outerAngle.valueRadians() * 0.5f));
			*passBufferPtr++ = cosf(outerAngle.valueRadians() * 0.5f);
			*passBufferPtr++ = light->getSpotlightFalloff();
			++passBufferPtr;
		}
	}
	else {
		//No shadow maps, only send directional lights
		const Ogre::LightListInfo& globalLightList = sceneManager->getGlobalLightList();

		for (Ogre::int32 i = 0; i < numDirectionalLights; ++i) {
			assert(globalLightList.lights[i]->getType() == Ogre::Light::LT_DIRECTIONAL);

			Ogre::Vector4 lightPos4 = globalLightList.lights[i]->getAs4DVector();
			Ogre::Vector3 lightPos  = viewMatrix3 * Ogre::Vector3(lightPos4.x, lightPos4.y, lightPos4.z);

			//vec3 lights[numLights].position
			*passBufferPtr++ = lightPos.x;
			*passBufferPtr++ = lightPos.y;
			*passBufferPtr++ = lightPos.z;
			++passBufferPtr;

			//vec3 lights[numLights].diffuse
			Ogre::ColourValue colour = globalLightList.lights[i]->getDiffuseColour() *
			                           globalLightList.lights[i]->getPowerScale();
			*passBufferPtr++ = colour.r;
			*passBufferPtr++ = colour.g;
			*passBufferPtr++ = colour.b;
			++passBufferPtr;

			//vec3 lights[numLights].specular
			colour           = globalLightList.lights[i]->getSpecularColour() * globalLightList.lights[i]->getPowerScale();
			*passBufferPtr++ = colour.r;
			*passBufferPtr++ = colour.g;
			*passBufferPtr++ = colour.b;
			++passBufferPtr;
		}
	}

	if (shadowNode) {
		mPreparedPass.shadowMaps.reserve(contiguousShadowMapTex.size());

		Ogre::TextureVec::const_iterator itShadowMap = contiguousShadowMapTex.begin();
		Ogre::TextureVec::const_iterator enShadowMap = contiguousShadowMapTex.end();

		while (itShadowMap != enShadowMap) {
			mPreparedPass.shadowMaps.push_back(itShadowMap->get());
			++itShadowMap;
		}
	}

	if (forwardPlus) {
		forwardPlus->fillConstBufferData(renderTarget, passBufferPtr);
		passBufferPtr += forwardPlus->getConstBufferSize() >> 2;
	}

	passBufferPtr = mListener->preparePassBuffer(shadowNode, casterPass, dualParaboloid, sceneManager, passBufferPtr);

	assert((size_t)(passBufferPtr - startupPtr) * 4u == mapSize);

	passBuffer->unmap(Ogre::UO_KEEP_PERSISTENT);

	mLastTextureHash   = 0;
	mLastMovableObject = 0;

	mLastBoundPool = 0;

	if (mShadowmapSamplerblock && !getProperty(Ogre::HlmsBaseProp::ShadowUsesDepthTexture))
		mCurrentShadowmapSamplerblock = mShadowmapSamplerblock;
	else
		mCurrentShadowmapSamplerblock = mShadowmapCmpSamplerblock;

	uploadDirtyDatablocks();

	return retVal;
}
//-----------------------------------------------------------------------------------
Ogre::uint32 HlmsTerrain::fillBuffersFor(const Ogre::HlmsCache* cache, const Ogre::QueuedRenderable& queuedRenderable, bool casterPass, Ogre::uint32 lastCacheHash, Ogre::uint32 lastTextureHash) {
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
	            "Trying to use slow-path on a desktop implementation. "
	            "Change the RenderQueue settings.",
	            "HlmsTerrain::fillBuffersFor");
}
//-----------------------------------------------------------------------------------
Ogre::uint32 HlmsTerrain::fillBuffersForV1(const Ogre::HlmsCache*        cache,
                                           const Ogre::QueuedRenderable& queuedRenderable,
                                           bool                          casterPass,
                                           Ogre::uint32                  lastCacheHash,
                                           Ogre::CommandBuffer*          commandBuffer) {
	return fillBuffersFor(cache, queuedRenderable, casterPass, lastCacheHash, commandBuffer, true);
}
//-----------------------------------------------------------------------------------
Ogre::uint32 HlmsTerrain::fillBuffersForV2(const Ogre::HlmsCache*        cache,
                                           const Ogre::QueuedRenderable& queuedRenderable,
                                           bool                          casterPass,
                                           Ogre::uint32                  lastCacheHash,
                                           Ogre::CommandBuffer*          commandBuffer) {
	return fillBuffersFor(cache, queuedRenderable, casterPass, lastCacheHash, commandBuffer, false);
}
//-----------------------------------------------------------------------------------
Ogre::uint32 HlmsTerrain::fillBuffersFor(const Ogre::HlmsCache* cache, const Ogre::QueuedRenderable& queuedRenderable, bool casterPass, Ogre::uint32 lastCacheHash, Ogre::CommandBuffer* commandBuffer, bool isV1) {
	assert(dynamic_cast<const HlmsTerrainDatablock*>(queuedRenderable.renderable->getDatablock()));
	const HlmsTerrainDatablock* datablock = static_cast<const HlmsTerrainDatablock*>(
	        queuedRenderable.renderable->getDatablock());

	if (OGRE_EXTRACT_HLMS_TYPE_FROM_CACHE_HASH(lastCacheHash) != Ogre::HLMS_USER3) {
		//layout(binding = 0) uniform PassBuffer {} pass
		Ogre::ConstBufferPacked* passBuffer                = mPassBuffers[mCurrentPassBuffer - 1];
		*commandBuffer->addCommand<Ogre::CbShaderBuffer>() = Ogre::CbShaderBuffer(Ogre::VertexShader,
		                                                                          0,
		                                                                          passBuffer,
		                                                                          0,
		                                                                          passBuffer->getTotalSizeBytes());
		*commandBuffer->addCommand<Ogre::CbShaderBuffer>() = Ogre::CbShaderBuffer(Ogre::PixelShader,
		                                                                          0,
		                                                                          passBuffer,
		                                                                          0,
		                                                                          passBuffer->getTotalSizeBytes());

		size_t texUnit = 3;

		if (mGridBuffer) {
			texUnit = 5;
			*commandBuffer->addCommand<Ogre::CbShaderBuffer>() =
			        Ogre::CbShaderBuffer(Ogre::PixelShader, 3, mGridBuffer, 0, 0);
			*commandBuffer->addCommand<Ogre::CbShaderBuffer>() =
			        Ogre::CbShaderBuffer(Ogre::PixelShader, 4, mGlobalLightListBuffer, 0, 0);
		}

		//We changed HlmsType, rebind the shared textures.
		Ogre::FastArray<Ogre::Texture*>::const_iterator itor = mPreparedPass.shadowMaps.begin();
		Ogre::FastArray<Ogre::Texture*>::const_iterator end  = mPreparedPass.shadowMaps.end();
		while (itor != end) {
			*commandBuffer->addCommand<Ogre::CbTexture>() = Ogre::CbTexture(texUnit, true, *itor, mCurrentShadowmapSamplerblock);
			++texUnit;
			++itor;
		}

		mLastTextureHash   = 0;
		mLastMovableObject = 0;
		mLastBoundPool     = 0;

		//layout(binding = 2) uniform InstanceBuffer {} instance
		if (mCurrentConstBuffer < mConstBuffers.size() &&
		    (size_t)((mCurrentMappedConstBuffer - mStartMappedConstBuffer) + 16) <=
		            mCurrentConstBufferSize) {
			*commandBuffer->addCommand<Ogre::CbShaderBuffer>() =
			        Ogre::CbShaderBuffer(Ogre::VertexShader, 2, mConstBuffers[mCurrentConstBuffer], 0, 0);
			*commandBuffer->addCommand<Ogre::CbShaderBuffer>() =
			        Ogre::CbShaderBuffer(Ogre::PixelShader, 2, mConstBuffers[mCurrentConstBuffer], 0, 0);
		}

		mListener->hlmsTypeChanged(casterPass, commandBuffer, datablock);
	}

	//Don't bind the material buffer on caster passes (important to keep
	//MDI & auto-instancing running on shadow map passes)
	if (mLastBoundPool != datablock->getAssignedPool() &&
	    (!casterPass || datablock->getAlphaTest() != Ogre::CMPF_ALWAYS_PASS)) {
		//layout(binding = 1) uniform MaterialBuf {} materialArray
		const Ogre::ConstBufferPool::BufferPool* newPool   = datablock->getAssignedPool();
		*commandBuffer->addCommand<Ogre::CbShaderBuffer>() = Ogre::CbShaderBuffer(Ogre::PixelShader,
		                                                                          1,
		                                                                          newPool->materialBuffer,
		                                                                          0,
		                                                                          newPool->materialBuffer->getTotalSizeBytes());
		mLastBoundPool = newPool;
	}

	if (mLastMovableObject != queuedRenderable.movableObject) {
		//Different Terrain? Must change textures then.
		const GraphicsTerrain* terraObj = static_cast<const GraphicsTerrain*>(queuedRenderable.movableObject);
		*commandBuffer->addCommand<Ogre::CbTexture>() =
		        Ogre::CbTexture(0, true, terraObj->getHeightMapTex().get());
		*commandBuffer->addCommand<Ogre::CbTexture>() =
		        Ogre::CbTexture(1, true, terraObj->getNormalMapTex().get(), mTerrainSamplerblock);
		*commandBuffer->addCommand<Ogre::CbTexture>() =
		        Ogre::CbTexture(2, true, terraObj->_getShadowMapTex().get(), mTerrainSamplerblock);

#if OGRE_DEBUG_MODE
		//          Commented: Hack to get a barrier without dealing with the Compositor while debugging.
		//            ResourceTransition resourceTransition;
		//            resourceTransition.readBarrierBits = ReadBarrier::Uav;
		//            resourceTransition.writeBarrierBits = WriteBarrier::Uav;
		//            mRenderSystem->_resourceTransitionCreated( &resourceTransition );
		//            mRenderSystem->_executeResourceTransition( &resourceTransition );
		//            mRenderSystem->_resourceTransitionDestroyed( &resourceTransition );
		Ogre::TexturePtr                     terraShadowText    = terraObj->_getShadowMapTex();
		const CompositorTextureVec&          compositorTextures = queuedRenderable.movableObject->_getManager()->getCompositorTextures();
		CompositorTextureVec::const_iterator itor               = compositorTextures.begin();
		CompositorTextureVec::const_iterator end                = compositorTextures.end();

		while (itor != end && (*itor->textures)[0] != terraShadowText)
			++itor;

		if (itor == end) {
			assert("Hazard Detected! You should expose this Terrain's shadow map texture"
			       " to the compositor pass so Ogre can place the proper Barriers" &&
			       false);
		}
#endif

		mLastMovableObject = queuedRenderable.movableObject;
	}

	Ogre::uint32* RESTRICT_ALIAS currentMappedConstBuffer = mCurrentMappedConstBuffer;

	//---------------------------------------------------------------------------
	//                          ---- VERTEX SHADER ----
	//---------------------------------------------------------------------------
	//We need to correct currentMappedConstBuffer to point to the right texture buffer's
	//offset, which may not be in sync if the previous draw had skeletal animation.
	bool exceedsConstBuffer = (size_t)((currentMappedConstBuffer - mStartMappedConstBuffer) + 12) > mCurrentConstBufferSize;

	if (exceedsConstBuffer)
		currentMappedConstBuffer = mapNextConstBuffer(commandBuffer);

	const TerrainCell* terrainCell = static_cast<const TerrainCell*>(queuedRenderable.renderable);

	terrainCell->uploadToGpu(currentMappedConstBuffer);
	currentMappedConstBuffer += 16u;

	//---------------------------------------------------------------------------
	//                          ---- PIXEL SHADER ----
	//---------------------------------------------------------------------------

	if (!casterPass || datablock->getAlphaTest() != Ogre::CMPF_ALWAYS_PASS) {
		if (datablock->mTextureHash != mLastTextureHash) {
			//Rebind textures
			size_t texUnit = mPreparedPass.shadowMaps.size() + (!mGridBuffer ? 3 : 5);

			TerrainBakedTextureArray::const_iterator itor = datablock->mBakedTextures.begin();
			TerrainBakedTextureArray::const_iterator end  = datablock->mBakedTextures.end();

			while (itor != end) {
				*commandBuffer->addCommand<Ogre::CbTexture>() =
				        Ogre::CbTexture(texUnit++, true, itor->texture.get(), itor->samplerBlock);
				++itor;
			}

			*commandBuffer->addCommand<Ogre::CbTextureDisableFrom>() = Ogre::CbTextureDisableFrom(texUnit);

			mLastTextureHash = datablock->mTextureHash;
		}
	}

	mCurrentMappedConstBuffer = currentMappedConstBuffer;

	return ((mCurrentMappedConstBuffer - mStartMappedConstBuffer) >> 4) - 1;
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::destroyAllBuffers(void) {
	Ogre::HlmsBufferManager::destroyAllBuffers();

	mCurrentPassBuffer = 0;

	{
		Ogre::ConstBufferPackedVec::const_iterator itor = mPassBuffers.begin();
		Ogre::ConstBufferPackedVec::const_iterator end  = mPassBuffers.end();

		while (itor != end) {
			if ((*itor)->getMappingState() != Ogre::MS_UNMAPPED)
				(*itor)->unmap(Ogre::UO_UNMAP_ALL);
			mVaoManager->destroyConstBuffer(*itor);
			++itor;
		}

		mPassBuffers.clear();
	}
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::frameEnded(void) {
	Ogre::HlmsBufferManager::frameEnded();
	mCurrentPassBuffer = 0;
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::setDebugPssmSplits(bool bDebug) {
	mDebugPssmSplits = bDebug;
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::setShadowSettings(ShadowFilter filter) {
	mShadowFilter = filter;
}
//-----------------------------------------------------------------------------------
void HlmsTerrain::setAmbientLightMode(AmbientLightMode mode) {
	mAmbientLightMode = mode;
}
//-----------------------------------------------------------------------------------
Ogre::HlmsDatablock* HlmsTerrain::createDatablockImpl(Ogre::IdString              datablockName,
                                                      const Ogre::HlmsMacroblock* macroblock,
                                                      const Ogre::HlmsBlendblock* blendblock,
                                                      const Ogre::HlmsParamVec&   paramVec) {
	return OGRE_NEW HlmsTerrainDatablock(datablockName, this, macroblock, blendblock, paramVec);
}
