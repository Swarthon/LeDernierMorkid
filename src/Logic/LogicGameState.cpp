#include "Logic/LogicGameState.h"

#include "GameEntityManager.h"
#include "LogicSystem.h"

#include "Converter.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

#include <OgreResourceGroupManager.h>
#include <OgreVector3.h>

LogicState::LogicState()
                : mTerrain(0),
                  mLogicSystem(0) {
}
//------------------------------------------------------------------------------------------------
LogicState::~LogicState() {
}
//------------------------------------------------------------------------------------------------
void LogicState::enter(void) {
	mTerrain = new CollisionTerrain(mLogicSystem->getWorld());
	mTerrain->load("terrain.png", Ogre::Vector3(0.0f, 4096.0f * 0.0f, 0.0f), Ogre::Vector3(1024.0f, 100.0f, 1024.0f));
}
//------------------------------------------------------------------------------------------------
void LogicState::update(double timeSinceLast) {
}
