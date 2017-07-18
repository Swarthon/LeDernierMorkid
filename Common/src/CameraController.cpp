#include "CameraController.h"

#include "GraphicsSystem.h"

#include <OgreCamera.h>
#include <OgreRenderWindow.h>

using namespace Common;

namespace Common {
	CameraController::CameraController(GraphicsSystem* graphicsSystem, bool useSceneNode)
	                : mUseSceneNode(useSceneNode),
	                  mSpeedModifier(false),
	                  mCameraYaw(0),
	                  mCameraPitch(0),
	                  mCameraBaseSpeed(10),
	                  mCameraSpeedBoost(5),
	                  mGraphicsSystem(graphicsSystem) {
		mKeymapState[Forward].first   = SDLK_z;
		mKeymapState[Backward].first  = SDLK_s;
		mKeymapState[Leftward].first  = SDLK_q;
		mKeymapState[Rightward].first = SDLK_d;
		mKeymapState[Up].first        = SDLK_SPACE;
		mKeymapState[Down].first      = SDLK_LSHIFT;
		mKeymapState[Run].first       = SDLK_LALT;
	}
	//------------------------------------------------------------------------------------------------
	void CameraController::update(float timeSinceLast) {
		Ogre::Camera* camera = mGraphicsSystem->getCamera();

		if (mCameraYaw || mCameraPitch) {
			if (mUseSceneNode) {
				Ogre::Node* cameraNode = camera->getParentNode();

				// Update now as yaw needs the derived orientation.
				cameraNode->_getFullTransformUpdated();
				cameraNode->yaw(Ogre::Radian(mCameraYaw), Ogre::Node::TS_WORLD);
				cameraNode->pitch(Ogre::Radian(mCameraPitch));
			}
			else {
				camera->yaw(Ogre::Radian(mCameraYaw));
				camera->pitch(Ogre::Radian(mCameraPitch));
			}

			mCameraYaw   = 0.0f;
			mCameraPitch = 0.0f;
		}

		int camMovementZ = mKeymapState[Backward].second - mKeymapState[Forward].second;
		int camMovementX = mKeymapState[Rightward].second - mKeymapState[Leftward].second;
		int slideUpDown  = mKeymapState[Up].second - mKeymapState[Down].second;

		Ogre::Vector3 camMovementDir(camMovementX, slideUpDown, camMovementZ);
		camMovementDir.normalise();
		camMovementDir *= timeSinceLast * mCameraBaseSpeed * (1 + mSpeedModifier * mCameraSpeedBoost);
		if (camMovementZ || camMovementX || slideUpDown) {
			if (mUseSceneNode) {
				Ogre::Node* cameraNode = camera->getParentNode();
				cameraNode->translate(camMovementDir, Ogre::Node::TS_LOCAL);
			}
			camera->moveRelative(camMovementDir);
		}
	}
	//------------------------------------------------------------------------------------------------
	bool CameraController::keyPressed(const SDL_KeyboardEvent& arg) {
		if (arg.keysym.sym == mKeymapState[Run].first)
			mSpeedModifier = true;

		if (arg.keysym.sym == mKeymapState[Forward].first)
			mKeymapState[Forward].second = true;
		else if (arg.keysym.sym == mKeymapState[Backward].first)
			mKeymapState[Backward].second = true;
		else if (arg.keysym.sym == mKeymapState[Leftward].first)
			mKeymapState[Leftward].second = true;
		else if (arg.keysym.sym == mKeymapState[Rightward].first)
			mKeymapState[Rightward].second = true;
		else if (arg.keysym.sym == mKeymapState[Up].first)
			mKeymapState[Up].second = true;
		else if (arg.keysym.sym == mKeymapState[Down].first)
			mKeymapState[Down].second = true;
		else
			return false;

		return true;
	}
	//------------------------------------------------------------------------------------------------
	bool CameraController::keyReleased(const SDL_KeyboardEvent& arg) {
		if (arg.keysym.sym == mKeymapState[Run].first)
			mSpeedModifier = false;

		if (arg.keysym.sym == mKeymapState[Forward].first)
			mKeymapState[Forward].second = false;
		else if (arg.keysym.sym == mKeymapState[Backward].first)
			mKeymapState[Backward].second = false;
		else if (arg.keysym.sym == mKeymapState[Leftward].first)
			mKeymapState[Leftward].second = false;
		else if (arg.keysym.sym == mKeymapState[Rightward].first)
			mKeymapState[Rightward].second = false;
		else if (arg.keysym.sym == mKeymapState[Up].first)
			mKeymapState[Up].second = false;
		else if (arg.keysym.sym == mKeymapState[Down].first)
			mKeymapState[Down].second = false;
		else
			return false;

		return true;
	}
	//------------------------------------------------------------------------------------------------
	void CameraController::mouseMoved(const SDL_Event& arg) {
		float width  = static_cast<float>(mGraphicsSystem->getRenderWindow()->getWidth());
		float height = static_cast<float>(mGraphicsSystem->getRenderWindow()->getHeight());

		mCameraYaw += -arg.motion.xrel / width;
		mCameraPitch += -arg.motion.yrel / height;
	}
}
