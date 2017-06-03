#include "GraphicsGameState.h"
#include "GraphicsSystem.h"

#include <OgreSceneManager.h>

#include <OgreRoot.h>
#include <OgreFrameStats.h>

#include <OgreHlmsManager.h>
#include <OgreHlms.h>
#include <OgreHlmsCompute.h>
#include <OgreGpuProgramManager.h>

using namespace Common;

extern const double cFrametime;

namespace Common {
    GraphicsGameState::GraphicsGameState() :
		mGraphicsSystem( 0 ),
        mEnableInterpolation( true ) {
    }
    //-----------------------------------------------------------------------------------
    GraphicsGameState::~GraphicsGameState() {
    }
    //-----------------------------------------------------------------------------------
    void GraphicsGameState::_notifyGraphicsSystem( GraphicsSystem *graphicsSystem ) {
        mGraphicsSystem = graphicsSystem;
    }
    //-----------------------------------------------------------------------------------
    void GraphicsGameState::createScene(void) {
    }
    //-----------------------------------------------------------------------------------
    void GraphicsGameState::update( float timeSinceLast ) {
		float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / cFrametime;
        weight = std::min( 1.0f, weight );

        if( !mEnableInterpolation )
            weight = 0;

        mGraphicsSystem->updateGameEntities( mGraphicsSystem->getGameEntities( Ogre::SCENE_DYNAMIC ), weight );
	}
    //-----------------------------------------------------------------------------------
    void GraphicsGameState::keyPressed( const SDL_KeyboardEvent &arg ) {
        GameState::keyPressed( arg );
    }
    //-----------------------------------------------------------------------------------
    void GraphicsGameState::keyReleased( const SDL_KeyboardEvent &arg ) {
        GameState::keyReleased( arg );
    }
    //-----------------------------------------------------------------------------------
    void GraphicsGameState::mouseMoved( const SDL_Event &arg ) {
        GameState::mouseMoved( arg );
    }
}
