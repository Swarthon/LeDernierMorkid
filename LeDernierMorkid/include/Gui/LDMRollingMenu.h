#ifndef LDMROLLINGMENU_H
#define LDMROLLINGMENU_H

// Ogre includes
#include <OGRE/Ogre.h>

// CEGUI includes
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include <CEGUI/RendererModules/Ogre/Texture.h>

// OIS include
#include <OIS/OIS.h>

// Morkidios includes
#include <Morkidios.h>

class LDMRollingMenu {
public:
	LDMRollingMenu(int numFaces, double faceWidth, double faceHeight);
	~LDMRollingMenu();

	// Input methodes
	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	// Various methodes
	void update(double timeSinceLastFrame);
	void show(bool b);

	// Return value methodes
	std::vector<CEGUI::Window*> getFaces();
private:
	CEGUI::GUIContext* mContext;
	CEGUI::TextureTarget* mTextureTarget;
	CEGUI::OgreRenderer* mRenderer;
	Ogre::Entity* mEntity;
	Ogre::SceneNode* mSceneNode;	
	Ogre::SceneManager* mSceneManager;
	Ogre::Camera* mCamera;
	Ogre::Viewport* mViewport;
	CEGUI::Window* mWindow;
	Ogre::TexturePtr mTexture;
	CEGUI::Window* mClickArea;
	CEGUI::Window* mDownButtonLeft;
	CEGUI::Window* mUpButtonLeft;

	Ogre::Degree mToRotate;
	double mLastClick;

	int mNumFaces;
	std::vector<CEGUI::Window*> mFaces;
	CEGUI::Window* mActive;

	void createMenuShape(int numFaces, double faceWidth, double faceHeight);
	bool down(const CEGUI::EventArgs& evt = CEGUI::EventArgs());
	bool up(const CEGUI::EventArgs& evt = CEGUI::EventArgs());

	// Event
	bool activeClicked(const CEGUI::EventArgs& evt = CEGUI::EventArgs());
};

#endif // LDMROLLINGMENU_H
