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
	class SdlInputHandler {
		/// Window of the program
		SDL_Window* mSdlWindow;

		/// Emitter of the events
		BaseSystem*              mEmitter;
		/// Receivers of the events
		std::vector<BaseSystem*> mReceivers;

		/// User setting, whether the user wants the mouse to be relative or not
		bool mWantRelative;
		/// User setting, whether the user wants the mouse to be locked or not
		bool mWantMouseGrab;
		/// User setting, whether the user wants the mouse to be visible or not
		bool mWantMouseVisible;

		/// Whether the mouse is relative
		bool mIsMouseRelative;
		/// Used when driver doesn't support relative mode
		bool mWrapPointerManually;
		/// Whether the pointer is lock
		bool mGrabPointer;
		/// Whether the mouse is in the window
		bool mMouseInWindow;
		/// Whether the window has the focus
		bool mWindowHasFocus;

		/// X coordinate of the mouse
		Uint16 mWarpX;
		/// Y coordinate of the mouse
		Uint16 mWarpY;
		/// Whether the mouse has to be wrapped
		bool   mWarpCompensate;

		/// Update mouse settings
		void updateMouseSettings(void);

		/// Manage window event
		void handleWindowEvent(const SDL_Event& evt);

		/**
		 * Moves the mouse to the specified point within the viewport
		 * @param x
		 * 	X coordinate
		 * @param y
		 *	Y coordinate
		 */
		void warpMouse(int x, int y);

		/**
		 * Prevents the mouse cursor from leaving the window
		 * @param evt
		 *	Event containing mouse data
		 */
		void wrapMousePointer(const SDL_MouseMotionEvent& evt);

		/**
		 * Internal method for ignoring relative
        	 * motions as a side effect of warpMouse()
		 * @param evt
		 *	Event containing mouse data
		 */
		bool handleWarpMotion(const SDL_MouseMotionEvent& evt);

	public:
		/**
		 * Constructor
		 * @param sdlWindow
		 *	SDL_Window receiving SDL_Event
		 * @param emitter
		 *	BaseSystem which will send the events
		 */
		SdlInputHandler(SDL_Window* sdlWindow,
		                BaseSystem* emitter);
		/// Simple destructor
		virtual ~SdlInputHandler();

		/**
		 * Decide what to do with events
		 * @param evt
		 *	SDL_Event to handle
		 */
		void _handleSdlEvents(const SDL_Event& evt);

		/// Locks the pointer to the window
		void setGrabMousePointer(bool grab);
		/**
		 * Set the mouse to relative positioning mode (when not supported
        	 * by hardware, we emulate the behavior).
        	 * From the SDL docs: While the mouse is in relative mode, the
        	 * cursor is hidden, and the driver will try to report continuous
        	 * motion in the current window. Only relative motion events will
        	 * be delivered, the mouse position will not change
		 */
		void setMouseRelative(bool relative);
		/// Show the cursor
		void setMouseVisible(bool visible);

		/**
		 * Add a new receiver
		 * @param receiver
		 *	The new receiver
		 */
		void addReceiver(BaseSystem* receiver);
		/*
		 * Emit a SDL_Event to all receivers
		 * @param evt
		 *	Event to send
		 */
		void emit(const SDL_Event& evt);
	};
}

#endif // _SDLINPUTHANDLER_H_
