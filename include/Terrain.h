#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <OgrePrerequisites.h>
#include <OgreMovableObject.h>

#include "TerrainCell.h"

struct GridPoint {
	Ogre::int32 x;
        Ogre::int32 z;
};
struct GridDirection {
        int x;
        int z;
};

class Terrain : public Ogre::MovableObject {
public:
	Terrain( Ogre::IdType id, Ogre::ObjectMemoryManager *objectMemoryManager,
		      Ogre::SceneManager *sceneManager, Ogre::uint8 renderQueueId,
		      Ogre::CompositorManager2 *compositorManager, Ogre::Camera *camera );
	~Terrain();
	void build(const Ogre::String &texName, const Ogre::Vector3 center, const Ogre::Vector3 &dimensions);
	void update(const Ogre::Vector3 &lightDir, float lightEpsilon=1e-6f);

	void addRenderable( const GridPoint &gridPos, const GridPoint &cellSize, Ogre::uint32 lodLevel);

	bool isVisible( const GridPoint &gPos, const GridPoint &gSize ) const;
	void setDatablock(Ogre::HlmsDatablock *datablock);

	const Ogre::String& getMovableType() const;
	bool getHeightAt(Ogre::Vector3 &vPos) const;

	Ogre::TexturePtr getHeightMapTex(void) const    { return mHeightMapTex; }
	Ogre::TexturePtr getNormalMapTex(void) const    { return mNormalMapTex; }

protected:
    friend class TerrainCell;
	Ogre::Vector3	mTerrainOrigin;
	Ogre::Vector2	mXZDimensions;
	Ogre::Vector2	mXZInvDimensions;
	Ogre::Vector2	mXZRelativeSize;
	Ogre::Real	mHeight;

	Ogre::uint32	mWidth;
	Ogre::uint32	mDepth;
	Ogre::Real	mInvWidth;
	Ogre::Real	mInvDepth;
	Ogre::Real	mDepthWidthRatio;
	Ogre::uint32 	mBasePixelDimension;
	size_t		mCurrentCell;
	float		mSkirtSize;

	std::vector<float> mHeightMap;
	std::vector<TerrainCell>   mTerrainCells;
	std::vector<TerrainCell*>  mCollectedCells[2];

	Ogre::Camera*	mCamera;

	Ogre::TexturePtr    mHeightMapTex;
	Ogre::TexturePtr    mNormalMapTex;

	void optimizeCellsAndAdd(void);
	inline Ogre::Vector2 gridToWorld(const GridPoint &gPos) const;
	inline GridPoint worldToGrid(const Ogre::Vector3 &vPos) const;

	void createHeightmap(const Ogre::String& imageName);
	void createHeightmapTexture(const Ogre::String &imageName, const Ogre::Image &image);
	void destroyHeightmapTexture();
};

#endif // _TERRAIN_H_