#include "LDMGraphicMenuState.h"

LDMGraphicMenuState::LDMGraphicMenuState(){
	mFrameEvent = Ogre::FrameEvent();
}
LDMGraphicMenuState::~LDMGraphicMenuState(){
}

void LDMGraphicMenuState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	mCamera = mSceneManager->createCamera("GraphicCam");
	mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	createScene();
	createGUI();
}
void LDMGraphicMenuState::createScene(){
}
void LDMGraphicMenuState::createGUI(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "GraphicMenu");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
	mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mWindow->setAlwaysOnTop(true);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("GraphicMenuFont", 40, true, "GreatVibes-Regular.ttf");

	mFOV = static_cast<CEGUI::Scrollbar*>(mWindow->createChild("TaharezLook/HorizontalScrollbar", "GraphicMenuFOV"));
	mFOV->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mFOV->setWidth(CEGUI::UDim(0.8,0));
	mFOV->setHeight(CEGUI::UDim(0.05,0));
	mFOV->setYPosition(CEGUI::UDim(0.1,0));
	mFOV->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&LDMGraphicMenuState::FOVChanged, this));
	mFOV->setUnitIntervalScrollPosition((Morkidios::GraphicOptions::getSingleton()->mFOV-Ogre::Degree(50).valueRadians())/Ogre::Degree(50).valueRadians());

	mFOVLabel = mWindow->createChild("Generic/Label", "GraphicMenuFOVLabel");
	mFOVLabel->setProperty("Text", _("Field of View"));
	mFOVLabel->setProperty("Font", "GraphicMenuFont");
	mFOVLabel->setProperty("NormalTextColour", "FFFFFFFF");
	mFOVLabel->setProperty("DisabledTextColour", "FFFFFFFF");
	mFOVLabel->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mFOVLabel->setWidth(CEGUI::UDim(0.8,0));
	mFOVLabel->setHeight(CEGUI::UDim(0.1,0));
	mFOVLabel->setYPosition(CEGUI::UDim(0.075,0));
	mFOVLabel->setAlwaysOnTop(true);
	mFOVLabel->setMousePassThroughEnabled(true);

	mCrossHair = static_cast<CEGUI::Scrollbar*>(mWindow->createChild("TaharezLook/HorizontalScrollbar", "GraphicMenuCrossHair"));
	mCrossHair->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mCrossHair->setWidth(CEGUI::UDim(0.8,0));
	mCrossHair->setHeight(CEGUI::UDim(0.05,0));
	mCrossHair->setYPosition(CEGUI::UDim(0.2,0));
	mCrossHair->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&LDMGraphicMenuState::CrossHairChanged, this));
	mCrossHair->setUnitIntervalScrollPosition(Morkidios::GraphicOptions::getSingleton()->mCrossHairSize.d_scale);

	mCrossHairLabel = mWindow->createChild("Generic/Label", "GraphicMenuCrossHairLabel");
	mCrossHairLabel->setProperty("Text", _("Cross Hair"));
	mCrossHairLabel->setProperty("Font", "GraphicMenuFont");
	mCrossHairLabel->setProperty("NormalTextColour", "FFFFFFFF");
	mCrossHairLabel->setProperty("DisabledTextColour", "FFFFFFFF");
	mCrossHairLabel->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mCrossHairLabel->setWidth(CEGUI::UDim(0.8,0));
	mCrossHairLabel->setHeight(CEGUI::UDim(0.1,0));
	mCrossHairLabel->setYPosition(CEGUI::UDim(0.175,0));
	mCrossHairLabel->setAlwaysOnTop(true);
	mCrossHairLabel->setMousePassThroughEnabled(true);

	mFullScreen = static_cast<CEGUI::PushButton*>(mWindow->createChild("TaharezLook/Button", "GraphicMenuFullscreen"));
	mFullScreen->setProperty("Text",(CEGUI::utf8*)_("Fullscreen"));
	mFullScreen->setProperty("Font", "GraphicMenuFont");
	if(Morkidios::GraphicOptions::getSingleton()->mFullScreen)
		mFullScreen->setText(mFullScreen->getText() + CEGUI::String(_(" : Yes")));
	else
		mFullScreen->setText(mFullScreen->getText() + CEGUI::String(_(" : No")));
	mFullScreen->setWidth(CEGUI::UDim(0.25,0));
	mFullScreen->setHeight(CEGUI::UDim(0.1,0));
	mFullScreen->setYPosition(CEGUI::UDim(0.3,0));
	mFullScreen->setXPosition(CEGUI::UDim(0.05,0));
	mFullScreen->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMGraphicMenuState::fullScreenChanged, this));

	mReturnButton = static_cast<CEGUI::PushButton*>(mWindow->createChild("AlfiskoSkin/Button", "ReturnButton"));
	mReturnButton->setProperty("Text", _("Return"));
	mReturnButton->setProperty("Font", "GraphicMenuFont");
	mReturnButton->setWidth(CEGUI::UDim(0.3,0));
	mReturnButton->setHeight(CEGUI::UDim(0.1,0));
	mReturnButton->setYPosition(CEGUI::UDim(0.8, 0));
	mReturnButton->setXPosition(CEGUI::UDim(0.6,0));
	mReturnButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMGraphicMenuState::returnButtonPressed, this));
}
void LDMGraphicMenuState::exit(){
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("GraphicMenu");

	Morkidios::GraphicOptions::getSingleton()->save();
}
void LDMGraphicMenuState::resume(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
	mWindow->setVisible(true);
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
	mFOVLabel->setProperty("Text", _("Field of View"));
	mCrossHairLabel->setProperty("Text", _("Cross Hair"));
	mReturnButton->setProperty("Text", _("Return"));
	mFullScreen->setProperty("Text",(CEGUI::utf8*)_("Fullscreen"));
	if(Morkidios::GraphicOptions::getSingleton()->mFullScreen)
		mFullScreen->setText(mFullScreen->getText() + CEGUI::String(_(" : Yes")));
	else
		mFullScreen->setText(mFullScreen->getText() + CEGUI::String(_(" : No")));
}
bool LDMGraphicMenuState::pause(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	mWindow->setVisible(false);

	return true;
}

bool LDMGraphicMenuState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	return true;
}
bool LDMGraphicMenuState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	if(keyEventRef.key == OIS::KC_ESCAPE)
		popState();
	return true;
}

bool LDMGraphicMenuState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	return true;
}
bool LDMGraphicMenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	return true;
}
bool LDMGraphicMenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	return true;
}

void LDMGraphicMenuState::update(double timeSinceLastFrame){
}


bool LDMGraphicMenuState::returnButtonPressed(const CEGUI::EventArgs& e){
	popState();

	return true;
}
bool LDMGraphicMenuState::FOVChanged(const CEGUI::EventArgs& e){
	double newVal = mFOV->getUnitIntervalScrollPosition() * Ogre::Degree(50).valueRadians() + Ogre::Degree(50).valueRadians();
	Morkidios::GraphicOptions::getSingleton()->mFOV = newVal;

	Morkidios::GraphicOptions::getSingleton()->config();

	return true;
}
bool LDMGraphicMenuState::CrossHairChanged(const CEGUI::EventArgs& e){
	double newVal = mCrossHair->getUnitIntervalScrollPosition();
	Morkidios::GraphicOptions::getSingleton()->mCrossHairSize = CEGUI::UDim(newVal,0);

	Morkidios::GraphicOptions::getSingleton()->config();

	return true;
}
bool LDMGraphicMenuState::fullScreenChanged(const CEGUI::EventArgs& e){
	Morkidios::GraphicOptions::getSingleton()->mFullScreen = !Morkidios::GraphicOptions::getSingleton()->mFullScreen;

	std::string s, c = mFullScreen->getText().c_str();
	bool b = false;
	for(int i = 0; i < c.size(); i++){
		if(c[i] == ':')
			b = true;
		if(!b)
			s += c[i];
	}
	if(Morkidios::GraphicOptions::getSingleton()->mFullScreen)
		mFullScreen->setText(CEGUI::String(s) + CEGUI::String(_(": Yes")));
	else
		mFullScreen->setText(CEGUI::String(s) + CEGUI::String(_(": No")));

	Morkidios::GraphicOptions::getSingleton()->config();

	return true;
}
