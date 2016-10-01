#include "UpdaterState.h"

namespace Morkidios {
	UpdaterState::UpdaterState(){
		mState = UserInput;
	}
	UpdaterState::~UpdaterState(){
	}

	void UpdaterState::enter(){
		mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
		mSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));
		mCamera = mSceneManager->createCamera("UpdaterStateCam");
		mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
		Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

		createScene();
		createGUI();

		mThread = boost::thread(boost::bind(&UpdaterState::download, this, "https://sourceforge.net/projects/lederniermorkid/files/versions.txt/download", "versions.txt"));
		mThreadEnded = false;
		mState = DownloadingVersions;
	}
	void UpdaterState::createScene(){
	}
	void UpdaterState::createGUI(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
		CEGUI::FontManager::getSingleton().createFreeTypeFont ("UpdaterStateFont", 40, true, "Jura-Light.ttf");

		mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "UpdaterState");
		mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mWindow->setAlwaysOnTop(true);

		mDownloadingWindow = mWindow->createChild("DefaultWindow","UpdaterStateDownloadingWindow");
		mDownloadingWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mDownloadingWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mDownloadingWindow->setAlwaysOnTop(true);
		mDownloadingWindowProgressBar = mDownloadingWindow->createChild("TaharezLook/AlternateProgressBar","UpdaterStateDownloadingWindowProgressBar");
		mDownloadingWindowProgressBar->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.1,0)));
		mDownloadingWindowProgressBar->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadingWindowProgressBar->setVerticalAlignment(CEGUI::VA_CENTRE);
		mDownloadingWindowText = mDownloadingWindow->createChild("TaharezLook/Label","UpdaterStateDonwloadingWindowText");
		mDownloadingWindowText->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.1,0)));
		mDownloadingWindowText->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadingWindowText->setYPosition(CEGUI::UDim(0.2,0));
		mDownloadingWindowText->setProperty("Font","UpdaterStateFont");
		mDownloadingWindowText->setProperty("Text",(CEGUI::utf8*)"Téléchargement en cours ...");

		mAvailableVersionsWindow = mWindow->createChild("DefaultWindow","UpdaterStateAvailableVersionsWindow");
		mAvailableVersionsWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mAvailableVersionsWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mAvailableVersionsWindow->setAlwaysOnTop(true);
		mAvailableVersionsWindow->setVisible(false);
		mAvailableVersionsWindowCombobox = ((CEGUI::Combobox*)mAvailableVersionsWindow->createChild("TaharezLook/Combobox","UpdaterStateAvailableVersionsWindowCombobox"));
		mAvailableVersionsWindowCombobox->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.3,0)));
		mAvailableVersionsWindowCombobox->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mAvailableVersionsWindowCombobox->setXPosition(CEGUI::UDim(-0.1,0));
		mAvailableVersionsWindowCombobox->setVerticalAlignment(CEGUI::VA_CENTRE);
		mAvailableVersionsWindowCombobox->setReadOnly(true);
		mAvailableVersionsWindowButton = ((CEGUI::PushButton*)mAvailableVersionsWindow->createChild("TaharezLook/Button","UpdaterStateAvailableVersionsWindowButton"));
		mAvailableVersionsWindowButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.1,0)));
		mAvailableVersionsWindowButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mAvailableVersionsWindowButton->setXPosition(CEGUI::UDim(0.2,0));
		mAvailableVersionsWindowButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mAvailableVersionsWindowButton->setProperty("Text",(CEGUI::utf8*)"Télécharger");
		mAvailableVersionsWindowButton->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::downloadButtonClicked, this));

		mInstallWindow = mWindow->createChild("DefaultWindow","UpdaterStateInstallWindow");
		mInstallWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mInstallWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mInstallWindow->setAlwaysOnTop(true);
		mInstallWindow->setVisible(false);
		mInstallWindowText = mInstallWindow->createChild("TaharezLook/Label","UpdaterStateInstallWindowText");
		mInstallWindowText->setSize(CEGUI::USize(CEGUI::UDim(0.6,0),CEGUI::UDim(0.1,0)));
		mInstallWindowText->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mInstallWindowText->setYPosition(CEGUI::UDim(0.2,0));
		mInstallWindowText->setProperty("Font","UpdaterStateFont");
		mInstallWindowText->setProperty("Text","Voulez-vous installer cette version ?");
		mInstallWindowYes = ((CEGUI::PushButton*)mInstallWindow->createChild("TaharezLook/Button","UpdaterStateInstallWindowYes"));
		mInstallWindowYes->setText("Oui");
		mInstallWindowYes->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mInstallWindowYes->setVerticalAlignment(CEGUI::VA_CENTRE);
		mInstallWindowYes->setYPosition(CEGUI::UDim(0.1,0));
		mInstallWindowYes->setXPosition(CEGUI::UDim(-0.1,0));
		mInstallWindowYes->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.1,0)));
		mInstallWindowYes->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::yesButtonClicked, this));
		mInstallWindowNo = ((CEGUI::PushButton*)mInstallWindow->createChild("TaharezLook/Button","UpdaterStateInstallWindowNo"));
		mInstallWindowNo->setText("Non");
		mInstallWindowNo->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mInstallWindowNo->setVerticalAlignment(CEGUI::VA_CENTRE);
		mInstallWindowNo->setYPosition(CEGUI::UDim(0.1,0));
		mInstallWindowNo->setXPosition(CEGUI::UDim(0.1,0));
		mInstallWindowNo->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.1,0)));
		mInstallWindowNo->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::noButtonClicked, this));
	}
	void UpdaterState::exit(){
		Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("UpdaterState");
	}
	void UpdaterState::resume(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
		mWindow->setVisible(true);
		Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
	}
	bool UpdaterState::pause(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
		mWindow->setVisible(false);

		return true;
	}

	bool UpdaterState::keyPressed(const OIS::KeyEvent &keyEventRef){
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
		context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
		context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
		return true;
	}
	bool UpdaterState::keyReleased(const OIS::KeyEvent &keyEventRef){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
		if(keyEventRef.key == OIS::KC_ESCAPE)
			popState();
		return true;
	}

	bool UpdaterState::mouseMoved(const OIS::MouseEvent &evt){
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

		if (evt.state.Z.rel) // Scroll wheel.
			sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
		return true;
	}
	bool UpdaterState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
		return true;
	}
	bool UpdaterState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
		return true;
	}

	void UpdaterState::update(double timeSinceLastFrame){
		if(!mThreadEnded && (mState == DownloadingVersions || mState == DownloadingFile)){
			((CEGUI::ProgressBar*)mDownloadingWindowProgressBar)->step();
			if(((CEGUI::ProgressBar*)mDownloadingWindowProgressBar)->getProgress() == 1)
				((CEGUI::ProgressBar*)mDownloadingWindowProgressBar)->setProgress(0);
		}
		if(mThreadEnded && mState == DownloadingVersions){
			mVersions = Version::createVersions("versions.txt");
			for(int i = 0; i < mVersions.size(); i++)
				mAvailableVersionsWindowCombobox->addItem(new CEGUI::ListboxTextItem(mVersions[i].mName));

			mDownloadingWindow->setVisible(false);
			mAvailableVersionsWindow->setVisible(true);
			mState = UserInput;
		}
		if(mThreadEnded && mState == DownloadingFile){
			mDownloadingWindow->setVisible(false);
			mInstallWindow->setVisible(true);
			mState = UserInput;
		}
	}

	// Private methodes
	void UpdaterState::download(std::string url, std::string fileName){
		mThreadEnded = false;

		Downloader downloader (url, fileName);
		downloader.run();

		mThreadEnded = true;
	}
	void UpdaterState::downloadFile(){
		mThreadEnded = false;

		mActualVersion.downloadFile("../../../Versions/");

		mThreadEnded = true;
	}
	bool UpdaterState::downloadButtonClicked(const CEGUI::EventArgs& evt){
		mActualVersion = Version::searchVersionByName(mVersions,mAvailableVersionsWindowCombobox->getSelectedItem()->getText().c_str());
		mThread = boost::thread(boost::bind(&UpdaterState::downloadFile, this));
		mDownloadingWindow->setVisible(true);
		mAvailableVersionsWindow->setVisible(false);
		mState = DownloadingFile;

		return true;
	}
	bool UpdaterState::noButtonClicked(const CEGUI::EventArgs& evt){
		popState();
		return true;
	}
	bool UpdaterState::yesButtonClicked(const CEGUI::EventArgs& evt){
		mActualVersion.install("LeDernierMorkid.exe", "../../../Versions/","../../../");
		popState();
		return true;
	}
}
