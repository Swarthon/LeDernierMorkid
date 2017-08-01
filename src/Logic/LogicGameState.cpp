#include "Logic/LogicGameState.h"

#include "GameEntityManager.h"
#include "LogicSystem.h"

#include "Converter.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

#include <OgreResourceGroupManager.h>
#include <OgreVector3.h>

LogicGameState::LogicGameState()
                : mTerrain(0),
                  mLogicSystem(0) {
}
//------------------------------------------------------------------------------------------------
LogicGameState::~LogicGameState() {
}
//------------------------------------------------------------------------------------------------
void LogicGameState::createScene(void) {
	mTerrain = new CollisionTerrain(mLogicSystem->getWorld());
	mTerrain->load("terrain.png", Ogre::Vector3(0.0f, 4096.0f * 0.0f, 0.0f), Ogre::Vector3(1024.0f, 100.0f, 1024.0f));
}
//------------------------------------------------------------------------------------------------
void LogicGameState::update(float timeSinceLast) {
	GameState::update(timeSinceLast);
}
