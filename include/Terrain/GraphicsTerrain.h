#ifndef _GRAPHICSTERRAIN_H_
#define _GRAPHICSTERRAIN_H_

#include "Terrain.h"

class GraphicsTerrain : public Terrain, public Ogre::MovableObject {
protected:
	std::vector<TerrainCell>  mTerrainCells;
	std::vector<TerrainCell*> mCollectedCells[2];
	size_t                    mCurrentCell;

	Ogre::TexturePtr mNormalMapTex;

	Ogre::Vector3 mPrevLightDir;
	ShadowMapper* mShadowMapper;

	Ogre::CompositorManager2* mCompositorManager;
	Ogre::Camera*             mCamera;

	void createNormalTexture(void);
	void destroyNormalTexture(void);

	void addRenderable(const GridPoint& gridPos, const GridPoint& cellSize, Ogre::uint32 lodLevel);

	void optimizeCellsAndAdd(void);

	bool isVisible(const GridPoint& gPos, const GridPoint& gSize) const;

public:
	GraphicsTerrain(Ogre::IdType id, Ogre::ObjectMemoryManager* objectMemoryManager, Ogre::SceneManager* sceneManager, Ogre::uint8 renderQueueId, Ogre::CompositorManager2* compositorManager, Ogre::Camera* camera);
	~GraphicsTerrain();

	void load(const Ogre::String& texName, const Ogre::Vector3 center, const Ogre::Vector3& dimensions);
	void update(const Ogre::Vector3& lightDir, float lightEpsilon = 1e-6f);

	const ShadowMapper* getShadowMapper(void) const { return mShadowMapper; }

	const Ogre::String& getMovableType(void) const;

	Ogre::TexturePtr getHeightMapTex(void) const { return mHeightMapTex; }
	Ogre::TexturePtr getNormalMapTex(void) const { return mNormalMapTex; }
	Ogre::TexturePtr _getShadowMapTex(void) const;

	void setDatablock(Ogre::HlmsDatablock* datablock);
};

#endif
