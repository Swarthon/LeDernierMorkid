#include "BaseSystem.h"
#include "GameState.h"

namespace Common {
	BaseSystem::BaseSystem(GameState* gameState)
	                : mCurrentGameState(gameState) {}
	//------------------------------------------------------------------------------------------------
	BaseSystem::~BaseSystem() {}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::initialize(void) { mCurrentGameState->initialize(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::deinitialize(void) { mCurrentGameState->deinitialize(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::createScene(void) { mCurrentGameState->createScene(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::destroyScene(void) { mCurrentGameState->destroyScene(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::beginFrameParallel(void) { this->processIncomingMessages(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::update(float timeSinceLast) { mCurrentGameState->update(timeSinceLast); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::finishFrameParallel(void) {
		mCurrentGameState->finishFrameParallel();

		this->flushQueuedMessages();
	}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::finishFrame(void) { mCurrentGameState->finishFrame(); }
}
