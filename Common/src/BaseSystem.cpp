#include "BaseSystem.h"
#include "State/State.h"

#include <SDL.h>

namespace Common {
	BaseSystem::BaseSystem() {}
	//------------------------------------------------------------------------------------------------
	BaseSystem::~BaseSystem() {}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::initialize(void) {}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::deinitialize(void) {}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::beginFrameParallel(void) { this->processIncomingMessages(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::update(float timeSinceLast) {}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::finishFrameParallel(void) { this->flushQueuedMessages(); }
	//------------------------------------------------------------------------------------------------
	void BaseSystem::finishFrame(void) {}
	//------------------------------------------------------------------------------------------------
	void BaseSystem::processIncomingMessage(Mq::MessageId messageId, const void* data) {
		const SDL_Event& evt = *(SDL_Event*) data;
		switch (messageId) {
		case Mq::SDL_EVENT:
			switch (evt.type) {
			case SDL_MOUSEMOTION:
				mouseMoved(evt);
				break;
			case SDL_MOUSEWHEEL:
				mouseMoved(evt);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mousePressed(evt.button, evt.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				mouseReleased(evt.button, evt.button.button);
				break;
			case SDL_KEYDOWN:
				if (!evt.key.repeat)
					keyPressed(evt.key);
				break;
			case SDL_KEYUP:
				if (!evt.key.repeat)
					keyReleased(evt.key);
				break;
			case SDL_TEXTINPUT:
				textInput(evt.text);
				break;
			case SDL_JOYAXISMOTION:
				joyAxisMoved(evt.jaxis, evt.jaxis.axis);
				break;
			case SDL_JOYBUTTONDOWN:
				joyButtonPressed(evt.jbutton,
                                                 evt.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				joyButtonReleased(evt.jbutton,
                                                  evt.jbutton.button);
				break;
			}
		}
	}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::mouseMoved(const SDL_Event& arg) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::mousePressed(const SDL_MouseButtonEvent& arg, Ogre::uint8 id) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::mouseReleased(const SDL_MouseButtonEvent& arg, Ogre::uint8 id) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::textInput(const SDL_TextInputEvent& arg) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::keyPressed(const SDL_KeyboardEvent& arg) {}
        //----------------------------------------------------------------------------------------------------
	void BaseSystem::keyReleased(const SDL_KeyboardEvent& arg) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::joyButtonPressed(const SDL_JoyButtonEvent& evt, int button) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::joyButtonReleased(const SDL_JoyButtonEvent& evt, int button) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::joyAxisMoved(const SDL_JoyAxisEvent& arg, int axis) {}
        //----------------------------------------------------------------------------------------------------
        void BaseSystem::joyPovMoved(const SDL_JoyHatEvent& arg, int index) {}
}
