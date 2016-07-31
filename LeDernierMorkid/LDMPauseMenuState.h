#ifndef LDMPAUSEMENUSTATE_H
#define LDMPAUSEMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

class LDMPauseMenuState : public Morkidios::State {
public:
	LDMPauseMenuState();
	virtual ~LDMPauseMenuState();

	DECLARE_APPSTATE_CLASS(LDMPauseMenuState)

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
	bool continueButtonPressed(const CEGUI::EventArgs& e);
	bool optionsButtonPressed(const CEGUI::EventArgs& e);
	bool quitButtonPressed(const CEGUI::EventArgs& e);
	bool quitCancelButtonPressed(const CEGUI::EventArgs& e);
	bool quitMainMenuButtonPressed(const CEGUI::EventArgs& e);
	bool quitDesktopButtonPressed(const CEGUI::EventArgs& e);

	Ogre::Camera* mCamera;

	CEGUI::Window* mWindow;
	CEGUI::Window* mContinueButton;
	CEGUI::Window* mOptionsButton;
	CEGUI::Window* mQuitButton;
	CEGUI::Window* mStatsWindow;
	CEGUI::Window* mGreyWindow;

	CEGUI::Window* mQuitWindow;
	CEGUI::Window* mQuitCancel;
	CEGUI::Window* mQuitMainMenu;
	CEGUI::Window* mQuitDesktop;
};

#endif // LDMPAUSEMENUSTATE_H