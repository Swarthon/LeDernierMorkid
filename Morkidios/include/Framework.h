#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <MorkidiosPrerequisites.h>

#include "Utils.h"
#include "Input.h"
#include "Language.h"

namespace Morkidios {

	class _MorkidiosExport Framework : public Ogre::Singleton<Framework>, OIS::KeyListener, OIS::MouseListener
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

		void saveConfigs();

		Ogre::Root*				mRoot;
		Ogre::RenderWindow*			mRenderWindow;
		Ogre::Viewport*				mViewport;
		Ogre::Log*				mLog;
		Ogre::Timer*				mTimer;

		Input* mInput;

	private:
		Framework(const Framework&);
		Framework& operator= (const Framework&);
	};

}

#endif
