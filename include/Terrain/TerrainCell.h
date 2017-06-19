#ifndef _TERRAINCELL_H_
#define _TERRAINCELL_H_

#include <OgrePrerequisites.h>
#include <OgreRenderable.h>

class Terrain;
struct GridPoint;

class TerrainCell : public Ogre::Renderable {
private:
	Terrain* mParentTerrain;
	Ogre::VaoManager *mVaoManager;

	Ogre::int32 	mGridX;
        Ogre::int32	mGridZ;
        Ogre::uint32	mLodLevel;
        Ogre::uint32	mVerticesPerLine;

        Ogre::uint32	mSizeX;
        Ogre::uint32	mSizeZ;
public:
	TerrainCell(Terrain*);
	void initialize( Ogre::VaoManager *vaoManager);
	void setOrigin(const GridPoint &gridPos, Ogre::uint32 horizontalPixelDim, Ogre::uint32 verticalPixelDim, Ogre::uint32 lodLevel);
	bool merge(TerrainCell *next);

	virtual const Ogre::LightList& getLights(void) const {}
	virtual void getWorldTransforms(Ogre::Matrix4* xform) const {}
	virtual void getRenderOperation(Ogre::v1::RenderOperation& op, bool casterPass) {}
	virtual void uploadToGpu( Ogre::uint32 * RESTRICT_ALIAS gpuPtr ) const;
};

#endif // _TERRAINCELL_H_
