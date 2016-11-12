#ifndef OPTIONSMENUSTATE_H
#define OPTIONSMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

class OptionsMenuState : public Morkidios::State {
public:
	OptionsMenuState();
	virtual ~OptionsMenuState();

	DECLARE_APPSTATE_CLASS(OptionsMenuState)

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
	bool returnButtonPressed(const CEGUI::EventArgs& e);
	bool mouseSensibilityChanged(const CEGUI::EventArgs& e);
	bool graphicButtonPressed(const CEGUI::EventArgs& e);
	bool keyboardButtonPressed(const CEGUI::EventArgs& e);
	bool languageButtonPressed(const CEGUI::EventArgs& e);

	CEGUI::Window* mWindow;
	CEGUI::Scrollbar* mMouseSensibility;
	CEGUI::Window* mMouseSensibilityLabel;
	CEGUI::PushButton* mKeyboardMenuButton;
	CEGUI::PushButton* mGraphicsMenuButton;
	CEGUI::PushButton* mLanguageMenuButton;
	CEGUI::PushButton* mReturnButton;

	Ogre::Camera* mCamera;
};

#endif // OPTIONSMENUSTATE_H
