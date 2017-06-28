#include "LogicGameState.h"
#include "GameEntityManager.h"
#include "LogicSystem.h"

#include <OgreResourceGroupManager.h>
#include <OgreVector3.h>

LogicGameState::LogicGameState()
                : mDisplacement(0), mCubeEntity(0), mCubeMoDef(0), mLogicSystem(0) {
}
//-----------------------------------------------------------------------------------
LogicGameState::~LogicGameState() {
	delete mCubeMoDef;
	mCubeMoDef = 0;
}
//-----------------------------------------------------------------------------------
void LogicGameState::createScene(void) {
}
//-----------------------------------------------------------------------------------
void LogicGameState::update(float timeSinceLast) {
	GameState::update(timeSinceLast);
}
