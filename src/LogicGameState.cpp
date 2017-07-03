#include "LogicGameState.h"
#include "GameEntityManager.h"
#include "LogicSystem.h"

#include "Converter.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

#include <OgreResourceGroupManager.h>
#include <OgreVector3.h>

#include <iostream>

LogicGameState::LogicGameState()
                : mDisplacement(0),
                  mCubeEntity(0),
                  mCubeMoDef(0),
                  mLogicSystem(0),
                  mTerrain(NULL) {
}
//------------------------------------------------------------------------------------------------
LogicGameState::~LogicGameState() {
	delete mCubeMoDef;
	mCubeMoDef = 0;
}
//------------------------------------------------------------------------------------------------
void LogicGameState::createScene(void) {
        mTerrain = new TerrainCollisions(mLogicSystem->getWorld());
        mTerrain->buildCollisions("terrain.png",
                Ogre::Vector3(64.0f, 4096.0f * 0.0f, 64.0f),
                Ogre::Vector3(1024.0f, 100.0f, 1024.0f),
                false);

        Common::GameEntityManager* geMgr = mLogicSystem->getGameEntityManager();
}
//------------------------------------------------------------------------------------------------
void LogicGameState::update(float timeSinceLast) {
	GameState::update(timeSinceLast);
}
