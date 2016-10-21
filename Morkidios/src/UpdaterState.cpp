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

		mVersionManager = VersionManager::VersionManager();
		mState = UserInput;
	}
	void UpdaterState::createScene(){
	}
	void UpdaterState::createGUI(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
		CEGUI::FontManager::getSingleton().createFreeTypeFont ("UpdaterStateFont", 40, true, "Jura-Light.ttf");
		CEGUI::FontManager::getSingleton().createFreeTypeFont ("UpdaterStateButtonFont", 25, true, "Jura-Light.ttf");
		CEGUI::FontManager::getSingleton().createFreeTypeFont ("UpdaterStateComoboxFont", 15, true, "Jura-Light.ttf");

		mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "UpdaterState");
		mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mWindow->setAlwaysOnTop(true);
		mWindow->setVisible(true);

		mHome = mWindow->createChild("DefaultWindow","UpdaterStateHomeWindow");
		mHome->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mHome->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mHomeDownload = (CEGUI::PushButton*)(mHome->createChild("TaharezLook/Button","UpdaterStateHomeDownload"));
		mHomeDownload->setSize(CEGUI::USize(CEGUI::UDim(0.2,0),CEGUI::UDim(0.1,0)));
		mHomeDownload->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mHomeDownload->setVerticalAlignment(CEGUI::VA_CENTRE);
		mHomeDownload->setYPosition(CEGUI::UDim(-0.1,0));
		mHomeDownload->setProperty("Font","UpdaterStateButtonFont");
		mHomeDownload->setProperty("Text",(CEGUI::utf8*)"Télécharger");
		mHomeDownload->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::downloadClicked, this));
		mHomeInstall = (CEGUI::PushButton*)(mHome->createChild("TaharezLook/Button","UpdaterStateHomeInstall"));
		mHomeInstall->setSize(CEGUI::USize(CEGUI::UDim(0.2,0),CEGUI::UDim(0.1,0)));
		mHomeInstall->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mHomeInstall->setVerticalAlignment(CEGUI::VA_CENTRE);
		mHomeInstall->setYPosition(CEGUI::UDim(0.1,0));
		mHomeInstall->setProperty("Font","UpdaterStateButtonFont");
		mHomeInstall->setProperty("Text",(CEGUI::utf8*)"Installer");
		mHomeInstall->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::installClicked, this));

		mInstall = mWindow->createChild("DefaultWindow","UpdaterStateInstallWindow");
		mInstall->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mInstall->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mInstallCombobox = (CEGUI::Combobox*)(mInstall->createChild("TaharezLook/Combobox","UpdaterStateInstallCombobox"));
		mInstallCombobox->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.5,0)));
		mInstallCombobox->setYPosition(CEGUI::UDim(0.3,0));
		mInstallCombobox->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mInstallCombobox->getEditbox()->setText((CEGUI::utf8*)"Sélectionnez la version à installer");
		mInstallCombobox->setReadOnly(true);
		mInstallCombobox->setProperty("Font","UpdaterStateComoboxFont");
		mInstallCombobox->setXPosition(CEGUI::UDim(-0.1,0));
		mInstallButton = (CEGUI::PushButton*)(mInstall->createChild("TaharezLook/Button","UpdaterStateInstallButton"));
		mInstallButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mInstallButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.1,0)));
		mInstallButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mInstallButton->setXPosition(CEGUI::UDim(0.2,0));
		mInstallButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mInstallButton->setProperty("Text",(CEGUI::utf8*)"Installer");
		mInstallButton->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::installButtonClicked, this));
		mInstall->setVisible(false);

		mDownload = mWindow->createChild("DefaultWindow","UpdaterStateDownloadWindow");
		mDownload->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mDownload->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mDownloadCombobox = (CEGUI::Combobox*)(mDownload->createChild("TaharezLook/Combobox","UpdaterStateDownloadCombobox"));
		mDownloadCombobox->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.5,0)));
		mDownloadCombobox->setYPosition(CEGUI::UDim(0.3,0));
		mDownloadCombobox->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadCombobox->getEditbox()->setText((CEGUI::utf8*)"Sélectionnez la version à télécharger");
		mDownloadCombobox->setReadOnly(true);
		mDownloadCombobox->setProperty("Font","UpdaterStateComoboxFont");
		mDownloadCombobox->setXPosition(CEGUI::UDim(-0.1,0));
		mDownloadButton = (CEGUI::PushButton*)(mDownload->createChild("TaharezLook/Button","UpdaterStateDownloadButton"));
		mDownloadButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mDownloadButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.1,0)));
		mDownloadButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadButton->setXPosition(CEGUI::UDim(0.2,0));
		mDownloadButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mDownloadButton->setProperty("Text",(CEGUI::utf8*)"Télécharger");
		mDownloadButton->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::downloadButtonClicked, this));
		mDownload->setVisible(false);

		mDownloadAfter = mWindow->createChild("DefaultWindow","UpdaterStateDownloadAfterWindow");
		mDownloadAfter->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mDownloadAfter->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mDownloadText = mDownloadAfter->createChild("TaharezLook/Label","UpdaterStateDownloadAfterText");
		mDownloadText->setSize(CEGUI::USize(CEGUI::UDim(0.6,0),CEGUI::UDim(0.1,0)));
		mDownloadText->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadText->setYPosition(CEGUI::UDim(0.2,0));
		mDownloadText->setProperty("Font","UpdaterStateFont");
		mDownloadText->setProperty("Text","Voulez-vous installer cette version ?");
		mDownloadYesButton = ((CEGUI::PushButton*)mDownloadAfter->createChild("TaharezLook/Button","UpdaterStateDownloadAfterYes"));
		mDownloadYesButton->setText("Oui");
		mDownloadYesButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadYesButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mDownloadYesButton->setYPosition(CEGUI::UDim(0.1,0));
		mDownloadYesButton->setXPosition(CEGUI::UDim(-0.1,0));
		mDownloadYesButton->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.1,0)));
		mDownloadYesButton->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::downloadAfterYesButtonClicked, this));
		mDownloadNoButton = ((CEGUI::PushButton*)mDownloadAfter->createChild("TaharezLook/Button","UpdaterStateDownloadAfterNo"));
		mDownloadNoButton->setText("Non");
		mDownloadNoButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadNoButton->setVerticalAlignment(CEGUI::VA_CENTRE);
		mDownloadNoButton->setYPosition(CEGUI::UDim(0.1,0));
		mDownloadNoButton->setXPosition(CEGUI::UDim(0.1,0));
		mDownloadNoButton->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.1,0)));
		mDownloadNoButton->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&UpdaterState::downloadAfterNoButtonClicked, this));
		mDownloadAfter->setVisible(false);

		mProgressWindow = mWindow->createChild("DefaultWindow","UpdaterStateProgressWindow");
		mProgressWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mProgressWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mProgressLabel = mProgressWindow->createChild("TaharezLook/Label","UpdaterStateProgressLabel");
		mProgressLabel->setSize(CEGUI::USize(CEGUI::UDim(0.6,0),CEGUI::UDim(0.1,0)));
		mProgressLabel->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mProgressLabel->setYPosition(CEGUI::UDim(0.2,0));
		mProgressBar = (CEGUI::ProgressBar*)mProgressWindow->createChild("TaharezLook/AlternateProgressBar","UpdaterStateProgressBar");
		mProgressBar->setSize(CEGUI::USize(CEGUI::UDim(0.7,0),CEGUI::UDim(0.15,0)));
		mProgressBar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.425,0)));
		mProgressWindow->setVisible(false);
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
	void UpdaterState::update(double timeSinceLastFrame){
		bool b = mThread.try_join_for(boost::chrono::duration<int, boost::milli>(1));
		if(b && mState == InitVMInternet){
			mState = UserInput;
			mDownload->setVisible(true);
			mProgressWindow->setVisible(false);
			std::vector<std::string> names = mVersionManager.getLoadedVersionsName();
			for(int i = 0; i < names.size(); i++)
				mDownloadCombobox->addItem(new CEGUI::ListboxTextItem(names[i]));
		}
		if(b && mState == InitVMFiles){
			mState = UserInput;
			mInstall->setVisible(true);
			mProgressWindow->setVisible(false);
			std::vector<std::string> names = mVersionManager.getDownloadedVersionsName();
			for(int i = 0; i < names.size(); i++)
				mInstallCombobox->addItem(new CEGUI::ListboxTextItem(names[i]));
		}
		if(b && mState == Downloading){
			if(std::find(mVersionManager.getDownloadedVersionsName().begin(),mVersionManager.getDownloadedVersionsName().end(),mDownloadCombobox->getSelectedItem()->getText().c_str()) == mVersionManager.getDownloadedVersionsName().end())
				popState();
			mState = UserInput;
			mDownloadAfter->setVisible(true);
			mProgressWindow->setVisible(false);
		}
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
	void UpdaterState::setProgressBarProgress(float f){
		mProgressBar->setProgress(f);
	}
	void UpdaterState::setProgressLabel(CEGUI::String s){
		mProgressLabel->setText(s);
	}

	// Private methodes
	void UpdaterState::initVMWithInternet(){
		mVersionManager.setDownloadPath("../../../Downloads/");
		mVersionManager.setInstallPath("../../../Versions/");
		mVersionManager.loadVersions();
		mVersionManager.loadFromInternet("https://sourceforge.net/projects/lederniermorkid/files/versions.txt/download");
	}
	void UpdaterState::downloadFile(std::string versionName){
		mVersionManager.download(versionName, &setDlProgress, this);
		mVersionManager.unzip(versionName);
	}
	bool UpdaterState::downloadClicked(const CEGUI::EventArgs& evt){
		mThread = boost::thread(boost::bind(&UpdaterState::initVMWithInternet, this));
		mHome->setVisible(false);
		mProgressWindow->setVisible(true);
		setProgressLabel((CEGUI::utf8*)"Téléchargement en cours");
		mState = InitVMInternet;

		return true;
	}
	bool UpdaterState::downloadButtonClicked(const CEGUI::EventArgs& evt){
		std::string name = mDownloadCombobox->getSelectedItem()->getText().c_str();
		for (size_t i = 0; i < mVersionManager.getLoadedVersionsName().size(); i++){
			if(mVersionManager.getLoadedVersionsName()[i] == name){
				mThread = boost::thread(boost::bind(&UpdaterState::downloadFile, this, name));
				mDownload->setVisible(false);
				mProgressWindow->setVisible(true);
				setProgressLabel((CEGUI::utf8*)"Téléchargement en cours");
				mState = Downloading;
				break;
			}
		}

		return true;
	}
	bool UpdaterState::downloadAfterYesButtonClicked(const CEGUI::EventArgs& evt){
		mVersionManager.install(mDownloadCombobox->getSelectedItem()->getText().c_str(), "LeDernierMorkid.exe", "Le Dernier Morkid", "../../../");
		popState();
		return true;
	}
	bool UpdaterState::downloadAfterNoButtonClicked(const CEGUI::EventArgs& evt){
		popState();
		return true;
	}

	void UpdaterState::initVMWithFiles(){
		mVersionManager.setDownloadPath("../../../Downloads/");
		mVersionManager.setInstallPath("../../../Versions/");
		mVersionManager.loadVersions();
	}
	bool UpdaterState::installClicked(const CEGUI::EventArgs& evt){
		mThread = boost::thread(boost::bind(&UpdaterState::initVMWithFiles, this));
		mHome->setVisible(false);
		mProgressWindow->setVisible(true);
		setProgressLabel((CEGUI::utf8*)"Téléchargement en cours");
		mState = InitVMFiles;

		return true;
	}
	bool UpdaterState::installButtonClicked(const CEGUI::EventArgs& evt){
		std::string name = mInstallCombobox->getSelectedItem()->getText().c_str();
		for (size_t i = 0; i < mVersionManager.getUnzipedVersionsName().size(); i++)
			if(mVersionManager.getUnzipedVersionsName()[i] == name){
				mVersionManager.install(name, "LeDernierMorkid.exe", "Le Dernier Morkid", "../../../");
				popState();
			}
		return true;
	}
}
int setDlProgress(void *userdata, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t){
	Morkidios::UpdaterState* us = (Morkidios::UpdaterState*)userdata;
	int total = (dltotal != 0 ? dltotal : 1);
	us->setProgressBarProgress((float)dlnow/(float)total);
	if(total != 1)
		us->setProgressLabel((CEGUI::utf8*)((std::string("Téléchargement en cours : ") + Morkidios::Utils::convertIntToString(dlnow) + std::string(" / ") + Morkidios::Utils::convertIntToString(dltotal)).c_str()));
	else
		us->setProgressLabel((CEGUI::utf8*)"Téléchargement en cours");
	return 0;
}
