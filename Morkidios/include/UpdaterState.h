#ifndef UPDATERSTATE_H
#define UPDATERSTATE_H

// OIS includes
#include <OIS/OIS.h>

// Updater includes
#include <Downloader.h>

// My includes
#include "State.h"

namespace Morkidios {
	class UpdaterState : public State {
	public:
		// Construction methodes
		UpdaterState();
		~UpdaterState();

		DECLARE_APPSTATE_CLASS(UpdaterState)

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
		// Private methodes
		bool downloadWindow(const CEGUI::EventArgs&);
		bool installWindow(const CEGUI::EventArgs&);
		void downloadVersions();

		Ogre::Camera* mCamera;
		CEGUI::Window* mWindow;

		CEGUI::Window* mVersionsDownloadWindow;
		CEGUI::Window* mDownloadingWindow;
		CEGUI::Window* mDownloadingWindowProgressBar;
		CEGUI::Window* mDownloadingWindowText;
		CEGUI::Window* mInstallWindow;

		enum State {
			DownloadingVersions,
			DownloadingFile,
			InstallingFile,
			UserInput,
		};

		State mState;
		boost::thread mThread;
	};
}

#endif // UPDATERSTATE_H