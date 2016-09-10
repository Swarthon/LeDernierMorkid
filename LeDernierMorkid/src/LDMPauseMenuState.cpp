#include "LDMPauseMenuState.h"

LDMPauseMenuState::LDMPauseMenuState(){
	mFrameEvent = Ogre::FrameEvent();
}
LDMPauseMenuState::~LDMPauseMenuState(){
}

void LDMPauseMenuState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	mCamera = mSceneManager->createCamera("PauseCam");
	mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	createScene();
	createGUI();
}
void LDMPauseMenuState::createScene(){
}
void LDMPauseMenuState::createGUI(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "PauseMenu");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
	mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mWindow->setAlwaysOnTop(true);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("PauseMenuButtonFont", 40, true, "GreatVibes-Regular.ttf");

	mContinueButton = mWindow->createChild("AlfiskoSkin/Button", "PauseMenuContinuerButton");
	mContinueButton->setProperty("Text", "Continuer");
	mContinueButton->setProperty("Font", "PauseMenuButtonFont");
	mContinueButton->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	mContinueButton->setHorizontalAlignment(CEGUI::HA_RIGHT);
	mContinueButton->setPosition(CEGUI::UVector2(CEGUI::UDim(-0.16,0), CEGUI::UDim(0.1,0)));
	mContinueButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMPauseMenuState::continueButtonPressed, this));

	mOptionsButton = mWindow->createChild("AlfiskoSkin/Button", "PauseMenuOptionsButton");
	mOptionsButton->setProperty("Text", "Options");
	mOptionsButton->setProperty("Font", "PauseMenuButtonFont");
	mOptionsButton->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	mOptionsButton->setVerticalAlignment(CEGUI::VA_CENTRE);
	mOptionsButton->setHorizontalAlignment(CEGUI::HA_RIGHT);
	mOptionsButton->setXPosition(CEGUI::UDim(-0.16,0));
	mOptionsButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMPauseMenuState::optionsButtonPressed, this));

	mQuitButton = mWindow->createChild("AlfiskoSkin/Button", "PauseMenuQuitButton");
	mQuitButton->setProperty("Text", "Quitter");
	mQuitButton->setProperty("Font", "PauseMenuButtonFont");
	mQuitButton->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	mQuitButton->setHorizontalAlignment(CEGUI::HA_RIGHT);
	mQuitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(-0.16,0), CEGUI::UDim(0.8,0)));
	mQuitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMPauseMenuState::quitButtonPressed, this));

	mStatsWindow = mWindow->createChild("TaharezLook/Listbox", "PauseMenuStatsWindow");
	mStatsWindow->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.9,0)));
	mStatsWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.05,0)));

	mQuitMenu = new LDMQuitMenu(mWindow);
	mQuitMenu->show(false);
	mQuitMenu->getCancel()->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMPauseMenuState::quitCancelButtonPressed, this));
	mQuitMenu->getMainMenu()->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMPauseMenuState::quitMainMenuButtonPressed, this));
	mQuitMenu->getDesktop()->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMPauseMenuState::quitDesktopButtonPressed, this));
}
void LDMPauseMenuState::exit(){
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("PauseMenu");
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("PauseMenuGreyWindowTexture");
	CEGUI::ImageManager::getSingleton().destroy("PauseMenuGreyWindowImage");

	delete mQuitMenu;
}
void LDMPauseMenuState::resume(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
	mWindow->setVisible(true);
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
	mQuitMenu->show(false);
}
bool LDMPauseMenuState::pause(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	mWindow->setVisible(false);
	mQuitMenu->show(false);

	return true;
}

bool LDMPauseMenuState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	return true;
}
bool LDMPauseMenuState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	
	if(keyEventRef.key == OIS::KC_ESCAPE && !mQuitMenu->getWindow()->isVisible())
		popState();
	else if(keyEventRef.key == OIS::KC_ESCAPE && mQuitMenu->getWindow()->isVisible())
		mQuitMenu->show(false);
	return true;
}

bool LDMPauseMenuState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	return true;
}
bool LDMPauseMenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	return true;
}
bool LDMPauseMenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	return true;
}

void LDMPauseMenuState::update(double timeSinceLastFrame){
}

bool LDMPauseMenuState::continueButtonPressed(const CEGUI::EventArgs& e){
	popState();

	return true;
}
bool LDMPauseMenuState::optionsButtonPressed(const CEGUI::EventArgs& e){
	pushState(findByName("OptionsMenuState"));

	return true;
}
bool LDMPauseMenuState::quitButtonPressed(const CEGUI::EventArgs& e){
	mQuitMenu->show(true);

	return true;
}
bool LDMPauseMenuState::quitCancelButtonPressed(const CEGUI::EventArgs& e){
	mQuitMenu->show(false);

	return true;
}
bool LDMPauseMenuState::quitMainMenuButtonPressed(const CEGUI::EventArgs& e){
	popAllAndPushState(findByName("MainMenuState"));

	return true;
}
bool LDMPauseMenuState::quitDesktopButtonPressed(const CEGUI::EventArgs& e){
	shutdown();

	return true;
}