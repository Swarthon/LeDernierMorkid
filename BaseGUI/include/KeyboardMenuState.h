#ifndef KEYBOARDMENUSTATE_H
#define KEYBOARDMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

class KeyboardMenuState : public Morkidios::State {
public:
	KeyboardMenuState();
	virtual ~KeyboardMenuState();

	DECLARE_APPSTATE_CLASS(KeyboardMenuState)

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
	CEGUI::ScrollablePane* mPane;
	CEGUI::PushButton* mReturnButton;

	// Private methodes
	void createButton(std::string name, OIS::KeyCode* k, int n, double width, double height, double offset);
	bool returnButtonPressed(const CEGUI::EventArgs& e);
	bool buttonClicked(const CEGUI::EventArgs& evt);
	bool windowClicked(const CEGUI::EventArgs& evt);
	bool keyPressed(const CEGUI::EventArgs& evt);

	Ogre::Camera* mCamera;
};

#endif // KEYBOARDMENUSTATE_H
