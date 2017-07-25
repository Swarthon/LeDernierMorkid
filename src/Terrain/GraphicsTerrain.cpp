#include "Terrain/GraphicsTerrain.h"

#include "Terrain/TerrainShadowMapper.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreImage.h>
#include <OgreTextureManager.h>

#include <Compositor/OgreCompositorChannel.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <OgreCamera.h>
#include <OgreMaterialManager.h>
#include <OgreRenderTexture.h>
#include <OgreSceneManager.h>
#include <OgreTechnique.h>

GraphicsTerrain::GraphicsTerrain(Ogre::IdType id, Ogre::ObjectMemoryManager* objectMemoryManager, Ogre::SceneManager* sceneManager, Ogre::uint8 renderQueueId, Ogre::CompositorManager2* compositorManager, Ogre::Camera* camera)
                : Ogre::MovableObject(id, objectMemoryManager, sceneManager, renderQueueId),
                  mCurrentCell(0u),
                  mPrevLightDir(Ogre::Vector3::ZERO),
                  mShadowMapper(0),
                  mCompositorManager(compositorManager),
                  mCamera(camera) {
}
//-----------------------------------------------------------------------------------
GraphicsTerrain::~GraphicsTerrain() {
	if (mShadowMapper) {
		mShadowMapper->destroyShadowMap();
		delete mShadowMapper;
		mShadowMapper = 0;
	}
	destroyNormalTexture();
	destroyHeightmapTexture();
	mTerrainCells.clear();
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::createNormalTexture(void) {
	destroyNormalTexture();

	mNormalMapTex = Ogre::TextureManager::getSingleton().createManual(
	        "NormalMapTex_" + Ogre::StringConverter::toString(getId()),
	        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	        Ogre::TEX_TYPE_2D,
	        mHeightMapTex->getWidth(),
	        mHeightMapTex->getHeight(),
	        Ogre::PixelUtil::getMaxMipmapCount(mHeightMapTex->getWidth(),
	                                           mHeightMapTex->getHeight()),
	        Ogre::PF_A2B10G10R10,
	        Ogre::TU_RENDERTARGET | Ogre::TU_AUTOMIPMAP);

	Ogre::MaterialPtr normalMapperMat = Ogre::MaterialManager::getSingleton().load(
	                                                                                 "Terra/GpuNormalMapper",
	                                                                                 Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)
	                                            .staticCast<Ogre::Material>();
	Ogre::Pass*             pass    = normalMapperMat->getTechnique(0)->getPass(0);
	Ogre::TextureUnitState* texUnit = pass->getTextureUnitState(0);
	texUnit->setTexture(mHeightMapTex);

	//Normalize vScale for better precision in the shader math
	const Ogre::Vector3 vScale = Ogre::Vector3(mXZRelativeSize.x, mHeight, mXZRelativeSize.y).normalisedCopy();

	Ogre::GpuProgramParametersSharedPtr psParams = pass->getFragmentProgramParameters();
	psParams->setNamedConstant("heightMapResolution", Ogre::Vector4(static_cast<Ogre::Real>(mWidth), static_cast<Ogre::Real>(mDepth), 1, 1));
	psParams->setNamedConstant("vScale", vScale);

	Ogre::CompositorChannelVec finalTargetChannels(1, Ogre::CompositorChannel());
	finalTargetChannels[0].target = mNormalMapTex->getBuffer()->getRenderTarget();
	finalTargetChannels[0].textures.push_back(mNormalMapTex);

	Ogre::Camera* dummyCamera = mManager->createCamera("TerrainDummyCamera");

	Ogre::CompositorWorkspace* workspace =
	        mCompositorManager->addWorkspace(mManager, finalTargetChannels, dummyCamera, "Terra/GpuNormalMapperWorkspace", false);
	workspace->_beginUpdate(true);
	workspace->_update();
	workspace->_endUpdate(true);

	mCompositorManager->removeWorkspace(workspace);
	mManager->destroyCamera(dummyCamera);
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::destroyNormalTexture(void) {
	if (!mNormalMapTex.isNull()) {
		Ogre::ResourcePtr resPtr = mNormalMapTex;
		Ogre::TextureManager::getSingleton().remove(resPtr);
		mNormalMapTex.setNull();
	}
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::addRenderable(const GridPoint& gridPos, const GridPoint& cellSize, Ogre::uint32 lodLevel) {
	TerrainCell* cell = &mTerrainCells[mCurrentCell++];
	cell->setOrigin(gridPos, cellSize.x, cellSize.z, lodLevel);
	mCollectedCells[0].push_back(cell);
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::optimizeCellsAndAdd(void) {
	//Keep iterating until mCollectedCells[0] stops shrinking
	size_t numCollectedCells = std::numeric_limits<size_t>::max();
	while (numCollectedCells != mCollectedCells[0].size()) {
		numCollectedCells = mCollectedCells[0].size();

		if (mCollectedCells[0].size() > 1) {
			mCollectedCells[1].clear();

			std::vector<TerrainCell*>::const_iterator itor = mCollectedCells[0].begin();
			std::vector<TerrainCell*>::const_iterator end  = mCollectedCells[0].end();

			while (end - itor >= 2u) {
				TerrainCell* currCell = *itor;
				TerrainCell* nextCell = *(itor + 1);

				mCollectedCells[1].push_back(currCell);
				if (currCell->merge(nextCell))
					itor += 2;
				else
					++itor;
			}

			while (itor != end)
				mCollectedCells[1].push_back(*itor++);

			mCollectedCells[1].swap(mCollectedCells[0]);
		}
	}

	std::vector<TerrainCell*>::const_iterator itor = mCollectedCells[0].begin();
	std::vector<TerrainCell*>::const_iterator end  = mCollectedCells[0].end();
	while (itor != end)
		mRenderables.push_back(*itor++);

	mCollectedCells[0].clear();
}
//-----------------------------------------------------------------------------------
bool GraphicsTerrain::isVisible(const GridPoint& gPos, const GridPoint& gSize) const {
	if (gPos.x >= static_cast<Ogre::int32>(mWidth) ||
	    gPos.z >= static_cast<Ogre::int32>(mDepth) ||
	    gPos.x + gSize.x <= 0 ||
	    gPos.z + gSize.z <= 0) {
		//Outside terrain bounds.
		return false;
	}

	//        return true;

	const Ogre::Vector2 cellPos = gridToWorld(gPos);
	const Ogre::Vector2 cellSize((gSize.x + 1u) * mXZRelativeSize.x,
	                             (gSize.z + 1u) * mXZRelativeSize.y);

	const Ogre::Vector3 vHalfSize = Ogre::Vector3(cellSize.x, mHeight, cellSize.y) * 0.5f;
	const Ogre::Vector3 vCenter   = Ogre::Vector3(cellPos.x, mTerrainOrigin.y, cellPos.y) + vHalfSize;

	for (int i = 0; i < 6; ++i) {
		//Skip far plane if view frustum is infinite
		if (i == Ogre::FRUSTUM_PLANE_FAR && mCamera->getFarClipDistance() == 0)
			continue;

		Ogre::Plane::Side side = mCamera->getFrustumPlane(i).getSide(vCenter, vHalfSize);

		//We only need one negative match to know the obj is outside the frustum
		if (side == Ogre::Plane::NEGATIVE_SIDE)
			return false;
	}

	return true;
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::load(const Ogre::String& texName, const Ogre::Vector3 center, const Ogre::Vector3& dimensions) {
        Ogre::Image image = loadImage(texName);
        createHeightmapTexture(texName, image);
	Terrain::load(image, center, dimensions);

	createNormalTexture();

	delete mShadowMapper;
	mShadowMapper = new ShadowMapper(mManager, mCompositorManager);
	mShadowMapper->createShadowMap(getId(), mHeightMapTex);

	const Ogre::uint32 basePixelDimension = mBasePixelDimension;
	const Ogre::uint32 vertPixelDimension = static_cast<Ogre::uint32>(mBasePixelDimension *
	                                                                  mDepthWidthRatio);
	const Ogre::uint32 maxPixelDimension = std::max(basePixelDimension, vertPixelDimension);
	const Ogre::uint32 maxRes            = std::max(mWidth, mDepth);

	Ogre::uint32 numCells  = 16u; //4x4
	Ogre::uint32 accumDim  = 0u;
	Ogre::uint32 iteration = 1u;
	while (accumDim < maxRes) {
		numCells += 12u; //4x4 - 2x2
		accumDim += maxPixelDimension * (1u << iteration);
		++iteration;
	}

	numCells += 12u;
	accumDim += maxPixelDimension * (1u << iteration);
	++iteration;

	mTerrainCells.clear();
	mTerrainCells.resize(numCells, TerrainCell(this));

	Ogre::VaoManager*                  vaoManager = mManager->getDestinationRenderSystem()->getVaoManager();
	std::vector<TerrainCell>::iterator itor       = mTerrainCells.begin();
	std::vector<TerrainCell>::iterator end        = mTerrainCells.end();

	const std::vector<TerrainCell>::iterator begin = itor;

	while (itor != end) {
		itor->initialize(vaoManager, (itor - begin) >= 16u);
		++itor;
	}
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::update(const Ogre::Vector3& lightDir, float lightEpsilon) {
	const float lightCosAngleChange = Ogre::Math::Clamp(
	        (float) mPrevLightDir.dotProduct(lightDir.normalisedCopy()), -1.0f, 1.0f);
	if (lightCosAngleChange <= (1.0f - lightEpsilon)) {
		mShadowMapper->updateShadowMap(lightDir, mXZDimensions, mHeight);
		mPrevLightDir = lightDir.normalisedCopy();
	}

	mRenderables.clear();
	mCurrentCell = 0;

	Ogre::Vector3 camPos = mCamera->getDerivedPosition();

	const Ogre::uint32 basePixelDimension = mBasePixelDimension;
	const Ogre::uint32 vertPixelDimension = static_cast<Ogre::uint32>(mBasePixelDimension * mDepthWidthRatio);

	GridPoint cellSize;
	cellSize.x = basePixelDimension;
	cellSize.z = vertPixelDimension;

	//Quantize the camera position to basePixelDimension steps
	GridPoint camCenter = worldToGrid(camPos);
	camCenter.x         = (camCenter.x / basePixelDimension) * basePixelDimension;
	camCenter.z         = (camCenter.z / vertPixelDimension) * vertPixelDimension;

	Ogre::uint32 currentLod = 0;

	//        camCenter.x = 64;
	//        camCenter.z = 64;

	//LOD 0: Add full 4x4 grid
	for (Ogre::int32 z = -2; z < 2; ++z) {
		for (Ogre::int32 x = -2; x < 2; ++x) {
			GridPoint pos = camCenter;
			pos.x += x * cellSize.x;
			pos.z += z * cellSize.z;

			if (isVisible(pos, cellSize))
				addRenderable(pos, cellSize, currentLod);
		}
	}

	optimizeCellsAndAdd();

	mCurrentCell = 16u; //The first 16 cells don't use skirts.

	const Ogre::uint32 maxRes = std::max(mWidth, mDepth);
	//TODO: When we're too far (outside the terrain), just display a 4x4 grid or something like that.

	size_t numObjectsAdded = std::numeric_limits<size_t>::max();
	//LOD n: Add 4x4 grid, ignore 2x2 center (which
	//is the same as saying the borders of the grid)
	while (numObjectsAdded != mCurrentCell ||
	       (mRenderables.empty() && (1u << currentLod) <= maxRes)) {
		numObjectsAdded = mCurrentCell;

		cellSize.x <<= 1u;
		cellSize.z <<= 1u;
		++currentLod;

		//Row 0
		{
			const Ogre::int32 z = 1;
			for (Ogre::int32 x = -2; x < 2; ++x) {
				GridPoint pos = camCenter;
				pos.x += x * cellSize.x;
				pos.z += z * cellSize.z;

				if (isVisible(pos, cellSize))
					addRenderable(pos, cellSize, currentLod);
			}
		}
		//Row 3
		{
			const Ogre::int32 z = -2;
			for (Ogre::int32 x = -2; x < 2; ++x) {
				GridPoint pos = camCenter;
				pos.x += x * cellSize.x;
				pos.z += z * cellSize.z;

				if (isVisible(pos, cellSize))
					addRenderable(pos, cellSize, currentLod);
			}
		}
		//Cells [0, 1] & [0, 2];
		{
			const Ogre::int32 x = -2;
			for (Ogre::int32 z = -1; z < 1; ++z) {
				GridPoint pos = camCenter;
				pos.x += x * cellSize.x;
				pos.z += z * cellSize.z;

				if (isVisible(pos, cellSize))
					addRenderable(pos, cellSize, currentLod);
			}
		}
		//Cells [3, 1] & [3, 2];
		{
			const Ogre::int32 x = 1;
			for (Ogre::int32 z = -1; z < 1; ++z) {
				GridPoint pos = camCenter;
				pos.x += x * cellSize.x;
				pos.z += z * cellSize.z;

				if (isVisible(pos, cellSize))
					addRenderable(pos, cellSize, currentLod);
			}
		}

		optimizeCellsAndAdd();
	}
}
//-----------------------------------------------------------------------------------
Ogre::TexturePtr GraphicsTerrain::_getShadowMapTex(void) const {
	return mShadowMapper->getShadowMapTex();
}
//-----------------------------------------------------------------------------------
const Ogre::String& GraphicsTerrain::getMovableType(void) const {
	static const Ogre::String movType = "Terrain";
	return movType;
}
//-----------------------------------------------------------------------------------
void GraphicsTerrain::setDatablock(Ogre::HlmsDatablock* datablock) {
	std::vector<TerrainCell>::iterator itor = mTerrainCells.begin();
	std::vector<TerrainCell>::iterator end  = mTerrainCells.end();

	while (itor != end) {
		itor->setDatablock(datablock);
		++itor;
	}
}
