#include "LDMKeyboardMenuState.h"

LDMKeyboardMenuState::LDMKeyboardMenuState(){
	mFrameEvent = Ogre::FrameEvent();
}
LDMKeyboardMenuState::~LDMKeyboardMenuState(){
}

void LDMKeyboardMenuState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	mCamera = mSceneManager->createCamera("KeyboardCam");
	mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	createScene();
	createGUI();
}
void LDMKeyboardMenuState::createScene(){
}
void LDMKeyboardMenuState::createGUI(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "LDMKeyboardMenuState");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
	mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mWindow->setAlwaysOnTop(true);
	mWindow->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::windowClicked, this));	

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("LDMKeyboardMenuStateFont", 40, true, "GreatVibes-Regular.ttf");	

	CEGUI::Window* button;

	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateAvancer");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.065,0)));
	button->setProperty("Text", CEGUI::String("Avancer : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gForwardMoveKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");
	button->setUserData(&Morkidios::gForwardMoveKey);

	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateReculer");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.195,0)));
	button->setProperty("Text", CEGUI::String("Reculer : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gBackwardMoveKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");
	button->setUserData(&Morkidios::gBackwardMoveKey);
	
	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateGauche");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.325,0)));
	button->setProperty("Text", CEGUI::String("Gauche : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gLeftMoveKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");	
	button->setUserData(&Morkidios::gLeftMoveKey);
	
	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateDroite");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.455,0)));
	button->setProperty("Text", CEGUI::String("Droite : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gRightMoveKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");	
	button->setUserData(&Morkidios::gRightMoveKey);

	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateSaut");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.585,0)));
	button->setProperty("Text", CEGUI::String("Saut : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gJumpKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");
	button->setUserData(&Morkidios::gJumpKey);

	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateCourir");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.715,0)));
	button->setProperty("Text", CEGUI::String("Courir : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gRunKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");
	button->setUserData(&Morkidios::gRunKey);

	button = mWindow->createChild("TaharezLook/Button", "LDMKeyboardMenuStateAccroupir");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::buttonClicked, this));	
	button->subscribeEvent(CEGUI::PushButton::EventKeyUp, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::keyPressed, this));	
	button->setWidth(CEGUI::UDim(0.2,0));
	button->setHeight(CEGUI::UDim(0.1,0));
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.845,0)));
	button->setProperty("Text", CEGUI::String("S'accroupir : ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(Morkidios::gSneakKey)));
	button->setProperty("Font","LDMKeyboardMenuStateFont");
	button->setUserData(&Morkidios::gSneakKey);

	mReturnButton = static_cast<CEGUI::PushButton*>(mWindow->createChild("AlfiskoSkin/Button", "ReturnButton"));
	mReturnButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMKeyboardMenuState::returnButtonPressed, this));	
	mReturnButton->setProperty("Text", "Retour");
	mReturnButton->setWidth(CEGUI::UDim(0.3,0));
	mReturnButton->setHeight(CEGUI::UDim(0.1,0));
	mReturnButton->setYPosition(CEGUI::UDim(0.8, 0));
	mReturnButton->setXPosition(CEGUI::UDim(0.6,0));
	mReturnButton->setProperty("Font","LDMKeyboardMenuStateFont");
	
	mActive = NULL;
}
void LDMKeyboardMenuState::exit(){
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("LDMKeyboardMenuState");
}
void LDMKeyboardMenuState::resume(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
	mWindow->setVisible(true);
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
}
bool LDMKeyboardMenuState::pause(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	mWindow->setVisible(false);

	return true;
}

bool LDMKeyboardMenuState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	return true;
}
bool LDMKeyboardMenuState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	if(keyEventRef.key == OIS::KC_ESCAPE)
		popState();
	return true;
}

bool LDMKeyboardMenuState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	return true;
}
bool LDMKeyboardMenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	return true;
}
bool LDMKeyboardMenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	return true;
}

void LDMKeyboardMenuState::update(double timeSinceLastFrame){
}


bool LDMKeyboardMenuState::returnButtonPressed(const CEGUI::EventArgs& e){
	popState();

	return true;
}
bool LDMKeyboardMenuState::buttonClicked(const CEGUI::EventArgs& evt){
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(evt);
	CEGUI::Window* button = we.window;

	if(mActive == button){
		std::string s, c = button->getText().c_str();
		bool b = false;
		for(int i = 0; i < c.size(); i++){
			if(b) {
				if(c[i] == ':')
					b = false;
				s += c[i];
			}
			else {
				if(c[i] == ']')
					b = true;
			}
		}
		button->setProperty("Text", CEGUI::String(s) + CEGUI::String(" ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(*((OIS::KeyCode*)(button->getUserData())))));
		mActive = NULL;
	}

	else {
		if(mActive){
			std::string s, c = mActive->getText().c_str();
			bool b = false;
			for(int i = 0; i < c.size(); i++){
				if(b) {
					if(c[i] == ':')
						b = false;
					s += c[i];
				}
				else
					if(c[i] == ']')
						b = true;
			}
			mActive->setProperty("Text", CEGUI::String(s) + CEGUI::String(" ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(*((OIS::KeyCode*)(mActive->getUserData())))));
			mActive = NULL;
		}

		std::string s, c = button->getText().c_str();
		bool b = false;
		for(int i = 0; i < c.size(); i++){
			if(!b) {
				if(c[i] == ':')
					b = true;
				s += c[i];
			}
		}
		button->setProperty("Text", CEGUI::String("[colour='FFFF0000]") + CEGUI::String(s));
		mActive = button;
	}

	return true;
}
bool LDMKeyboardMenuState::windowClicked(const CEGUI::EventArgs& evt){
	if(mActive){
		std::string s, c = mActive->getText().c_str();
		bool b = false;
		for(int i = 0; i < c.size(); i++){
			if(b) {
				if(c[i] == ':')
					b = false;
				s += c[i];
			}
			else
				if(c[i] == ']')
					b = true;
		}
		mActive->setProperty("Text", CEGUI::String(s) + CEGUI::String(" ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(*((OIS::KeyCode*)(mActive->getUserData())))));
		mActive = NULL;
	}

	return true;
}
bool LDMKeyboardMenuState::keyPressed(const CEGUI::EventArgs& evt){
	if(mActive){
		const CEGUI::KeyEventArgs& we = static_cast<const CEGUI::KeyEventArgs&>(evt);
		OIS::KeyCode* okey = (OIS::KeyCode*)(mActive->getUserData());
		*okey = (OIS::KeyCode)(we.scancode);

		std::string s, c = mActive->getText().c_str();
		bool b = false;
		for(int i = 0; i < c.size(); i++){
			if(b) {
				if(c[i] == ':')
					b = false;
				s += c[i];
			}
			else
				if(c[i] == ']')
					b = true;
		}
		mActive->setProperty("Text", CEGUI::String(s) + CEGUI::String(" ") + CEGUI::String(Morkidios::Framework::getSingletonPtr()->mKeyboard->getAsString(*((OIS::KeyCode*)(mActive->getUserData())))));
		mActive = NULL;
	}

	return true;
}