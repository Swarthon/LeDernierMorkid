#ifndef _TERRAINCELL_H_
#define _TERRAINCELL_H_

#include <OgrePrerequisites.h>
#include <OgreRenderable.h>

class GraphicsTerrain;
struct GridPoint;

class TerrainCell : public Ogre::Renderable {
	Ogre::int32  mGridX;
	Ogre::int32  mGridZ;
	Ogre::uint32 mLodLevel;
	Ogre::uint32 mVerticesPerLine;

	Ogre::uint32 mSizeX;
	Ogre::uint32 mSizeZ;

	Ogre::VaoManager* mVaoManager;

	GraphicsTerrain* mParentTerrain;

	bool mUseSkirts;

public:
	TerrainCell(GraphicsTerrain* parentTerrain);
	virtual ~TerrainCell();

	bool getUseSkirts(void) const { return mUseSkirts; }

	void initialize(Ogre::VaoManager* vaoManager, bool useSkirts);

	void setOrigin(const GridPoint& gridPos, Ogre::uint32 horizontalPixelDim, Ogre::uint32 verticalPixelDim, Ogre::uint32 lodLevel);

	/** Merges another TerrainCell into 'this' for reducing batch counts.
            e.g.
                Two 32x32 cells will merge into one 64x32 or 32x64
                Two 64x32 cells will merge into one 64x64
                A 32x64 cell cannot merge with a 32x32 one.
                A 64x32 cell cannot merge with a 32x32 one.
        @remarks
            Merge will only happen if the cells are of the same LOD level and are contiguous.
        @param next
            The other TerrainCell to merge with.
        @return
            False if couldn't merge, true on success.
        */
	bool merge(TerrainCell* next);

	void uploadToGpu(Ogre::uint32* RESTRICT_ALIAS gpuPtr) const;

	//Ogre::Renderable overloads
	virtual const Ogre::LightList& getLights(void) const;
	virtual void getRenderOperation(Ogre::v1::RenderOperation& op, bool casterPass);
	virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
	virtual bool getCastsShadows(void) const;
};

#endif
