#include "Terrain/Terrain.h"

#include "Converter.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

#include <OgreCamera.h>
#include <OgreFrustum.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreImage.h>
#include <OgrePixelFormat.h>
#include <OgreResourceGroupManager.h>
#include <OgreSceneManager.h>
#include <OgreTextureManager.h>

#include <iostream>

Terrain::Terrain()
                : mWidth(0u),
                  mDepth(0u),
                  mHeight(1.0f),
                  mTerrainOrigin(Ogre::Vector3::ZERO),
                  mXZDimensions(Ogre::Vector2::UNIT_SCALE),
                  mXZInvDimensions(Ogre::Vector2::UNIT_SCALE),
                  mDepthWidthRatio(1.0f),
                  mInvWidth(1.0f),
                  mInvDepth(1.0f) {}
//-------------------------------------------------------------------------------------------------
void Terrain::destroyHeightmapTexture(void) {
	if (!mHeightMapTex.isNull()) {
		Ogre::ResourcePtr resPtr = mHeightMapTex;
		Ogre::TextureManager::getSingleton().remove(resPtr);
		mHeightMapTex.setNull();
	}
}
//------------------------------------------------------------------------------------------------
void Terrain::createHeightmapTexture(const Ogre::String& imageName, const Ogre::Image& image) {
	destroyHeightmapTexture();

	if (image.getBPP() != 8 && image.getBPP() != 16 && image.getFormat() != Ogre::PF_FLOAT32_R) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
		            "Texture " + imageName + "must be 8 bpp, 16 bpp, or 32-bit Float",
		            "Terrain::createHeightmapTexture");
	}

        const Ogre::uint8 numMipmaps = image.getNumMipmaps();
//	const Ogre::uint8 numMipmaps = 0u;

	mHeightMapTex = Ogre::TextureManager::getSingleton().createManual(
	        "HeightMapTex" + Ogre::Id::generateNewId<Ogre::Texture>(),
	        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	        Ogre::TEX_TYPE_2D,
	        (uint) image.getWidth(),
	        (uint) image.getHeight(),
	        numMipmaps,
	        image.getFormat(),
	        Ogre::TU_STATIC_WRITE_ONLY);

	for (Ogre::uint8 mip = 0; mip <= numMipmaps; ++mip) {
		Ogre::v1::HardwarePixelBufferSharedPtr pixelBufferBuf = mHeightMapTex->getBuffer(0, mip);
		const Ogre::PixelBox&                  currImage      = pixelBufferBuf->lock(
		        Ogre::Box(0, 0, pixelBufferBuf->getWidth(), pixelBufferBuf->getHeight()),
		        Ogre::v1::HardwareBuffer::HBL_DISCARD);
		Ogre::PixelUtil::bulkPixelConversion(image.getPixelBox(0, mip), currImage);
		pixelBufferBuf->unlock();
	}
}
//-------------------------------------------------------------------------------------------------
void Terrain::createHeightmap(const Ogre::String& imageName, bool useHeightmapTexture) {
	Ogre::Image image;
	image.load(imageName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mWidth           = image.getWidth();
	mDepth           = image.getHeight();
	mDepthWidthRatio = mDepth / (float) (mWidth);
	mInvWidth        = 1.0f / mWidth;
	mInvDepth        = 1.0f / mDepth;

	if (Ogre::PixelUtil::getComponentCount(image.getFormat()) != 1) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
		            "Only grayscale images supported! " + imageName,
		            "Terrain::createHeightmap");
	}

        if (useHeightmapTexture)
	       createHeightmapTexture(imageName, image);
	mHeightMap.resize(mWidth * mDepth);

	const float maxValue    = powf(2.0f, (float) image.getBPP()) - 1.0f;
	const float invMaxValue = 1.0f / maxValue;

	if (image.getBPP() == 8) {
		const Ogre::uint8* RESTRICT_ALIAS data = reinterpret_cast<Ogre::uint8 * RESTRICT_ALIAS>(image.getData());
		for (Ogre::uint32 y = 0; y < mDepth; ++y)
			for (Ogre::uint32 x                = 0; x < mWidth; ++x)
				mHeightMap[y * mWidth + x] = (data[y * mWidth + x] * invMaxValue) * mHeight;
	}
	else if (image.getBPP() == 16) {
		const Ogre::uint16* RESTRICT_ALIAS data = reinterpret_cast<Ogre::uint16 * RESTRICT_ALIAS>(image.getData());
		for (Ogre::uint32 y = 0; y < mDepth; ++y)
			for (Ogre::uint32 x                = 0; x < mWidth; ++x)
				mHeightMap[y * mWidth + x] = (data[y * mWidth + x] * invMaxValue) * mHeight;
	}
	else if (image.getFormat() == Ogre::PF_FLOAT32_R) {
		const float* RESTRICT_ALIAS data = reinterpret_cast<float * RESTRICT_ALIAS>(image.getData());
		for (Ogre::uint32 y = 0; y < mDepth; ++y)
			for (Ogre::uint32 x                = 0; x < mWidth; ++x)
				mHeightMap[y * mWidth + x] = data[y * mWidth + x] * mHeight;
	}
	//	calculateOptimumSkirtSize();
}
//-------------------------------------------------------------------------------------------------
bool Terrain::getHeightAt(Ogre::Vector3& vPos) const {
	bool      retVal = false;
	GridPoint pos2D  = worldToGrid(vPos);

	if (pos2D.x < mWidth - 1 && pos2D.z < mDepth - 1) {
		const Ogre::Vector2 vPos2D = gridToWorld(pos2D);

		const float dx = (vPos.x - vPos2D.x) * mWidth * mXZInvDimensions.x;
		const float dz = (vPos.z - vPos2D.y) * mDepth * mXZInvDimensions.y;

		float       a, b, c;
		const float h00 = mHeightMap[pos2D.z * mWidth + pos2D.x];
		const float h11 = mHeightMap[(pos2D.z + 1) * mWidth + pos2D.x + 1];

		c = h00;
		if (dx < dz) {
			const float h01 = mHeightMap[(pos2D.z + 1) * mWidth + pos2D.x];
			b               = h01 - c;
			a               = h11 - b - c;
		}
		else {
			const float h10 = mHeightMap[pos2D.z * mWidth + pos2D.x + 1];

			a = h10 - c;
			b = h11 - a - c;
		}

		vPos.y = a * dx + b * dz + c + mTerrainOrigin.y;
		retVal = true;
	}

	return retVal;
}
//-------------------------------------------------------------------------------------------------
inline Ogre::Vector2 Terrain::gridToWorld(const GridPoint& gPos) const {
	Ogre::Vector2 retVal;

	const float fWidth = static_cast<float>(mWidth);
	const float fDepth = static_cast<float>(mDepth);

	retVal.x = (gPos.x / fWidth) * mXZDimensions.x + mTerrainOrigin.x;
	retVal.y = (gPos.z / fDepth) * mXZDimensions.y + mTerrainOrigin.z;

	return retVal;
}
//-------------------------------------------------------------------------------------------------
inline GridPoint Terrain::worldToGrid(const Ogre::Vector3& vPos) const {
	GridPoint   retVal;
	const float fWidth = static_cast<float>(mWidth);
	const float fDepth = static_cast<float>(mDepth);

	retVal.x = (Ogre::uint32) floorf(((vPos.x - mTerrainOrigin.x) * mXZInvDimensions.x) * fWidth);
	retVal.z = (Ogre::uint32) floorf(((vPos.z - mTerrainOrigin.z) * mXZInvDimensions.y) * fDepth);

	return retVal;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
TerrainGraphics::TerrainGraphics(Ogre::IdType               id,
                                 Ogre::ObjectMemoryManager* objectMemoryManager,
                                 Ogre::SceneManager*        sceneManager,
                                 Ogre::uint8                renderQueueId,
                                 Ogre::CompositorManager2*  compositorManager,
                                 Ogre::Camera*              camera)
                : Ogre::MovableObject(id, objectMemoryManager, sceneManager, renderQueueId),
                  mXZRelativeSize(Ogre::Vector2::UNIT_SCALE),
                  mBasePixelDimension(256u),
                  mCurrentCell(0u),
                  mCamera(camera) {
}
//-------------------------------------------------------------------------------------------------
TerrainGraphics::~TerrainGraphics() {
	mTerrainCells.clear();
}
//-------------------------------------------------------------------------------------------------
void TerrainGraphics::buildGraphics(const Ogre::String&  texName,
                                    const Ogre::Vector3  center,
                                    const Ogre::Vector3& dimensions,
                                    bool useHeightmapTexture) {
	mTerrainOrigin      = center - dimensions * 0.5f;
	mXZDimensions       = Ogre::Vector2(dimensions.x, dimensions.z);
	mXZInvDimensions    = 1.0f / mXZDimensions;
	mHeight             = dimensions.y;
	mBasePixelDimension = 64u;
	createHeightmap(texName, useHeightmapTexture);

	mXZRelativeSize = mXZDimensions / Ogre::Vector2(static_cast<Ogre::Real>(mWidth),
	                                                static_cast<Ogre::Real>(mDepth));

	{
		const Ogre::uint32 basePixelDimension = mBasePixelDimension;
		const Ogre::uint32 vertPixelDimension = static_cast<Ogre::uint32>(mBasePixelDimension * mDepthWidthRatio);
		const Ogre::uint32 maxPixelDimension  = std::max(basePixelDimension, vertPixelDimension);
		const Ogre::uint32 maxRes             = std::max(mWidth, mDepth);

		Ogre::uint32 numCells  = 16u; // 4x4
		Ogre::uint32 accumDim  = 0u;
		Ogre::uint32 iteration = 1u;
		while (accumDim < maxRes) {
			numCells += 12u; // 4x4 - 2x2
			accumDim += maxPixelDimension * (1u << iteration);
			++iteration;
		}

		numCells += 12u;
		accumDim += maxPixelDimension * (1u << iteration);
		++iteration;

		mTerrainCells.clear();
		mTerrainCells.resize(numCells, TerrainCell(this));
	}

	Ogre::VaoManager*                  vaoManager = mManager->getDestinationRenderSystem()->getVaoManager();
	std::vector<TerrainCell>::iterator itor       = mTerrainCells.begin();
	std::vector<TerrainCell>::iterator end        = mTerrainCells.end();

	const std::vector<TerrainCell>::iterator begin = itor;

	while (itor != end) {
		itor->initialize(vaoManager);
		++itor;
	}
}
//-------------------------------------------------------------------------------------------------
const Ogre::String& TerrainGraphics::getMovableType(void) const {
	static const Ogre::String movType = "Terrain";
	return movType;
}
//-------------------------------------------------------------------------------------------------
void TerrainGraphics::addRenderable(const GridPoint& gridPos,
                                    const GridPoint& cellSize,
                                    Ogre::uint32     lodLevel) {
	TerrainCell* cell = &mTerrainCells[mCurrentCell++];
	cell->setOrigin(gridPos, cellSize.x, cellSize.z, lodLevel);
	mCollectedCells[0].push_back(cell);
}
//-------------------------------------------------------------------------------------------------
void TerrainGraphics::update(const Ogre::Vector3& lightDir, float lightEpsilon) {
	mRenderables.clear();
	mCurrentCell = 0;

	Ogre::Vector3 camPos = mCamera->getDerivedPosition();

	const Ogre::uint32 basePixelDimension = mBasePixelDimension;
	const Ogre::uint32 vertPixelDimension = static_cast<Ogre::uint32>(mBasePixelDimension * mDepthWidthRatio);

	GridPoint cellSize;
	cellSize.x = basePixelDimension;
	cellSize.z = vertPixelDimension;

	// Quantize the camera position to basePixelDimension steps
	GridPoint camCenter = worldToGrid(camPos);
	camCenter.x         = (camCenter.x / basePixelDimension) * basePixelDimension;
	camCenter.z         = (camCenter.z / vertPixelDimension) * vertPixelDimension;

	Ogre::uint32 currentLod = 0;

	// LOD 0: Add full 4x4 grid
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

	mCurrentCell = 16u; // The first 16 cells don't use skirts.

	const Ogre::uint32 maxRes = std::max(mWidth, mDepth);

	size_t numObjectsAdded = std::numeric_limits<size_t>::max();
	while (numObjectsAdded != mCurrentCell || (mRenderables.empty() && (1u << currentLod) <= maxRes)) {
		numObjectsAdded = mCurrentCell;

		cellSize.x <<= 1u;
		cellSize.z <<= 1u;
		++currentLod;

		// Row 0
		const Ogre::int32 z = 1;
		for (Ogre::int32 x = -2; x < 2; ++x) {
			GridPoint pos = camCenter;
			pos.x += x * cellSize.x;
			pos.z += z * cellSize.z;

			if (isVisible(pos, cellSize))
				addRenderable(pos, cellSize, currentLod);
		}

		{
			// Row 3
			const Ogre::int32 z = -2;
			for (Ogre::int32 x = -2; x < 2; ++x) {
				GridPoint pos = camCenter;
				pos.x += x * cellSize.x;
				pos.z += z * cellSize.z;

				if (isVisible(pos, cellSize))
					addRenderable(pos, cellSize, currentLod);
			}
		}
		{
			// Cells [0, 1] & [0, 2];
			const Ogre::int32 x = -2;
			for (Ogre::int32 z = -1; z < 1; ++z) {
				GridPoint pos = camCenter;
				pos.x += x * cellSize.x;
				pos.z += z * cellSize.z;

				if (isVisible(pos, cellSize))
					addRenderable(pos, cellSize, currentLod);
			}
		}
		// Cells [3, 1] & [3, 2];
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
//-------------------------------------------------------------------------------------------------
void TerrainGraphics::optimizeCellsAndAdd(void) {
	// Keep iterating until mCollectedCells[0] stops shrinking
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
//-------------------------------------------------------------------------------------------------
bool TerrainGraphics::isVisible(const GridPoint& gPos, const GridPoint& gSize) const {
	if (gPos.x >= static_cast<Ogre::int32>(mWidth) || gPos.z >= static_cast<Ogre::int32>(mDepth) || gPos.x + gSize.x <= 0 || gPos.z + gSize.z <= 0)
		return false; // Outside terrain bounds

	const Ogre::Vector2 cellPos = gridToWorld(gPos);
	const Ogre::Vector2 cellSize((gSize.x + 1u) * mXZRelativeSize.x,
	                             (gSize.z + 1u) * mXZRelativeSize.y);

	const Ogre::Vector3 vHalfSize = Ogre::Vector3(cellSize.x, mHeight, cellSize.y) * 0.5f;
	const Ogre::Vector3 vCenter   = Ogre::Vector3(cellPos.x, mTerrainOrigin.y, cellPos.y) + vHalfSize;

	for (int i = 0; i < 6; ++i) {
		if (i == Ogre::FRUSTUM_PLANE_FAR && mCamera->getFarClipDistance() == 0)
			continue;

		Ogre::Plane::Side side = mCamera->getFrustumPlane(i).getSide(vCenter, vHalfSize);

		if (side == Ogre::Plane::NEGATIVE_SIDE)
			return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------------------
void TerrainGraphics::setDatablock(Ogre::HlmsDatablock* datablock) {
	std::vector<TerrainCell>::iterator itor = mTerrainCells.begin();
	std::vector<TerrainCell>::iterator end  = mTerrainCells.end();

	while (itor != end) {
		itor->setDatablock(datablock);
		++itor;
	}
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
TerrainCollisions::TerrainCollisions(btDynamicsWorld* world)
                : mWorld(world) {
}
//-------------------------------------------------------------------------------------------------
void TerrainCollisions::buildCollisions(const Ogre::String&  texName,
                                        const Ogre::Vector3  center,
                                        const Ogre::Vector3& dimensions,
                                        bool useHeightmapTexture) {
	mTerrainOrigin   = center - dimensions * 0.5f;
	mXZDimensions    = Ogre::Vector2(dimensions.x, dimensions.z);
	mXZInvDimensions = 1.0f / mXZDimensions;
	mHeight          = dimensions.y;

	createHeightmap(texName, useHeightmapTexture);
        createShape();
}
//-------------------------------------------------------------------------------------------------
void TerrainCollisions::createShape() {
	btScalar  heightScale = 1.f;
	btVector3 localScaling(1, heightScale, 1);

	btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(mWidth, mDepth, mHeightMap.data(), 1, 0, mHeight, 1, PHY_FLOAT, true);
	double scale = mXZDimensions.x / (mWidth - 1);
        terrainShape->setLocalScaling(btVector3(scale, 1, scale));

	btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), terrainShape);
	body->setFriction(0.8f);
	body->setHitFraction(0.8f);
	body->setRestitution(0.6f);
	body->getWorldTransform().setOrigin(btVector3(0, mHeight / 2, 0));
	body->getWorldTransform().setRotation(Collision::Converter::to(Ogre::Quaternion::IDENTITY));
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	mWorld->addRigidBody(body);

}
