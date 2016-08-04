#ifndef INVENTAIRE_H
#define INVENTAIRE_H

// My includes
#include "State.h"
#include "Terrain.h"
#include "Character/Hero.h"

namespace Morkidios {

	class Inventory : public Morkidios::State {
	public:
		Inventory();
		virtual ~Inventory();

		DECLARE_APPSTATE_CLASS(Inventory)

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
		Ogre::Camera* mCamera;

		CEGUI::Window* mWindow;

		CEGUI::ScrollablePane* mTypePane;
		CEGUI::VerticalLayoutContainer* mTypeBar;

		CEGUI::ScrollablePane* mObjectPane;
		CEGUI::VerticalLayoutContainer* mObjectBar;
		CEGUI::Window* mObjectLabel;

		CEGUI::VerticalLayoutContainer* mDescBar;
		CEGUI::Window* mDesc3D;
		CEGUI::Window* mDescText;
		
		CEGUI::Window* mActiveButton;


		// Desc3D
		Ogre::SceneManager* mSceneManager;
		Ogre::Camera* mObjectCamera;
		Ogre::Viewport* mObjectViewport;
		Ogre::Entity* mObjectEntity;
		Ogre::SceneNode* mObjectSceneNode;
		Ogre::TexturePtr mTexture;


		// Private methodes
		void createLeftBarButton(std::string name, std::string text, Object::Type t);
		void createObjectButton(std::string name, Object* o, CEGUI::Window* parent);
		bool typeBarButtonClicked(const CEGUI::EventArgs& e);
		bool objectButtonClicked(const CEGUI::EventArgs& e);
		bool equipeButtonClicked(const CEGUI::EventArgs& e);
		bool drop(const CEGUI::EventArgs& e);
		
		void clear();
		void refresh(Object::Type t);
	};
}

#endif // INVENTAIRE_H