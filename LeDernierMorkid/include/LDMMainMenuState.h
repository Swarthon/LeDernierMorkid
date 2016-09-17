#ifndef LDMMAINMENUSTATE_H
#define LDMMAINMENUSTATE_H

// Morkidios includes
#include <Morkidios.h>

// My includes
#include "LDMMaze.h"
#include "LDMRollingMenu.h"
#include "LDMQuitMenu.h"

class LDMMainMenuState : public Morkidios::State {
public:
	LDMMainMenuState();
	virtual ~LDMMainMenuState();

	DECLARE_APPSTATE_CLASS(LDMMainMenuState)

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
	bool playButtonPressed(const CEGUI::EventArgs& e);
	bool optionsButtonPressed(const CEGUI::EventArgs& e);
	bool quitButtonPressed(const CEGUI::EventArgs& e);
	bool updateButtonPressed(const CEGUI::EventArgs& e);

	CEGUI::Window* mWindow;
	CEGUI::Window* mLabel;
	CEGUI::Window* mPlayButton;
	CEGUI::Window* mHeroWindow;

	Ogre::Camera* mHeroCamera;
	Ogre::Viewport* mHeroViewport;
	Ogre::Entity* mHeroEntity;
	Ogre::SceneNode* mHeroSceneNode;
	Ogre::TexturePtr mHeroTexture;
	Ogre::SceneManager* mHeroSceneManager;

	Ogre::Camera* mCamera;

	LDMRollingMenu* mRollingMenu;
};

#endif // LDMMAINMENUSTATE_H