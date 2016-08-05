#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

#include "Utils.h"

namespace Morkidios {

	class Framework : public Ogre::Singleton<Framework>, OIS::KeyListener, OIS::MouseListener
	{
	public:
		Framework();
		~Framework();

		bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
		void updateOgre(double timeSinceLastFrame);

		bool keyPressed(const OIS::KeyEvent &keyEventRef);
		bool keyReleased(const OIS::KeyEvent &keyEventRef);

		bool mouseMoved(const OIS::MouseEvent &evt);
		bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

		Ogre::Root*				mRoot;
		Ogre::RenderWindow*			mRenderWindow;
		Ogre::Viewport*				mViewport;
		Ogre::Log*				mLog;
		Ogre::Timer*				mTimer;

		OIS::InputManager*			mInputManager;
		OIS::Keyboard*				mKeyboard;
		OIS::Mouse*				mMouse;
	
	private:
		Framework(const Framework&);
		Framework& operator= (const Framework&);
	};

}

#endif