#include "Global.h"

namespace Morkidios {

	// Keyboard
	std::map<OIS::KeyCode, bool> gKeysState;
	OIS::KeyCode gBackwardMoveKey = OIS::KC_S;
	OIS::KeyCode gForwardMoveKey = OIS::KC_Z;
	OIS::KeyCode gRightMoveKey = OIS::KC_D;
	OIS::KeyCode gLeftMoveKey = OIS::KC_Q;
	OIS::KeyCode gJumpKey = OIS::KC_SPACE;
	OIS::KeyCode gRunKey = OIS::KC_LMENU;
	OIS::KeyCode gSneakKey = OIS::KC_LSHIFT;

	// Mouse
	float gMouseSensibility = 0.5f;
}