#ifndef GAMESTATE_H
#define GAMESTATE_H

// Morkidios includes
#include <Morkidios.h>

// LeDernierMorkid includes
#include "LDMMaze.h"

class GameState : public Morkidios::State {
public:
	GameState();
	virtual ~GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void createGUI();
	void exit();
	void resume();
	bool pause();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void update(double timeSinceLastFrame);

private:
	void heroMove();
	void heroRotate(const OIS::MouseEvent &keyEventRef);

	LDMMaze* mTerrain;
	Morkidios::Map* mMap;
	Morkidios::CrossHair* mCrossHair;

	// Debug
	bool mFPSVisible;
	CEGUI::Window* mFPSWindow;
};

#endif // GAMESTATE_H
