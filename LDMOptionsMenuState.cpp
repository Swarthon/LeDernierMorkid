#include "LDMOptionsMenuState.h"

LDMOptionsMenuState::LDMOptionsMenuState(){
	mFrameEvent = Ogre::FrameEvent();
}
LDMOptionsMenuState::~LDMOptionsMenuState(){
}

void LDMOptionsMenuState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	mCamera = mSceneManager->createCamera("OptionsCam");
	mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	createScene();
	createGUI();
}
void LDMOptionsMenuState::createScene(){
}
void LDMOptionsMenuState::createGUI(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "OptionsMenu");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
	mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mWindow->setAlwaysOnTop(true);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("OptionsMenuButtonFont", 40, true, "GreatVibes-Regular.ttf");

	mMouseSensibility = static_cast<CEGUI::Scrollbar*>(mWindow->createChild("TaharezLook/HorizontalScrollbar", "OptionsMenuMouseSensibility"));
	mMouseSensibility->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mMouseSensibility->setWidth(CEGUI::UDim(0.8,0));
	mMouseSensibility->setHeight(CEGUI::UDim(0.05,0));
	mMouseSensibility->setYPosition(CEGUI::UDim(0.1,0));
	mMouseSensibility->setUnitIntervalScrollPosition(Morkidios::gMouseSensibility);
	mMouseSensibility->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&LDMOptionsMenuState::mouseSensibilityChanged, this));

	mMouseSensibilityLabel = mWindow->createChild("Generic/Label", "OptionsMenuMouseSensibilityLabel");
	mMouseSensibilityLabel->setProperty("Text", "Sensibilite de la souris");
	mMouseSensibilityLabel->setProperty("Font", "OptionsMenuButtonFont");
	mMouseSensibilityLabel->setProperty("NormalTextColour", "FFFFFFFF");
	mMouseSensibilityLabel->setProperty("DisabledTextColour", "FFFFFFFF");
	mMouseSensibilityLabel->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mMouseSensibilityLabel->setWidth(CEGUI::UDim(0.8,0));
	mMouseSensibilityLabel->setHeight(CEGUI::UDim(0.1,0));
	mMouseSensibilityLabel->setYPosition(CEGUI::UDim(0.075,0));
	mMouseSensibilityLabel->setAlwaysOnTop(true);
	mMouseSensibilityLabel->setMousePassThroughEnabled(true);

	mKeyboardMenuButton = static_cast<CEGUI::PushButton*>(mWindow->createChild("AlfiskoSkin/Button", "KeyboardMenuOptions"));
	mKeyboardMenuButton->setProperty("Text", "Controles");
	mKeyboardMenuButton->setProperty("Font", "OptionsMenuButtonFont");
	mKeyboardMenuButton->setWidth(CEGUI::UDim(0.3,0));
	mKeyboardMenuButton->setHeight(CEGUI::UDim(0.1,0));
	mKeyboardMenuButton->setYPosition(CEGUI::UDim(0.2, 0));
	mKeyboardMenuButton->setXPosition(CEGUI::UDim(0.1,0));
	mKeyboardMenuButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMOptionsMenuState::keyboardButtonPressed, this));	

	mGraphicsMenuButton = static_cast<CEGUI::PushButton*>(mWindow->createChild("AlfiskoSkin/Button", "GraphicMenuOptions"));
	mGraphicsMenuButton->setProperty("Text", "Graphismes");
	mGraphicsMenuButton->setProperty("Font", "OptionsMenuButtonFont");
	mGraphicsMenuButton->setWidth(CEGUI::UDim(0.3,0));
	mGraphicsMenuButton->setHeight(CEGUI::UDim(0.1,0));
	mGraphicsMenuButton->setYPosition(CEGUI::UDim(0.2, 0));
	mGraphicsMenuButton->setXPosition(CEGUI::UDim(0.6,0));
	mGraphicsMenuButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMOptionsMenuState::graphicButtonPressed, this));

	mReturnButton = static_cast<CEGUI::PushButton*>(mWindow->createChild("AlfiskoSkin/Button", "ReturnButton"));
	mReturnButton->setProperty("Text", "Retour");
	mReturnButton->setProperty("Font", "OptionsMenuButtonFont");
	mReturnButton->setWidth(CEGUI::UDim(0.3,0));
	mReturnButton->setHeight(CEGUI::UDim(0.1,0));
	mReturnButton->setYPosition(CEGUI::UDim(0.8, 0));
	mReturnButton->setXPosition(CEGUI::UDim(0.6,0));
	mReturnButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMOptionsMenuState::returnButtonPressed, this));
}
void LDMOptionsMenuState::exit(){
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("OptionsMenu");
}
void LDMOptionsMenuState::resume(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
	mWindow->setVisible(true);
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
}
bool LDMOptionsMenuState::pause(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	mWindow->setVisible(false);

	return true;
}

bool LDMOptionsMenuState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	return true;
}
bool LDMOptionsMenuState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	if(keyEventRef.key == OIS::KC_ESCAPE)
		popState();
	return true;
}

bool LDMOptionsMenuState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	return true;
}
bool LDMOptionsMenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	return true;
}
bool LDMOptionsMenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	return true;
}

void LDMOptionsMenuState::update(double timeSinceLastFrame){
}

bool LDMOptionsMenuState::returnButtonPressed(const CEGUI::EventArgs& e){
	popState();

	return true;
}
bool LDMOptionsMenuState::graphicButtonPressed(const CEGUI::EventArgs& e){
	pushState(findByName("GraphicMenuState"));
	
	return true;
}
bool LDMOptionsMenuState::keyboardButtonPressed(const CEGUI::EventArgs& e){
	pushState(findByName("KeyboardMenuState"));
	
	return true;
}
bool LDMOptionsMenuState::mouseSensibilityChanged(const CEGUI::EventArgs& e){
	double newVal = mMouseSensibility->getUnitIntervalScrollPosition();
	Morkidios::gMouseSensibility = newVal;

	return true;
}