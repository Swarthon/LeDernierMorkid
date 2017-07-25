#ifndef _TERRAINSHADOWMAPPER_H_
#define _TERRAINSHADOWMAPPER_H_

#include <OgreMovableObject.h>
#include <OgrePrerequisites.h>
#include <OgreShaderParams.h>

#include "Terrain/TerrainCell.h"

namespace Ogre {
	struct CompositorChannel;
}

class ShadowMapper {
	Ogre::TexturePtr mHeightMapTex;

	Ogre::ConstBufferPacked*   mShadowStarts;
	Ogre::ConstBufferPacked*   mShadowPerGroupData;
	Ogre::CompositorWorkspace* mShadowWorkspace;
	Ogre::TexturePtr           mShadowMapTex;
	Ogre::HlmsComputeJob*      mShadowJob;
	Ogre::ShaderParams::Param* mJobParamDelta;
	Ogre::ShaderParams::Param* mJobParamXYStep;
	Ogre::ShaderParams::Param* mJobParamIsStep;
	Ogre::ShaderParams::Param* mJobParamHeightDelta;

	//Ogre stuff
	Ogre::SceneManager*       mSceneManager;
	Ogre::CompositorManager2* mCompositorManager;

	static inline size_t getStartsPtrCount(Ogre::int32* starts, Ogre::int32* startsBase);

	/** Gets how many steps are needed in Bresenham's algorithm to reach certain height,
            given its dx / dy ratio where:
                dx = abs( x1 - x0 );
                dy = abs( y1 - y0 );
            and Bresenham is drawn in ranges [x0; x1) and [y0; y1)
        @param y
            Height to reach
        @param fStep
            (dx * 0.5f) / dy;
        @return
            Number of X iterations needed to reach the the pixel at height 'y'
            The returned value is at position (retVal; y)
            which means (retVal-1; y-1) is true unless y = 0;
        */
	static inline Ogre::int32 getXStepsNeededToReachY(Ogre::uint32 y, float fStep);

	/** Calculates the value of the error at position x = xIterationsToSkip from
            Bresenham's algorithm.
        @remarks
            We use this function so we can start Bresenham from '0' but resuming as
            if we wouldn't be starting from 0.
        @param xIterationsToSkip
            The X position in which we want the error.
        @param dx
            delta.x
        @param dy
            delta.y
        @return
            The error at position (xIterationsToSkip; y)
        */
	static inline float getErrorAfterXsteps(Ogre::uint32 xIterationsToSkip, float dx, float dy);

	static void setGaussianFilterParams(Ogre::HlmsComputeJob* job, Ogre::uint8 kernelRadius, float gaussianDeviationFactor = 0.5f);

public:
	ShadowMapper(Ogre::SceneManager* sceneManager, Ogre::CompositorManager2* compositorManager);
	~ShadowMapper();

	/** Sets the parameter of the gaussian filter we apply to the shadow map.
        @param kernelRadius
            Kernel radius. Must be an even number.
        @param gaussianDeviationFactor
            Expressed in terms of gaussianDeviation = kernelRadius * gaussianDeviationFactor
        */
	void setGaussianFilterParams(Ogre::uint8 kernelRadius, float gaussianDeviationFactor = 0.5f);

	void createShadowMap(Ogre::IdType id, Ogre::TexturePtr& heightMapTex);
	void destroyShadowMap(void);
	void updateShadowMap(const Ogre::Vector3& lightDir, const Ogre::Vector2& xzDimensions, float heightScale);

	void fillUavDataForCompositorChannel(Ogre::CompositorChannel& outChannel,
	                                     Ogre::ResourceLayoutMap& outInitialLayouts,
	                                     Ogre::ResourceAccessMap& outInitialUavAccess) const;

	Ogre::TexturePtr getShadowMapTex(void) const { return mShadowMapTex; }
};

#endif
