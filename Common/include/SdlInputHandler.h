// Thanks to Jordan Milne and Scrawl for allowing to use their
// sdlinputwrapper files as base under the MIT license

/*
This file has been modified to fit with Swarthon's syntax rules
*/

#ifndef _SDLINPUTHANDLER_H_
#define _SDLINPUTHANDLER_H_

#include "BaseSystem.h"
#include "SdlEmulationLayer.h"
#include <OgrePrerequisites.h>

#include <SDL.h>

namespace Common {
	class MouseListener;
	class KeyboardListener;
	class JoystickListener;

	class SdlInputHandler {
		SDL_Window* mSdlWindow;

		BaseSystem*       mGraphicsSystem;
		BaseSystem*       mLogicSystem;
		MouseListener*    mMouseListener;
		KeyboardListener* mKeyboardListener;
		JoystickListener* mJoystickListener;

		bool mWantRelative;
		bool mWantMouseGrab;
		bool mWantMouseVisible;

		bool mIsMouseRelative;
		bool mWrapPointerManually;
		bool mGrabPointer;
		bool mMouseInWindow;
		bool mWindowHasFocus;

		Uint16 mWarpX;
		Uint16 mWarpY;
		bool   mWarpCompensate;

		void updateMouseSettings(void);

		void handleWindowEvent(const SDL_Event& evt);

		void warpMouse(int x, int y);

		void wrapMousePointer(const SDL_MouseMotionEvent& evt);

		bool handleWarpMotion(const SDL_MouseMotionEvent& evt);

	public:
		SdlInputHandler(SDL_Window*       sdlWindow,
		                MouseListener*    mouseListener,
		                KeyboardListener* keyboardListener,
		                JoystickListener* joystickListener,
				BaseSystem*	  graphicsSystem,
				BaseSystem*	  logicSystem);
		virtual ~SdlInputHandler();

		void _handleSdlEvents(const SDL_Event& evt);

		void setGrabMousePointer(bool grab);

		void setMouseRelative(bool relative);

		void setMouseVisible(bool visible);
	};
}

#endif // _SDLINPUTHANDLER_H_
