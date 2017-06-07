#include "GraphicsGameState.h"
#include "GraphicsSystem.h"
#include "CameraController.h"

#include <OgreSceneManager.h>

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreFrameStats.h>

#include <OgreHlmsManager.h>
#include <OgreHlms.h>
#include <OgreHlmsCompute.h>
#include <OgreGpuProgramManager.h>

extern const double cFrametime;

GraphicsGameState::GraphicsGameState() :
	mGraphicsSystem( 0 ),
	mEnableInterpolation( true ),
        mCameraController( 0 ) {
}
//-----------------------------------------------------------------------------------
GraphicsGameState::~GraphicsGameState() {
        delete mCameraController;
        mCameraController = 0;
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::_notifyGraphicsSystem(Common::GraphicsSystem *graphicsSystem) {
	mGraphicsSystem = graphicsSystem;
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::createScene(void) {
	mCameraController = new Common::CameraController(mGraphicsSystem, false);
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::update(float timeSinceLast) {
	float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / cFrametime;
	weight = std::min( 1.0f, weight );

	if( !mEnableInterpolation )
		weight = 0;

	mGraphicsSystem->updateGameEntities(mGraphicsSystem->getGameEntities( Ogre::SCENE_DYNAMIC ), weight);
	if(mCameraController)
		mCameraController->update(timeSinceLast);
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::keyPressed(const SDL_KeyboardEvent &arg) {
        if(mCameraController)
        	mCameraController->keyPressed(arg);
	GameState::keyPressed(arg);
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::keyReleased(const SDL_KeyboardEvent &arg) {
        if(mCameraController)
        	mCameraController->keyReleased(arg);
	GameState::keyReleased(arg);
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::mouseMoved(const SDL_Event &arg) {
	if(mCameraController)
        	mCameraController->mouseMoved(arg);

        GameState::mouseMoved( arg );
}
