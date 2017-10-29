// Thanks to Jordan Milne and Scrawl for allowing to use their
// sdlinputwrapper files as base under the MIT license

/*
This file has been modified to fit with Swarthon's syntax rules
*/

#include "SdlInputHandler.h"
#include "InputListeners.h"

#include <SDL_syswm.h>

namespace Common {
	SdlInputHandler::SdlInputHandler(SDL_Window* sdlWindow,
	                                 BaseSystem* emitter)
	                : mSdlWindow(sdlWindow),
	                  mEmitter(emitter),
	                  mWantRelative(false),
	                  mWantMouseGrab(false),
	                  mWantMouseVisible(true),
	                  mIsMouseRelative(!mWantRelative),
	                  mWrapPointerManually(false),
	                  mGrabPointer(false),
	                  mMouseInWindow(true),
	                  mWindowHasFocus(true),
	                  mWarpX(0),
	                  mWarpY(0),
	                  mWarpCompensate(false) {}
	//------------------------------------------------------------------------------------------------
	SdlInputHandler::~SdlInputHandler() {}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::handleWindowEvent(const SDL_Event& evt) {
		switch (evt.window.event) {
		case SDL_WINDOWEVENT_ENTER:
			mMouseInWindow = true;
			updateMouseSettings();
			break;
		case SDL_WINDOWEVENT_LEAVE:
			mMouseInWindow = false;
			updateMouseSettings();
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mWindowHasFocus = true;
			updateMouseSettings();
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mWindowHasFocus = false;
			updateMouseSettings();
			break;
		}
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::_handleSdlEvents(const SDL_Event& evt) {
		switch (evt.type) {
		case SDL_MOUSEMOTION:
			// Ignore this if it happened due to a warp
			if (!handleWarpMotion(evt.motion)) {
                                // Try to keep the mouse inside the window
				if (mWindowHasFocus)
					wrapMousePointer(evt.motion);

				emit(evt);
			}
			break;
		case SDL_MOUSEWHEEL:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_TEXTINPUT:
		case SDL_JOYAXISMOTION:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			emit(evt);
			break;
		case SDL_JOYDEVICEADDED: break;
		case SDL_JOYDEVICEREMOVED: break;
		case SDL_WINDOWEVENT: handleWindowEvent(evt); break;
		}
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::setGrabMousePointer(bool grab) {
		mWantMouseGrab = grab;
		updateMouseSettings();
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::setMouseRelative(bool relative) {
		mWantRelative = relative;
		updateMouseSettings();
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::setMouseVisible(bool visible) {
		mWantMouseVisible = visible;
		updateMouseSettings();
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::updateMouseSettings(void) {
		mGrabPointer = mWantMouseGrab && mMouseInWindow && mWindowHasFocus;
		SDL_SetWindowGrab(mSdlWindow, mGrabPointer ? SDL_TRUE : SDL_FALSE);

		SDL_ShowCursor(mWantMouseVisible || !mWindowHasFocus);

		bool relative = mWantRelative && mMouseInWindow && mWindowHasFocus;
		if (mIsMouseRelative == relative)
			return;

		mIsMouseRelative = relative;

		mWrapPointerManually = false;

		int success = SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
		if (!relative || (relative && success != 0))
			mWrapPointerManually = true;

		SDL_PumpEvents();
		SDL_FlushEvent(SDL_MOUSEMOTION);
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::warpMouse(int x, int y) {
		SDL_WarpMouseInWindow(mSdlWindow, x, y);
		mWarpCompensate = true;
		mWarpX          = x;
		mWarpY          = y;
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::wrapMousePointer(const SDL_MouseMotionEvent& evt) {
		if (mIsMouseRelative || !mWrapPointerManually || !mGrabPointer)
			return;

		int width  = 0;
		int height = 0;

		SDL_GetWindowSize(mSdlWindow, &width, &height);

		const int FUDGE_FACTOR_X = width;
		const int FUDGE_FACTOR_Y = height;

		if (evt.x - FUDGE_FACTOR_X < 0 || evt.x + FUDGE_FACTOR_X > width || evt.y - FUDGE_FACTOR_Y < 0 || evt.y + FUDGE_FACTOR_Y > height) {
			warpMouse(width / 2, height / 2);
		}
	}
	//------------------------------------------------------------------------------------------------
	bool SdlInputHandler::handleWarpMotion(const SDL_MouseMotionEvent& evt) {
		if (!mWarpCompensate)
			return false;

		if (evt.x == mWarpX && evt.y == mWarpY) {
			mWarpCompensate = false;
			return true;
		}

		return false;
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::addReceiver(BaseSystem* receiver) {
		mReceivers.push_back(receiver);
	}
	//------------------------------------------------------------------------------------------------
	void SdlInputHandler::emit(const SDL_Event& evt) {
		for (size_t i = 0; i < mReceivers.size(); i++)
			mEmitter->queueSendMessage(mReceivers[i], Mq::SDL_EVENT, evt);
	}
}
