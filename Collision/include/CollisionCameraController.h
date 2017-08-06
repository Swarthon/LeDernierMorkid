#ifndef _COLLISIONCAMERACONTROLLER_H_
#define _COLLISIONCAMERACONTROLLER_H_

#include "CameraController.h"

namespace Collision {

	/**
	 * @class CollisionCameraController
	 * @brief
	 * 	Camera Controller using Collision
	 *
	 * Camera Controller using btGhostObject and btKinematicCharacterController to move the Camera
	 */
	class CollisionCameraController : public Common::CameraController {
	private:
		/// btDynamicsWorld in which the Camera evolve
		btDynamicsWorld* mWorld;
		/// Attribute which provide collision to the Camera
		btPairCachingGhostObject* mGhostObject;
		/// Kinematic Object used to control mGhostObject
		btKinematicCharacterController* mCharacter;

	public:
		/**
		 * Constructor of CollisionCameraController
		 * @param graphicsSystem
		 * 	The Common::GraphicsSystem which owned the camera
		 * @param world
		 *	The World in which the CollisionCameraController will evolve
		 */
		CollisionCameraController(Common::GraphicsSystem* graphicsSystem, btDynamicsWorld* world);

		/**
	         * Method called in order to move and rotate the btGhostObject and synchronize the Ogre::Camera
		 * @param timeSinceLast
		 *	The time past since last call
		 */
		virtual void update(float timeSinceLast);
	};
}

#endif // _CAMERACONTROLLER_H_
