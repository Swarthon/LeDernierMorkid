#include "LanguageChangeState.h"

LanguageChangeState::LanguageChangeState(){
	mFrameEvent = Ogre::FrameEvent();
}
LanguageChangeState::~LanguageChangeState(){
}

void LanguageChangeState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	mCamera = mSceneManager->createCamera("KeyboardCam");
	mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	createScene();
	createGUI();
}
void LanguageChangeState::createScene(){
}
void LanguageChangeState::createGUI(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "LanguageChangeState");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
	mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mWindow->setAlwaysOnTop(true);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("LanguageChangeStateFont", 40, true, "GreatVibes-Regular.ttf");
	CEGUI::FontManager::getSingleton().createFreeTypeFont ("LanguageChangeStateComoboxFont", 15, true, "Jura-Light.ttf");

	mCombobox = (CEGUI::Combobox*)(mWindow->createChild("TaharezLook/Combobox", "LanguageChangeStateCombobox"));
	initCombobox();
	mCombobox->setSize(CEGUI::USize(CEGUI::UDim(0.4,0),CEGUI::UDim(0.5,0)));
	mCombobox->setYPosition(CEGUI::UDim(0.3,0));
	mCombobox->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mCombobox->setText((CEGUI::utf8*)_("Please select your language"));
	mCombobox->setReadOnly(true);
	mCombobox->setProperty("Font","LanguageChangeStateComoboxFont");
	mCombobox->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&LanguageChangeState::languageChanged, this));

	mReturnButton = static_cast<CEGUI::PushButton*>(mWindow->createChild("AlfiskoSkin/Button", "ReturnButton"));
	mReturnButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LanguageChangeState::returnButtonPressed, this));
	mReturnButton->setProperty("Text", (CEGUI::utf8*)_("Return"));
	mReturnButton->setWidth(CEGUI::UDim(0.3,0));
	mReturnButton->setHeight(CEGUI::UDim(0.1,0));
	mReturnButton->setYPosition(CEGUI::UDim(0.01, 0));
	mReturnButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mReturnButton->setProperty("Font","LanguageChangeStateFont");
}
void LanguageChangeState::exit(){
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("LanguageChangeState");
}
void LanguageChangeState::resume(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
	mWindow->setVisible(true);
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
}
bool LanguageChangeState::pause(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	mWindow->setVisible(false);

	return true;
}

bool LanguageChangeState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	return true;
}
bool LanguageChangeState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	if(keyEventRef.key == OIS::KC_ESCAPE)
		popState();

	return true;
}

bool LanguageChangeState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	return true;
}
bool LanguageChangeState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	return true;
}
bool LanguageChangeState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	return true;
}

void LanguageChangeState::update(double timeSinceLastFrame){
}

// Private Methodes
void LanguageChangeState::initCombobox(){
	for (auto& i: Morkidios::Language::getSingleton()->getLanguages())
		mCombobox->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)i.first.c_str()));
}
bool LanguageChangeState::returnButtonPressed(const CEGUI::EventArgs& e){
	popState();

	return true;
}
bool LanguageChangeState::languageChanged(const CEGUI::EventArgs& e){
	Morkidios::Language::getSingleton()->setLanguage(mCombobox->getSelectedItem()->getText().c_str());
	Morkidios::Language::getSingleton()->save();
	mReturnButton->setProperty("Text", (CEGUI::utf8*)_("Return"));

	return true;
}
