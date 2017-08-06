#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "InputListeners.h"

namespace Common {

	/**
	 * @class GameState
	 * @brief
	 * 	Simple GameState class
	 * @remarks
	 *	This class does nothing
	 */
	class GameState : public MouseListener, public KeyboardListener, public JoystickListener {
	public:
		/// Simple Destructor
		virtual ~GameState() {}
		/// Initialize
		virtual void initialize(void) {}
		/// Deinitialize
		virtual void deinitialize(void) {}
		/// Create the scene
		virtual void createScene(void) {}
		/// Destroy the scene
		virtual void destroyScene(void) {}
		/**
		 * Update the GameState
		 * param timeSinceLast
		 *	Time past since last frame
		 */
		virtual void update(float timeSinceLast) {}
		/// Method to call at the end of the frame
		virtual void              finishFrameParallel(void) {}
		/// Method to call at the end of the frame
		virtual void              finishFrame(void) {}
	};
}

#endif // _GAMESTATE_H_
