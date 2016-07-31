#ifndef LDMGAME_H
#define LDMGAME_H

// Morkidios
#include <Morkidios.h>

// My includes
#include "LDMGameState.h"
#include "LDMMainMenuState.h"
#include "LDMPauseMenuState.h"
#include "LDMOptionsMenuState.h"
#include "LDMGraphicMenuState.h"
#include "LDMKeyboardMenuState.h"

class LDMGame {
public:
	// Construction methodes
	LDMGame();
private:
	// Private construction methodes
	void initGui();

	Morkidios::StateManager* mStateManager;
};

#endif // LDMGAME_H