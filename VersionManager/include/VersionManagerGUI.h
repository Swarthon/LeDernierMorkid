#ifndef VERSIONMANAGERGUI_H
#define VERSIONMANAGERGUI_H
// boost includes
#include <boost/thread/thread.hpp>

// OIS includes
#include <OIS/OIS.h>

// Updater includes
#include "VersionManager.h"

// My includes
#define NO_COLLISION
#include <State.h>
#include <StateManager.h>

namespace VersionManager {
	class VersionManagerGUI : public Morkidios::State {
	public:
		// Construction methodes
		VersionManagerGUI();
		~VersionManagerGUI();

		DECLARE_APPSTATE_CLASS(VersionManagerGUI)

		void enter();
		void createScene();
		void createGUI();
		void exit();
		void resume();
		bool pause();
		void update(double timeSinceLastFrame);

		bool keyPressed(const OIS::KeyEvent &keyEventRef);
		bool keyReleased(const OIS::KeyEvent &keyEventRef);

		bool mouseMoved(const OIS::MouseEvent &evt);
		bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

		void setProgressBarProgress(float f);
		void setProgressLabel(CEGUI::String s);
	private:
		// Private methodes
		void initVM();
		bool yesButtonClicked(const CEGUI::EventArgs& evt);
		bool noButtonClicked(const CEGUI::EventArgs& evt);

		void initVMWithInternet();
		void downloadFile(std::string versionName);
		bool downloadClicked(const CEGUI::EventArgs& evt);
		bool downloadButtonClicked(const CEGUI::EventArgs& evt);
		bool downloadAfterYesButtonClicked(const CEGUI::EventArgs& evt);
		bool downloadAfterNoButtonClicked(const CEGUI::EventArgs& evt);

		void initVMWithFiles();
		bool installClicked(const CEGUI::EventArgs& evt);
		bool installButtonClicked(const CEGUI::EventArgs& evt);

		Ogre::Camera* mCamera;
		CEGUI::Window* mWindow;

		CEGUI::Window* mHome;
		CEGUI::PushButton* mHomeDownload;
		CEGUI::PushButton* mHomeInstall;

		CEGUI::Window* mInstall;
		CEGUI::Combobox* mInstallCombobox;
		CEGUI::PushButton* mInstallButton;

		CEGUI::Window* mDownload;
		CEGUI::Combobox* mDownloadCombobox;
		CEGUI::PushButton* mDownloadButton;

		CEGUI::Window* mDownloadAfter;
		CEGUI::Window* mDownloadText;
		CEGUI::PushButton* mDownloadYesButton;
		CEGUI::PushButton* mDownloadNoButton;

		CEGUI::Window* mProgressWindow;
		CEGUI::Window* mProgressLabel;
		CEGUI::ProgressBar* mProgressBar;

		enum State {
			UserInput,
			InitVMInternet,
			InitVMFiles,
			Downloading,
		};

		State mState;
		boost::thread mThread;

		VersionManager mVersionManager;
	};
}
extern int setDlProgress(void *userdata, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t);


#endif // VERSIONMANAGERGUI_H
