#include "LogicGameState.h"
#include "LogicSystem.h"
#include "GameEntityManager.h"

#include <OgreVector3.h>
#include <OgreResourceGroupManager.h>

LogicGameState::LogicGameState() :
	mDisplacement( 0 ),
	mCubeEntity( 0 ),
	mCubeMoDef( 0 ),
	mLogicSystem( 0 ) {}
//-----------------------------------------------------------------------------------
LogicGameState::~LogicGameState() {
	delete mCubeMoDef;
	mCubeMoDef = 0;
}
//-----------------------------------------------------------------------------------
void LogicGameState::createScene(void) {
	const Ogre::Vector3 origin( -5.0f, 0.0f, 0.0f );

	Common::GameEntityManager *geMgr = mLogicSystem->getGameEntityManager();

	mCubeMoDef = new Common::MovableObjectDefinition();
	mCubeMoDef->meshName        = "athene.mesh";
	mCubeMoDef->resourceGroup   = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
	mCubeMoDef->moType          = Common::MoTypeItem;

	mCubeEntity = geMgr->addGameEntity(Ogre::SCENE_DYNAMIC, mCubeMoDef, origin,
										Ogre::Quaternion::IDENTITY,
										Ogre::Vector3::UNIT_SCALE);
}
//-----------------------------------------------------------------------------------
void LogicGameState::update(float timeSinceLast) {
	const Ogre::Vector3 origin(-5.0f, 0.0f, 0.0f);

	mDisplacement += timeSinceLast * 4.0f;
	mDisplacement = fmodf(mDisplacement, 10.0f);

	const size_t currIdx = mLogicSystem->getCurrentTransformIdx();
	mCubeEntity->mTransform[currIdx]->vPos = origin + Ogre::Vector3::UNIT_X * mDisplacement;
	mCubeEntity->mTransform[currIdx]->vScale = mDisplacement;

	GameState::update(timeSinceLast);
}
