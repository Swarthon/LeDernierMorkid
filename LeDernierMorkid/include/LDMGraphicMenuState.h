#ifndef LDMGRAPHICMENUSTATE_H
#define LDMGRAPHICMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

class LDMGraphicMenuState : public Morkidios::State {
public:
	LDMGraphicMenuState();
	virtual ~LDMGraphicMenuState();

	DECLARE_APPSTATE_CLASS(LDMGraphicMenuState)

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
	CEGUI::Scrollbar* mFOV;
	CEGUI::Window* mFOVLabel;
	CEGUI::PushButton* mFullScreen;
	CEGUI::PushButton* mReturnButton;

	// Private methodes
	bool returnButtonPressed(const CEGUI::EventArgs& e);
	bool FOVChanged(const CEGUI::EventArgs& e);
	bool fullScreenChanged(const CEGUI::EventArgs& e);

	Ogre::Camera* mCamera;
};

#endif // LDMGRAPHICMENUSTATE_H