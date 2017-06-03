#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "InputListeners.h"

namespace Common {
	class GameState : public MouseListener, public KeyboardListener, public JoystickListener {
	public:
		virtual ~GameState() {}

		virtual void initialize(void) {}
		virtual void deinitialize(void) {}

		virtual void createScene(void) {}

		virtual void destroyScene(void) {}

		virtual void update( float timeSinceLast ) {}
		virtual void finishFrameParallel(void) {}
		virtual void finishFrame(void) {}
	};
}

#endif // _GAMESTATE_H_
