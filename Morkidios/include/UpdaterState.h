#ifndef UPDATERSTATE_H
#define UPDATERSTATE_H

// OIS includes
#include <OIS/OIS.h>

// Updater includes
#include <Downloader.h>
#include <Version.h>

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
		void download(std::string url, std::string fileName);
		void downloadFile();
		bool downloadButtonClicked(const CEGUI::EventArgs& evt);
		bool noButtonClicked(const CEGUI::EventArgs& evt);
		bool yesButtonClicked(const CEGUI::EventArgs& evt);

		Ogre::Camera* mCamera;
		CEGUI::Window* mWindow;

		CEGUI::Window* mDownloadingWindow;
		CEGUI::Window* mDownloadingWindowProgressBar;
		CEGUI::Window* mDownloadingWindowText;
		CEGUI::Window* mAvailableVersionsWindow;
		CEGUI::Combobox* mAvailableVersionsWindowCombobox;
		CEGUI::PushButton* mAvailableVersionsWindowButton;
		CEGUI::Window* mInstallWindow;
		CEGUI::Window* mInstallWindowText;
		CEGUI::PushButton* mInstallWindowYes;
		CEGUI::PushButton* mInstallWindowNo;

		enum State {
			DownloadingVersions,
			DownloadingFile,
			InstallingFile,
			UserInput,
		};

		State mState;
		boost::thread mThread;
		std::vector<Version> mVersions;
		Version mActualVersion;
		bool mThreadEnded;
	};
}

#endif // UPDATERSTATE_H