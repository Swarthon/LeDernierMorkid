//Thanks to Jordan Milne and Scrawl for allowing to use their
//sdlinputwrapper files as base under the MIT license

/*
This file has been modified to fit with Swarthon's syntax rules
*/

#include "SdlInputHandler.h"
#include "InputListeners.h"

#include <SDL_syswm.h>

namespace Common {
    SdlInputHandler::SdlInputHandler( SDL_Window *sdlWindow,
                                      MouseListener *mouseListener,
                                      KeyboardListener *keyboardListener,
                                      JoystickListener *joystickListener ) :
        mSdlWindow( sdlWindow ),
        mLogicSystem( 0 ),
        mMouseListener( mouseListener ),
        mKeyboardListener( keyboardListener ),
        mJoystickListener( joystickListener ),
        mWantRelative( false ),
        mWantMouseGrab( false ),
        mWantMouseVisible( true ),
        mIsMouseRelative( !mWantRelative ),
        mWrapPointerManually( false ),
        mGrabPointer( false ),
        mMouseInWindow( true ),
        mWindowHasFocus( true ),
        mWarpX( 0 ),
        mWarpY( 0 ),
        mWarpCompensate( false ) {
    }
    //-----------------------------------------------------------------------------------
    SdlInputHandler::~SdlInputHandler() {
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::handleWindowEvent( const SDL_Event& evt ) {
        switch( evt.window.event ) {
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
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::_handleSdlEvents( const SDL_Event& evt ) {
        switch( evt.type ) {
            case SDL_MOUSEMOTION:
                // Ignore this if it happened due to a warp
                if( !handleWarpMotion(evt.motion) ) {
                    // If in relative mode, don't trigger events unless window has focus
                    if( (!mWantRelative || mWindowHasFocus) && mMouseListener )
                        mMouseListener->mouseMoved( evt );

                    // Try to keep the mouse inside the window
                    if (mWindowHasFocus)
                        wrapMousePointer( evt.motion );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_MOUSEWHEEL:
                {
                    if( mMouseListener )
                        mMouseListener->mouseMoved( evt );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                {
                    if( mMouseListener )
                        mMouseListener->mousePressed( evt.button, evt.button.button );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_MOUSEBUTTONUP:
                {
                    if( mMouseListener )
                        mMouseListener->mouseReleased( evt.button, evt.button.button );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_KEYDOWN:
                {
                    if( !evt.key.repeat && mKeyboardListener )
                        mKeyboardListener->keyPressed( evt.key );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_KEYUP:
                {
                    if( !evt.key.repeat && mKeyboardListener )
                        mKeyboardListener->keyReleased( evt.key );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_TEXTINPUT:
                {
                    if( mKeyboardListener )
                        mKeyboardListener->textInput( evt.text );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_JOYAXISMOTION:
                {
                    if( mJoystickListener )
                        mJoystickListener->joyAxisMoved( evt.jaxis, evt.jaxis.axis );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_JOYBUTTONDOWN:
                {
                    if( mJoystickListener )
                        mJoystickListener->joyButtonPressed( evt.jbutton, evt.jbutton.button );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_JOYBUTTONUP:
                {
                    if( mJoystickListener )
                        mJoystickListener->joyButtonReleased( evt.jbutton, evt.jbutton.button );

                    if( mLogicSystem )
                        mGraphicsSystem->queueSendMessage( mLogicSystem, Mq::SDL_EVENT, evt );
                }
                break;
            case SDL_JOYDEVICEADDED:
                break;
            case SDL_JOYDEVICEREMOVED:
                break;
            case SDL_WINDOWEVENT:
                handleWindowEvent(evt);
                break;
        }
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::setGrabMousePointer( bool grab ) {
        mWantMouseGrab = grab;
        updateMouseSettings();
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::setMouseRelative( bool relative ) {
        mWantRelative = relative;
        updateMouseSettings();
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::setMouseVisible( bool visible ) {
        mWantMouseVisible = visible;
        updateMouseSettings();
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::updateMouseSettings(void) {
        mGrabPointer = mWantMouseGrab && mMouseInWindow && mWindowHasFocus;
        SDL_SetWindowGrab( mSdlWindow, mGrabPointer ? SDL_TRUE : SDL_FALSE );

        SDL_ShowCursor( mWantMouseVisible || !mWindowHasFocus );

        bool relative = mWantRelative && mMouseInWindow && mWindowHasFocus;
        if( mIsMouseRelative == relative )
            return;

        mIsMouseRelative = relative;

        mWrapPointerManually = false;

        int success = SDL_SetRelativeMouseMode( relative ? SDL_TRUE : SDL_FALSE );
        if( !relative || (relative && success != 0) )
            mWrapPointerManually = true;

        SDL_PumpEvents();
        SDL_FlushEvent( SDL_MOUSEMOTION );
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::warpMouse( int x, int y ) {
        SDL_WarpMouseInWindow( mSdlWindow, x, y );
        mWarpCompensate = true;
        mWarpX = x;
        mWarpY = y;
    }
    //-----------------------------------------------------------------------------------
    void SdlInputHandler::wrapMousePointer( const SDL_MouseMotionEvent& evt ) {
        if( mIsMouseRelative || !mWrapPointerManually || !mGrabPointer )
            return;

        int width = 0;
        int height = 0;

        SDL_GetWindowSize( mSdlWindow, &width, &height );

        const int FUDGE_FACTOR_X = width;
        const int FUDGE_FACTOR_Y = height;

        if( evt.x - FUDGE_FACTOR_X < 0  || evt.x + FUDGE_FACTOR_X > width ||
            evt.y - FUDGE_FACTOR_Y < 0  || evt.y + FUDGE_FACTOR_Y > height ) {
            warpMouse( width / 2, height / 2 );
        }
    }
    //-----------------------------------------------------------------------------------
    bool SdlInputHandler::handleWarpMotion( const SDL_MouseMotionEvent& evt ) {
        if( !mWarpCompensate )
            return false;

        if( evt.x == mWarpX && evt.y == mWarpY ) {
            mWarpCompensate = false;
            return true;
        }

        return false;
    }
}