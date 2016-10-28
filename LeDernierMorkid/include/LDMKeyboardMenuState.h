#ifndef LDMKEYBOARDMENUSTATE_H
#define LDMKEYBOARDMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

class LDMKeyboardMenuState : public Morkidios::State {
public:
	LDMKeyboardMenuState();
	virtual ~LDMKeyboardMenuState();

	DECLARE_APPSTATE_CLASS(LDMKeyboardMenuState)

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
	CEGUI::Window* mWindow;
	CEGUI::Window* mActive;
	CEGUI::PushButton* mReturnButton;

	// Private methodes
	void createButton();
	bool returnButtonPressed(const CEGUI::EventArgs& e);
	bool buttonClicked(const CEGUI::EventArgs& evt);
	bool windowClicked(const CEGUI::EventArgs& evt);
	bool keyPressed(const CEGUI::EventArgs& evt);

	Ogre::Camera* mCamera;
};

#endif // LDMKEYBOARDMENUSTATE_H
