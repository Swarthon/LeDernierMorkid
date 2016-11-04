#ifndef LDMLANGUAGECHANGESTATE_H
#define LDMLANGUAGECHANGESTATE_H

// Morkidios includes
#include <Morkidios.h>

class LDMLanguageChangeState : public Morkidios::State {
public:
	LDMLanguageChangeState();
	virtual ~LDMLanguageChangeState();

	DECLARE_APPSTATE_CLASS(LDMLanguageChangeState)

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
	// Methodes privates
	void initCombobox();
	bool returnButtonPressed(const CEGUI::EventArgs& e);
	bool languageChanged(const CEGUI::EventArgs& e);

	CEGUI::Window* mWindow;
	CEGUI::PushButton* mReturnButton;
	CEGUI::Combobox* mCombobox;

	Ogre::Camera* mCamera;
};

#endif // LDMLANGUAGECHANGESTATE_H
