#ifndef _CAMERACONTROLLER_H_
#define _CAMERACONTROLLER_H_

#include <OgrePrerequisites.h>

#include "GameState.h"
#include "GraphicsSystem.h"

class btDynamicsWorld;
class btPairCachingGhostObject;
class btKinematicCharacterController;

namespace Common {
	class CameraController {
		enum Action {
			Forward,
			Backward,
			Leftward,
			Rightward,
			Up,
			Down,
			Run
		};
		typedef std::pair<SDL_Keycode, bool> KeyState;
		typedef std::map<Action, KeyState>   KeymapState;

	protected:
		bool mUseSceneNode;
		bool mUseCollision;

		bool        mSpeedModifier;
		KeymapState mKeymapState;
		float       mCameraYaw;
		float       mCameraPitch;

		btDynamicsWorld*                mWorld;
		btPairCachingGhostObject*       mGhostObject;
		btKinematicCharacterController* mCharacter;

	public:
		float mCameraBaseSpeed;
		float mCameraSpeedBoost;

	private:
		GraphicsSystem* mGraphicsSystem;

	public:
		CameraController(GraphicsSystem* graphicsSystem, bool useSceneNode = false, btDynamicsWorld* world = NULL);

		virtual void update(float timeSinceLast);

		/// Returns true if we've handled the event
		virtual bool keyPressed(const SDL_KeyboardEvent& arg);
		/// Returns true if we've handled the event
		virtual bool keyReleased(const SDL_KeyboardEvent& arg);

		virtual void mouseMoved(const SDL_Event& arg);

		virtual void initCollisions();
	};
}

#endif
