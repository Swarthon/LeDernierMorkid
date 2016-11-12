#ifndef GAME_H
#define GAME_H

// Morkidios includes
#include <Morkidios.h>

// Core includes
#include <Core.h>

// My includes
#include "GameState.h"

class LDMGame {
public:
	// Construction methodes
	LDMGame();
private:
	// Private construction methodes
	void initGui();

	Morkidios::StateManager* mStateManager;
};

#endif // GAME_H
