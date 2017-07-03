#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <OgreMovableObject.h>
#include <OgrePrerequisites.h>

class btDynamicsWorld;

#include "TerrainCell.h"

struct GridPoint {
	Ogre::int32 x;
	Ogre::int32 z;
};
struct GridDirection {
	int x;
	int z;
};

class Terrain {
public:
	Terrain();
protected:
	Ogre::uint32 mWidth;
	Ogre::uint32 mDepth;
	Ogre::Real   mHeight;

	Ogre::Vector2 mXZDimensions;
	Ogre::Vector2 mXZInvDimensions;
	Ogre::Vector3 mTerrainOrigin;

	std::vector<float> mHeightMap;
	Ogre::TexturePtr mHeightMapTex;

	Ogre::Real   mInvWidth;
	Ogre::Real   mInvDepth;
	Ogre::Real   mDepthWidthRatio;

	bool getHeightAt(Ogre::Vector3& vPos) const;

	void createHeightmap(const Ogre::String& imageName, bool useHeightmapTexture = true);
	void createHeightmapTexture(const Ogre::String& imageName, const Ogre::Image& image);
	void destroyHeightmapTexture();

	inline Ogre::Vector2 gridToWorld(const GridPoint& gPos) const;
	inline GridPoint worldToGrid(const Ogre::Vector3& vPos) const;
};

class TerrainGraphics : public Terrain, public Ogre::MovableObject {
public:
	TerrainGraphics(Ogre::IdType		   id,
	       		Ogre::ObjectMemoryManager* objectMemoryManager,
	        	Ogre::SceneManager*        sceneManager,
	        	Ogre::uint8                renderQueueId,
	        	Ogre::CompositorManager2*  compositorManager,
	        	Ogre::Camera*              camera);
	~TerrainGraphics();
	void buildGraphics(const Ogre::String&  texName,
	                   const Ogre::Vector3  center,
	                   const Ogre::Vector3& dimensions,
			   bool useHeightmapTexture = true);
	void update(const Ogre::Vector3& lightDir, float lightEpsilon = 1e-6f);

	void
	addRenderable(const GridPoint& gridPos, const GridPoint& cellSize, Ogre::uint32 lodLevel);

	bool isVisible(const GridPoint& gPos, const GridPoint& gSize) const;
	void setDatablock(Ogre::HlmsDatablock* datablock);

	const Ogre::String& getMovableType() const;

	Ogre::TexturePtr getHeightMapTex(void) const { return mHeightMapTex; }
	Ogre::TexturePtr getNormalMapTex(void) const { return mNormalMapTex; }

protected:
	friend class TerrainCell;
	Ogre::Vector2 mXZRelativeSize;

	Ogre::uint32 mBasePixelDimension;
	size_t       mCurrentCell;
	float        mSkirtSize;

	std::vector<TerrainCell>  mTerrainCells;
	std::vector<TerrainCell*> mCollectedCells[2];

	Ogre::Camera* mCamera;

	Ogre::TexturePtr mNormalMapTex;

	void                 optimizeCellsAndAdd(void);
};

class TerrainCollisions : public Terrain {
public:
	TerrainCollisions(btDynamicsWorld* world);
	void buildCollisions(const Ogre::String&  texName,
	                     const Ogre::Vector3  center,
	                     const Ogre::Vector3& dimensions,
		     	     bool useHeightmapTexture = true);
	void createShape();
private:
	btDynamicsWorld* mWorld;
};

#endif // _TERRAIN_H_
