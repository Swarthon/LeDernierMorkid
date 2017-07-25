#include "Terrain/TerrainCell.h"
#include "Terrain/GraphicsTerrain.h"

#include <Vao/OgreVaoManager.h>
#include <Vao/OgreVertexArrayObject.h>

TerrainCell::TerrainCell(GraphicsTerrain* parentTerrain)
                : mGridX(0),
                  mGridZ(0),
                  mLodLevel(0),
                  mVerticesPerLine(1),
                  mSizeX(0),
                  mSizeZ(0),
                  mVaoManager(0),
                  mParentTerrain(parentTerrain),
                  mUseSkirts(false) {
}
//-----------------------------------------------------------------------------------
TerrainCell::~TerrainCell() {
	Ogre::VertexArrayObjectArray::const_iterator itor = mVaoPerLod[Ogre::VpNormal].begin();
	Ogre::VertexArrayObjectArray::const_iterator end  = mVaoPerLod[Ogre::VpNormal].end();

	while (itor != end)
		mVaoManager->destroyVertexArrayObject(*itor++);

	mVaoPerLod[Ogre::VpNormal].clear();
	mVaoPerLod[Ogre::VpShadow].clear();
}
//-----------------------------------------------------------------------------------
void TerrainCell::initialize(Ogre::VaoManager* vaoManager, bool useSkirts) {
	assert(mVaoPerLod[Ogre::VpNormal].empty() && "Already initialized!");
	mVaoManager = vaoManager;
	mUseSkirts  = useSkirts;

	//Setup bufferless vao
	Ogre::VertexBufferPackedVec vertexBuffers;
	Ogre::VertexArrayObject*    vao = vaoManager->createVertexArrayObject(
	        vertexBuffers, 0, Ogre::OT_TRIANGLE_STRIP);
	mVaoPerLod[Ogre::VpNormal].push_back(vao);
	mVaoPerLod[Ogre::VpShadow].push_back(vao);
}
//-----------------------------------------------------------------------------------
void TerrainCell::setOrigin(const GridPoint& gridPos, Ogre::uint32 horizontalPixelDim, Ogre::uint32 verticalPixelDim, Ogre::uint32 lodLevel) {
	mGridX    = gridPos.x;
	mGridZ    = gridPos.z;
	mLodLevel = lodLevel;

	horizontalPixelDim = std::min(horizontalPixelDim, mParentTerrain->mWidth - mGridX);
	verticalPixelDim   = std::min(verticalPixelDim, mParentTerrain->mDepth - mGridZ);

	mSizeX = horizontalPixelDim;
	mSizeZ = verticalPixelDim;

	//Divide by 2^lodLevel and round up
	horizontalPixelDim = (horizontalPixelDim + (1u << lodLevel) - 1u) >> lodLevel;
	verticalPixelDim   = (verticalPixelDim + (1u << lodLevel) - 1u) >> lodLevel;

	//Add an extra vertex to fill the gaps to the next TerrainCell.
	horizontalPixelDim += 1u;
	verticalPixelDim += 1u;

	horizontalPixelDim = std::max(horizontalPixelDim, 2u);
	verticalPixelDim   = std::max(verticalPixelDim, 2u);

	if (mUseSkirts) {
		//Add two extra vertices & two extra rows for the skirts.
		horizontalPixelDim += 2u;
		verticalPixelDim += 2u;
	}

	mVerticesPerLine = horizontalPixelDim * 2u + 2u;

	assert(mVerticesPerLine * (verticalPixelDim - 1u) > 0u);

	Ogre::VertexArrayObject* vao = mVaoPerLod[Ogre::VpNormal][0];
	vao->setPrimitiveRange(0, mVerticesPerLine * (verticalPixelDim - 1u));
}
//-----------------------------------------------------------------------
bool TerrainCell::merge(TerrainCell* next) {
	bool merged = false;

	if (mLodLevel == next->mLodLevel) {
		GridPoint pos;
		pos.x                           = mGridX;
		pos.z                           = mGridZ;
		Ogre::uint32 horizontalPixelDim = mSizeX;
		Ogre::uint32 verticalPixelDim   = mSizeZ;

		if ((this->mGridX + this->mSizeX == next->mGridX ||
		     next->mGridX + next->mSizeX == this->mGridX) &&
		    mGridZ == next->mGridZ && mSizeZ == next->mSizeZ) {
			//Merge horizontally
			pos.x = std::min(mGridX, next->mGridX);
			horizontalPixelDim += next->mSizeX;

			this->setOrigin(pos, horizontalPixelDim, verticalPixelDim, mLodLevel);
			merged = true;
		}
		else if ((this->mGridZ + this->mSizeZ == next->mGridZ ||
		          next->mGridZ + next->mSizeZ == this->mGridZ) &&
		         mGridX == next->mGridX && mSizeX == next->mSizeX) {
			//Merge vertically
			pos.z = std::min(mGridZ, next->mGridZ);
			verticalPixelDim += next->mSizeZ;

			this->setOrigin(pos, horizontalPixelDim, verticalPixelDim, mLodLevel);
			merged = true;
		}
	}

	return merged;
}
//-----------------------------------------------------------------------
void TerrainCell::uploadToGpu(Ogre::uint32* RESTRICT_ALIAS gpuPtr) const {
	//Ogre::uint32 rows = (mSizeZ + (1u << mLodLevel) - 1u) >> mLodLevel;
	Ogre::VertexArrayObject* vao = mVaoPerLod[Ogre::VpNormal][0];

	//uvec4 numVertsPerLine
	gpuPtr[0] = mVerticesPerLine;
	gpuPtr[1] = mLodLevel;
	gpuPtr[2] = vao->getPrimitiveCount() / mVerticesPerLine - 2u;
	gpuPtr[3] = *reinterpret_cast<Ogre::uint32*>(&mParentTerrain->mSkirtSize);

	//ivec4 xzTexPosBounds
	((Ogre::int32 * RESTRICT_ALIAS) gpuPtr)[4] = mGridX;
	((Ogre::int32 * RESTRICT_ALIAS) gpuPtr)[5] = mGridZ;
	((Ogre::int32 * RESTRICT_ALIAS) gpuPtr)[6] = mParentTerrain->mWidth - 1u;
	((Ogre::int32 * RESTRICT_ALIAS) gpuPtr)[7] = mParentTerrain->mDepth - 1u;

	((float* RESTRICT_ALIAS) gpuPtr)[8]  = mParentTerrain->mTerrainOrigin.x;
	((float* RESTRICT_ALIAS) gpuPtr)[9]  = mParentTerrain->mTerrainOrigin.y;
	((float* RESTRICT_ALIAS) gpuPtr)[10] = mParentTerrain->mTerrainOrigin.z;
	((float* RESTRICT_ALIAS) gpuPtr)[11] = mParentTerrain->mInvWidth;

	((float* RESTRICT_ALIAS) gpuPtr)[12] = mParentTerrain->mXZRelativeSize.x;
	((float* RESTRICT_ALIAS) gpuPtr)[13] = mParentTerrain->mHeight;
	((float* RESTRICT_ALIAS) gpuPtr)[14] = mParentTerrain->mXZRelativeSize.y;
	((float* RESTRICT_ALIAS) gpuPtr)[15] = mParentTerrain->mInvDepth;
}
//-----------------------------------------------------------------------
const Ogre::LightList& TerrainCell::getLights(void) const {
	return mParentTerrain->queryLights();
}
//-----------------------------------------------------------------------------
void TerrainCell::getRenderOperation(Ogre::v1::RenderOperation& op, bool casterPass) {
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
	            "Items do not implement getRenderOperation. You've put an Item in "
	            "the wrong RenderQueue ID (which is set to be compatible with "
	            "Ogre::v1::Entity). Do not mix Items and Entities",
	            "TerrainCell::getRenderOperation");
}
//-----------------------------------------------------------------------------
void TerrainCell::getWorldTransforms(Ogre::Matrix4* xform) const {
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
	            "Items do not implement getWorldTransforms. You've put an Item in "
	            "the wrong RenderQueue ID (which is set to be compatible with "
	            "Ogre::v1::Entity). Do not mix Items and Entities",
	            "TerrainCell::getWorldTransforms");
}
//-----------------------------------------------------------------------------
bool TerrainCell::getCastsShadows(void) const {
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
	            "Items do not implement getCastsShadows. You've put an Item in "
	            "the wrong RenderQueue ID (which is set to be compatible with "
	            "Ogre::v1::Entity). Do not mix Items and Entities",
	            "TerrainCell::getCastsShadows");
}
