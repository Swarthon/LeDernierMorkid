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

        mCubeMoDef = new Common::MovableObjectDefinition();
        mCubeMoDef->meshName        = "Cube.mesh";
        mCubeMoDef->resourceGroup   = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
        mCubeMoDef->moType          = Common::MoTypeItem;

        mCubeCoDef = new Common::CollisionObjectDefinition();
        mCubeCoDef->shape  = new btBoxShape(btVector3(1,1,1));
        mCubeCoDef->coType = Common::CoRigidBody;
        mCubeCoDef->mass   = 1;

        mCubeEntity = geMgr->addGameEntity(Ogre::SCENE_DYNAMIC,
                                           mCubeMoDef,
                                           mCubeCoDef,
                                           Ogre::Vector3(100,30,0),
                                           Ogre::Quaternion::IDENTITY,
                                           Ogre::Vector3::UNIT_SCALE * 0.5);
}
//------------------------------------------------------------------------------------------------
void LogicGameState::update(float timeSinceLast) {
	GameState::update(timeSinceLast);
}
