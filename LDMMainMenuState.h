#ifndef LDMMAINMENUSTATE_H
#define LDMMAINMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

// My includes
#include "LDMMaze.h"

class LDMMainMenuState : public Morkidios::State {
public:
	LDMMainMenuState();
	virtual ~LDMMainMenuState();

	DECLARE_APPSTATE_CLASS(LDMMainMenuState)

	void enter();
	void createScene();
	void createGUI();
	void exit();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void update(double timeSinceLastFrame);

private:
	bool playButtonPressed(const CEGUI::EventArgs& e);

	CEGUI::Window* mWindow;
	CEGUI::Window* mLabel;
	CEGUI::Window* mPlayButton;
	CEGUI::Window* mHeroWindow;

	Ogre::Camera* mHeroCamera;
	Ogre::Viewport* mHeroVieport;
	Ogre::Entity* mHeroEntity;
	Ogre::SceneNode* mSceneNode;
	Ogre::TexturePtr mTexture;

	Ogre::Camera* mCamera;
};

#endif // LDMMAINMENUSTATE_H