#ifndef GRAPHICMENUSTATE_H
#define GRAPHICMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

class GraphicMenuState : public Morkidios::State {
public:
	GraphicMenuState();
	virtual ~GraphicMenuState();

	DECLARE_APPSTATE_CLASS(GraphicMenuState)

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
	CEGUI::Scrollbar* mCrossHair;
	CEGUI::Window* mCrossHairLabel;
	CEGUI::PushButton* mFullScreen;
	CEGUI::PushButton* mReturnButton;

	// Private methodes
	bool returnButtonPressed(const CEGUI::EventArgs& e);
	bool FOVChanged(const CEGUI::EventArgs& e);
	bool CrossHairChanged(const CEGUI::EventArgs& e);
	bool fullScreenChanged(const CEGUI::EventArgs& e);

	Ogre::Camera* mCamera;
};

#endif // GRAPHICMENUSTATE_H
