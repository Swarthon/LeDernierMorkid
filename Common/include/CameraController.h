#ifndef _CAMERACONTROLLER_H_
#define _CAMERACONTROLLER_H_

#include <OgrePrerequisites.h>

#include "GraphicsSystem.h"

class btDynamicsWorld;
class btPairCachingGhostObject;
class btKinematicCharacterController;

namespace Common {

	/**
	 * @class CameraController
	 * @brief
	 *	Simple CameraController
	 * @ingroup Common
	 *
	 * This class does not implement more than rotate and move a Ogre::Camera or Ogre::SceneNode
	 */
	class CameraController {
	protected:
		/**
		 * @enum Action
		 * @ingroup Common
		 * Enum describing the available Actions
		 */
		enum Action {
			Forward,
			Backward,
			Leftward,
			Rightward,
			Up,
			Down,
			Run,
			NUM_ACTION_IDS
		};
		typedef std::pair<SDL_Keycode, bool> KeyState;
		typedef std::map<Action, KeyState>   KeymapState;

		/**
		 * @var mUseSceneNode
		 * @brief
 		 *	Use SceneNode or not
		 * @see
 		 *	CameraController::CameraController
		 */
		bool mUseSceneNode;

		/// Factor of speed
		bool        mSpeedModifier;
		/// Total Y rotation of the camera
		float       mCameraYaw;
		/// Total X rotation of the camera
		float       mCameraPitch;
		/// States of keys
		KeymapState mKeymapState;

		/// GraphicsSystem to which belongs the camera
		GraphicsSystem* mGraphicsSystem;

	public:
		/// Camera speed
		float mCameraBaseSpeed;
		/// Camera speed boost
		float mCameraSpeedBoost;

	public:
		/**
		 * Constructor
		 * @param graphicsSystem
 		 *	GraphicsSystem to which belongs the camera
		 * @param useSceneNode
 		 *	Choose whether to use Ogre::SceneNode or not
		 */
		CameraController(GraphicsSystem* graphicsSystem, bool useSceneNode = false);

		/**
		 * Method to update the CameraController. Call it once per frame
		 * @param timeSinceLast
 		 *	Time since the last frame
		 */
		virtual void update(float timeSinceLast);

		/**
		 * Method called when a key is pressed
		 * @param arg
 		 *	Event containing all data needed
		 * @return true
 		 *	if we've handled the event
		 */
		virtual bool keyPressed(const SDL_KeyboardEvent& arg);
		/**
		 * Method called when a key is released
		 * @param arg
 		 *	Event containing all data needed
		 * @return true
 		 *	if we've handled the event
		 */
		virtual bool keyReleased(const SDL_KeyboardEvent& arg);

		/**
 		 * Method called when the mouse is moved
		 * @param arg
 		 *	Event containing all data needed
 		 */
		virtual void mouseMoved(const SDL_Event& arg);
	};
}

#endif
