#ifndef LANGUAGECHANGESTATE_H
#define LANGUAGECHANGESTATE_H

// Morkidios includes
#include <Morkidios.h>

class LanguageChangeState : public Morkidios::State {
public:
	LanguageChangeState();
	virtual ~LanguageChangeState();

	DECLARE_APPSTATE_CLASS(LanguageChangeState)

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

#endif // LANGUAGECHANGESTATE_H
