#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <OgreMovableObject.h>
#include <OgrePrerequisites.h>
#include <OgreShaderParams.h>

#include "Terrain/TerrainCell.h"

struct GridPoint {
	Ogre::int32 x;
	Ogre::int32 z;
};

struct GridDirection {
	int x;
	int z;
};

class ShadowMapper;

class Terrain {
protected:
	friend class TerrainCell;

	std::vector<float> mHeightMap;
	Ogre::uint32       mWidth;
	Ogre::uint32       mDepth; //PNG's Height
	float              mDepthWidthRatio;
	float              mSkirtSize;
	float              mInvWidth;
	float              mInvDepth;

	Ogre::uint32 mBasePixelDimension;

	Ogre::Vector2 mXZDimensions;
	Ogre::Vector2 mXZInvDimensions;
	Ogre::Vector2 mXZRelativeSize; // mXZDimensions / [mWidth, mHeight]
	float         mHeight;
	Ogre::Vector3 mTerrainOrigin;

	Ogre::TexturePtr mHeightMapTex;

	virtual Ogre::Image loadImage(const Ogre::String& imageName);
	virtual void createHeightmap(Ogre::Image& image);

	void createHeightmapTexture(const Ogre::String& imageName, const Ogre::Image& image);
	void destroyHeightmapTexture(void);

	inline virtual GridPoint worldToGrid(const Ogre::Vector3& vPos) const;
	inline virtual Ogre::Vector2 gridToWorld(const GridPoint& gPos) const;

	virtual void calculateOptimumSkirtSize(void);

public:
	Terrain();
	~Terrain();

	virtual void load(Ogre::Image& image, const Ogre::Vector3 center, const Ogre::Vector3& dimensions);

	virtual bool getHeightAt(Ogre::Vector3& vPos) const;

	virtual const Ogre::Vector2& getXZDimensions(void) const { return mXZDimensions; }
	virtual const Ogre::Vector2& getXZInvDimensions(void) const { return mXZInvDimensions; }
	virtual float                getHeight(void) const { return mHeight; }
	virtual const Ogre::Vector3& getTerrainOrigin(void) const { return mTerrainOrigin; }
};

#endif
