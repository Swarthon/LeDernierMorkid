#include "BaseSystem.h"
#include "GameState.h"

#include <SDL.h>

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
	//------------------------------------------------------------------------------------------------
	void BaseSystem::processIncomingMessage(Mq::MessageId messageId, const void* data) {
		const SDL_Event& evt = *(SDL_Event*)data;
		switch (messageId) {
		case Mq::SDL_EVENT:
			switch (evt.type) {
			case SDL_MOUSEMOTION:
				mCurrentGameState->mouseMoved(evt);
				break;
			case SDL_MOUSEWHEEL:
				mCurrentGameState->mouseMoved(evt);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mCurrentGameState->mousePressed(evt.button, evt.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				mCurrentGameState->mouseReleased(evt.button, evt.button.button);
				break;
			case SDL_KEYDOWN:
				if (!evt.key.repeat)
					mCurrentGameState->keyPressed(evt.key);
				break;
			case SDL_KEYUP:
				if (!evt.key.repeat)
					mCurrentGameState->keyReleased(evt.key);
				break;
			case SDL_TEXTINPUT:
				mCurrentGameState->textInput(evt.text);
				break;
			case SDL_JOYAXISMOTION:
				mCurrentGameState->joyAxisMoved(evt.jaxis, evt.jaxis.axis);
				break;
			case SDL_JOYBUTTONDOWN:
				mCurrentGameState->joyButtonPressed(evt.jbutton,
				                                    evt.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				mCurrentGameState->joyButtonReleased(evt.jbutton,
				                                     evt.jbutton.button);
				break;
			}
		}
	}
}
