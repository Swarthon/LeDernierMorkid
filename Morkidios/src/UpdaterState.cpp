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

		mThread = boost::thread(boost::bind(&UpdaterState::downloadVersions, this));
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
		mDownloadingWindowText = mDownloadingWindow->createChild("Generic/Label","UpdaterStateDonwloadingWindowText");
		mDownloadingWindowText->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.1,0)));
		mDownloadingWindowText->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mDownloadingWindowText->setYPosition(CEGUI::UDim(0.2,0));
		mDownloadingWindowText->setProperty("Font","UpdaterStateFont");
		mDownloadingWindowText->setProperty("Text",(CEGUI::utf8*)"Téléchargement en cours ...");
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
		if(!mThread.try_join_for(boost::chrono::seconds(0)) && mState == DownloadingVersions){
			((CEGUI::ProgressBar*)mDownloadingWindowProgressBar)->step();
			if(((CEGUI::ProgressBar*)mDownloadingWindowProgressBar)->getProgress() == 100)
				((CEGUI::ProgressBar*)mDownloadingWindowProgressBar)->setProgress(0);
		}
		if(mThread.try_join_for(boost::chrono::seconds(0)) && mState == DownloadingVersions){
			mVersionsDownloadWindow->setVisible(false);
		}
	}

	// Private methodes
	bool UpdaterState::downloadWindow(const CEGUI::EventArgs& evt){
	}
	void UpdaterState::downloadVersions(){
		Downloader downloader ("https://sourceforge.net/projects/lederniermorkid/files/v%200.5/bin/v0.5.zip/download", "versions.txt");
		downloader.run();
	}
}