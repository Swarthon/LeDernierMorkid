#ifndef GLOBAL_H
#define GLOBAL_H

// C++ includes
#include <map>

// OIS includes
#include <OIS/OIS.h>

// My includes
#include "Framework.h"

namespace Morkidios {

	// Keyboard
	extern OIS::KeyCode gBackwardMoveKey;
	extern OIS::KeyCode gForwardMoveKey;
	extern OIS::KeyCode gRightMoveKey;
	extern OIS::KeyCode gLeftMoveKey;
	extern OIS::KeyCode gJumpKey;	
	extern OIS::KeyCode gRunKey;
	extern OIS::KeyCode gSneakKey;

	// Mouse
	extern float gMouseSensibility;

}

#endif // GLOBAL_H