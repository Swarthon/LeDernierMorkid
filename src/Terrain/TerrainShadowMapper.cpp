#include "Terrain/TerrainShadowMapper.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreTextureManager.h>

#include <Compositor/OgreCompositorChannel.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <OgreRenderTexture.h>
#include <OgreSceneManager.h>

#include <OgreHlmsCompute.h>
#include <OgreHlmsComputeJob.h>
#include <OgreHlmsManager.h>
#include <OgreRoot.h>
#include <Vao/OgreConstBufferPacked.h>

#include <OgreLwString.h>

ShadowMapper::ShadowMapper(Ogre::SceneManager* sceneManager, Ogre::CompositorManager2* compositorManager)
                : mShadowStarts(0),
                  mShadowPerGroupData(0),
                  mShadowWorkspace(0),
                  mShadowJob(0),
                  mJobParamDelta(0),
                  mJobParamXYStep(0),
                  mJobParamIsStep(0),
                  mJobParamHeightDelta(0),
                  mSceneManager(sceneManager),
                  mCompositorManager(compositorManager) {
}
//-----------------------------------------------------------------------------------
ShadowMapper::~ShadowMapper() {
	destroyShadowMap();
}
//-----------------------------------------------------------------------------------
void ShadowMapper::createShadowMap(Ogre::IdType id, Ogre::TexturePtr& heightMapTex) {
	destroyShadowMap();

	mHeightMapTex = heightMapTex;

	Ogre::VaoManager* vaoManager = mSceneManager->getDestinationRenderSystem()->getVaoManager();

	if (!mShadowStarts) {
		mShadowStarts = vaoManager->createConstBuffer(4096u * 16u,
		                                              Ogre::BT_DYNAMIC_PERSISTENT,
		                                              0,
		                                              false);
	}
	if (!mShadowPerGroupData) {
		mShadowPerGroupData = vaoManager->createConstBuffer(4096u * 16u,
		                                                    Ogre::BT_DYNAMIC_PERSISTENT,
		                                                    0,
		                                                    false);
	}

	Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();
	Ogre::HlmsCompute* hlmsCompute = hlmsManager->getComputeHlms();
	mShadowJob                     = hlmsCompute->findComputeJob("Terra/ShadowGenerator");

	//TODO: Mipmaps
	mShadowMapTex = Ogre::TextureManager::getSingleton().createManual(
	        "ShadowMap" + Ogre::StringConverter::toString(id),
	        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	        Ogre::TEX_TYPE_2D,
	        mHeightMapTex->getWidth(),
	        mHeightMapTex->getHeight(),
	        0,
	        Ogre::PF_A2B10G10R10,
	        Ogre::TU_RENDERTARGET | Ogre::TU_UAV);

	Ogre::CompositorChannelVec finalTarget(1, Ogre::CompositorChannel());
	finalTarget[0].target = mShadowMapTex->getBuffer(0)->getRenderTarget();
	finalTarget[0].textures.push_back(mShadowMapTex);
	mShadowWorkspace = mCompositorManager->addWorkspace(mSceneManager, finalTarget, 0, "Terra/ShadowGeneratorWorkspace", false);

	Ogre::ShaderParams& shaderParams = mShadowJob->getShaderParams("default");
	mJobParamDelta                   = shaderParams.findParameter("delta");
	mJobParamXYStep                  = shaderParams.findParameter("xyStep");
	mJobParamIsStep                  = shaderParams.findParameter("isSteep");
	mJobParamHeightDelta             = shaderParams.findParameter("heightDelta");

	setGaussianFilterParams(8, 0.5f);
}
//-----------------------------------------------------------------------------------
void ShadowMapper::destroyShadowMap(void) {
	mHeightMapTex.setNull();

	Ogre::VaoManager* vaoManager = mSceneManager->getDestinationRenderSystem()->getVaoManager();

	if (mShadowStarts) {
		if (mShadowStarts->getMappingState() != Ogre::MS_UNMAPPED)
			mShadowStarts->unmap(Ogre::UO_UNMAP_ALL);
		vaoManager->destroyConstBuffer(mShadowStarts);
		mShadowStarts = 0;
	}

	if (mShadowPerGroupData) {
		if (mShadowPerGroupData->getMappingState() != Ogre::MS_UNMAPPED)
			mShadowPerGroupData->unmap(Ogre::UO_UNMAP_ALL);
		vaoManager->destroyConstBuffer(mShadowPerGroupData);
		mShadowPerGroupData = 0;
	}

	if (mShadowWorkspace) {
		mCompositorManager->removeWorkspace(mShadowWorkspace);
		mShadowWorkspace = 0;
	}

	if (!mShadowMapTex.isNull()) {
		Ogre::ResourcePtr resPtr = mShadowMapTex;
		Ogre::TextureManager::getSingleton().remove(resPtr);
		mShadowMapTex.setNull();
	}
}
//-----------------------------------------------------------------------------------
inline size_t ShadowMapper::getStartsPtrCount(Ogre::int32* starts, Ogre::int32* startsBase) {
	const size_t offset = starts - startsBase;
	if ((offset & 0x11) == 0)
		return offset >> 2u;
	else
		return ((offset - 2u) >> 2u) + 4096u;
}
//-----------------------------------------------------------------------------------
inline Ogre::int32 ShadowMapper::getXStepsNeededToReachY(Ogre::uint32 y, float fStep) {
	return static_cast<Ogre::int32>(ceilf(Ogre::max(((y << 1u) - 1u) * fStep, 0.0f)));
}
//-----------------------------------------------------------------------------------
inline float ShadowMapper::getErrorAfterXsteps(Ogre::uint32 xIterationsToSkip, float dx, float dy) {
	//Round accumulatedError to next multiple of dx, then subtract accumulatedError.
	//That's the error at position (x; y). *MUST* be done in double precision, otherwise
	//we get artifacts with certain light angles.
	const double accumulatedError = dx * 0.5 + dy * (double) (xIterationsToSkip);
	const double newErrorAtX      = ceil(accumulatedError / dx) * dx - accumulatedError;
	return static_cast<float>(newErrorAtX);
}
//-----------------------------------------------------------------------------------
void ShadowMapper::updateShadowMap(const Ogre::Vector3& lightDir, const Ogre::Vector2& xzDimensions, float heightScale) {
	struct PerGroupData {
		Ogre::int32 iterations;
		float       deltaErrorStart;
		float       padding0;
		float       padding1;
	};

	Ogre::Vector2 lightDir2d(Ogre::Vector2(lightDir.x, lightDir.z).normalisedCopy());
	float         heightDelta = lightDir.y;

	if (lightDir2d.squaredLength() < 1e-6f) {
		//lightDir = Ogre::Vector3::UNIT_Y. Fix NaNs.
		lightDir2d.x = 1.0f;
		lightDir2d.y = 0.0f;
	}

	assert(mShadowStarts->getNumElements() >= (mHeightMapTex->getHeight() << 4u));

	Ogre::int32* startsBase = reinterpret_cast<Ogre::int32*>(
	        mShadowStarts->map(0, mShadowStarts->getNumElements()));
	PerGroupData* perGroupData = reinterpret_cast<PerGroupData*>(
	        mShadowPerGroupData->map(0, mShadowPerGroupData->getNumElements()));

	Ogre::uint32 width  = mHeightMapTex->getWidth();
	Ogre::uint32 height = mHeightMapTex->getHeight();

	//Bresenham's line algorithm.
	float x0 = 0;
	float y0 = 0;
	float x1 = static_cast<float>(width - 1u);
	float y1 = static_cast<float>(height - 1u);

	Ogre::uint32 heightOrWidth;
	Ogre::uint32 widthOrHeight;

	if (fabsf(lightDir2d.x) > fabsf(lightDir2d.y)) {
		y1 *= fabsf(lightDir2d.y) / fabsf(lightDir2d.x);
		heightOrWidth = height;
		widthOrHeight = width;

		heightDelta *= 1.0f / fabsf(lightDir.x);
	}
	else {
		x1 *= fabsf(lightDir2d.x) / fabsf(lightDir2d.y);
		heightOrWidth = width;
		widthOrHeight = height;

		heightDelta *= 1.0f / fabsf(lightDir.z);
	}

	if (lightDir2d.x < 0)
		std::swap(x0, x1);
	if (lightDir2d.y < 0)
		std::swap(y0, y1);

	const bool steep = fabsf(y1 - y0) > fabsf(x1 - x0);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	mJobParamIsStep->setManualValue((Ogre::int32) steep);

	float dx;
	float dy;
	{
		float _x0 = x0;
		float _y0 = y0;
		float _x1 = x1;
		float _y1 = y1;
		if (_x0 > _x1) {
			std::swap(_x0, _x1);
			std::swap(_y0, _y1);
		}
		dx = _x1 - _x0 + 1.0f;
		dy = fabs(_y1 - _y0);
		if (fabsf(lightDir2d.x) > fabsf(lightDir2d.y))
			dy += 1.0f * fabsf(lightDir2d.y) / fabsf(lightDir2d.x);
		else
			dy += 1.0f * fabsf(lightDir2d.x) / fabsf(lightDir2d.y);
		mJobParamDelta->setManualValue(Ogre::Vector2(dx, dy));
	}

	const Ogre::int32 xyStep[2] =
	        {
	                (x0 < x1) ? 1 : -1,
	                (y0 < y1) ? 1 : -1};
	mJobParamXYStep->setManualValue(xyStep, 2u);

	heightDelta = (-heightDelta * (xzDimensions.x / width)) / heightScale;
	//Avoid sending +/- inf (which causes NaNs inside the shader).
	//Values greater than 1.0 (or less than -1.0) are pointless anyway.
	heightDelta = Ogre::max(-1.0f, Ogre::min(1.0f, heightDelta));
	mJobParamHeightDelta->setManualValue(heightDelta);

	//y0 is not needed anymore, and we need it to be either 0 or heightOrWidth for the
	//algorithm to work correctly (depending on the sign of xyStep[1]). So do this now.
	if (y0 >= y1)
		y0 = heightOrWidth;

	Ogre::int32* starts = startsBase;

	const float fStep = (dx * 0.5f) / dy;
	//TODO numExtraIterations correct? -1? +1?
	Ogre::uint32 numExtraIterations = static_cast<Ogre::uint32>(
	        Ogre::min(ceilf(dy), ceilf(((heightOrWidth - 1u) / fStep - 1u) * 0.5f)));

	const Ogre::uint32 threadsPerGroup   = mShadowJob->getThreadsPerGroupX();
	const Ogre::uint32 firstThreadGroups = Ogre::alignToNextMultiple(heightOrWidth,
	                                                                 threadsPerGroup) /
	                                       threadsPerGroup;
	const Ogre::uint32 lastThreadGroups = Ogre::alignToNextMultiple(numExtraIterations,
	                                                                threadsPerGroup) /
	                                      threadsPerGroup;
	const Ogre::uint32 totalThreadGroups = firstThreadGroups + lastThreadGroups;

	const Ogre::int32 idy = static_cast<Ogre::int32>(floorf(dy));

	//"First" series of threadgroups
	for (Ogre::uint32 h = 0; h < firstThreadGroups; ++h) {
		const Ogre::uint32 startY = h * threadsPerGroup;

		for (Ogre::uint32 i = 0; i < threadsPerGroup; ++i) {
			*starts++ = static_cast<Ogre::int32>(x0);
			*starts++ = static_cast<Ogre::int32>(y0) + static_cast<Ogre::int32>(startY + i) * xyStep[1];
			*starts++ = 0; //Padding
			*starts++ = 0; //Padding

			if (starts - startsBase >= (4096u << 2u))
				starts -= (4096u << 2u) - 2u;
		}

		perGroupData->iterations      = widthOrHeight - std::max<Ogre::int32>(0, idy - (heightOrWidth - startY));
		perGroupData->deltaErrorStart = 0;
		perGroupData->padding0        = 0;
		perGroupData->padding1        = 0;
		++perGroupData;
	}

	//"Last" series of threadgroups
	for (Ogre::uint32 h = 0; h < lastThreadGroups; ++h) {
		const Ogre::int32 xN = getXStepsNeededToReachY(threadsPerGroup * h + 1u, fStep);

		for (Ogre::uint32 i = 0; i < threadsPerGroup; ++i) {
			*starts++ = static_cast<Ogre::int32>(x0) + xN * xyStep[0];
			*starts++ = static_cast<Ogre::int32>(y0) - static_cast<Ogre::int32>(i) * xyStep[1];
			*starts++ = 0; //Padding
			*starts++ = 0; //Padding

			if (starts - startsBase >= (4096u << 2u))
				starts -= (4096u << 2u) - 2u;
		}

		perGroupData->iterations      = widthOrHeight - xN;
		perGroupData->deltaErrorStart = getErrorAfterXsteps(xN, dx, dy) - dx * 0.5f;
		++perGroupData;
	}

	mShadowPerGroupData->unmap(Ogre::UO_KEEP_PERSISTENT);
	mShadowStarts->unmap(Ogre::UO_KEEP_PERSISTENT);

	//Re-Set them every frame (they may have changed if we have multiple Terrain instances)
	mShadowJob->setConstBuffer(0, mShadowStarts);
	mShadowJob->setConstBuffer(1, mShadowPerGroupData);
	mShadowJob->setTexture(0, mHeightMapTex);

	mShadowJob->setNumThreadGroups(totalThreadGroups, 1u, 1u);

	Ogre::ShaderParams& shaderParams = mShadowJob->getShaderParams("default");
	shaderParams.setDirty();

	mShadowWorkspace->_update();
}
//-----------------------------------------------------------------------------------
void ShadowMapper::fillUavDataForCompositorChannel(Ogre::CompositorChannel& outChannel,
                                                   Ogre::ResourceLayoutMap& outInitialLayouts,
                                                   Ogre::ResourceAccessMap& outInitialUavAccess) const {
	outChannel.target = mShadowMapTex->getBuffer(0)->getRenderTarget();
	outChannel.textures.push_back(mShadowMapTex);
	outInitialLayouts.insert(mShadowWorkspace->getResourcesLayout().begin(),
	                         mShadowWorkspace->getResourcesLayout().end());
	outInitialUavAccess.insert(mShadowWorkspace->getUavsAccess().begin(),
	                           mShadowWorkspace->getUavsAccess().end());
}
//-----------------------------------------------------------------------------------
void ShadowMapper::setGaussianFilterParams(Ogre::uint8 kernelRadius, float gaussianDeviationFactor) {
	Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();
	Ogre::HlmsCompute* hlmsCompute = hlmsManager->getComputeHlms();

	Ogre::HlmsComputeJob* job = 0;
	job                       = hlmsCompute->findComputeJob("Terra/GaussianBlurH");
	setGaussianFilterParams(job, kernelRadius, gaussianDeviationFactor);
	job = hlmsCompute->findComputeJob("Terra/GaussianBlurV");
	setGaussianFilterParams(job, kernelRadius, gaussianDeviationFactor);
}
//-----------------------------------------------------------------------------------
void ShadowMapper::setGaussianFilterParams(Ogre::HlmsComputeJob* job, Ogre::uint8 kernelRadius, float gaussianDeviationFactor) {
	assert(!(kernelRadius & 0x01) && "kernelRadius must be even!");

	if (job->getProperty("kernel_radius") != kernelRadius)
		job->setProperty("kernel_radius", kernelRadius);
	Ogre::ShaderParams& shaderParams = job->getShaderParams("default");

	std::vector<float> weights(kernelRadius + 1u);

	const float fKernelRadius     = kernelRadius;
	const float gaussianDeviation = fKernelRadius * gaussianDeviationFactor;

	//It's 2.0f if using the approximate filter (sampling between two pixels to
	//get the bilinear interpolated result and cut the number of samples in half)
	const float stepSize = 1.0f;

	//Calculate the weights
	float fWeightSum = 0;
	for (Ogre::uint32 i = 0; i < kernelRadius + 1u; ++i) {
		const float _X      = i - fKernelRadius + (1.0f - 1.0f / stepSize);
		float       fWeight = 1.0f / sqrt(2.0f * Ogre::Math::PI * gaussianDeviation * gaussianDeviation);
		fWeight *= exp(-(_X * _X) / (2.0f * gaussianDeviation * gaussianDeviation));

		fWeightSum += fWeight;
		weights[i] = fWeight;
	}

	fWeightSum = fWeightSum * 2.0f - weights[kernelRadius];

	//Normalize the weights
	for (Ogre::uint32 i = 0; i < kernelRadius + 1u; ++i)
		weights[i] /= fWeightSum;

	//Remove shader constants from previous calls (needed in case we've reduced the radius size)
	Ogre::ShaderParams::ParamVec::iterator itor = shaderParams.mParams.begin();
	Ogre::ShaderParams::ParamVec::iterator end  = shaderParams.mParams.end();

	while (itor != end) {
		Ogre::String::size_type pos = itor->name.find("c_weights[");

		if (pos != Ogre::String::npos) {
			itor = shaderParams.mParams.erase(itor);
			end  = shaderParams.mParams.end();
		}
		else {
			++itor;
		}
	}

	//Set the shader constants, 16 at a time (since that's the limit of what ManualParam can hold)
	char               tmp[32];
	Ogre::LwString     weightsString(Ogre::LwString::FromEmptyPointer(tmp, sizeof(tmp)));
	const Ogre::uint32 floatsPerParam = sizeof(Ogre::ShaderParams::ManualParam().dataBytes) / sizeof(float);
	for (Ogre::uint32 i = 0; i < kernelRadius + 1u; i += floatsPerParam) {
		weightsString.clear();
		weightsString.a("c_weights[", i, "]");

		Ogre::ShaderParams::Param p;
		p.isAutomatic = false;
		p.isDirty     = true;
		p.name        = weightsString.c_str();
		shaderParams.mParams.push_back(p);
		Ogre::ShaderParams::Param* param = &shaderParams.mParams.back();

		param->setManualValue(&weights[i], std::min<Ogre::uint32>(floatsPerParam, weights.size() - i));
	}

	shaderParams.setDirty();
}
