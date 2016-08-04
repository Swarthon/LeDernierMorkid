#include "LDMMainMenuState.h"

LDMMainMenuState::LDMMainMenuState(){
	mFrameEvent = Ogre::FrameEvent();
}
LDMMainMenuState::~LDMMainMenuState(){
}

void LDMMainMenuState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager("OctreeSceneManager");
	mSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));

	createGUI();
	createScene();
}
void LDMMainMenuState::createScene(){
	mTexture = Morkidios::Framework::getSingletonPtr()->mRoot->getTextureManager()->createManual("RTT", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 512, 512, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *rtex = mTexture->getBuffer()->getRenderTarget();

	mHeroCamera = mSceneManager->createCamera("MainMenuHeroBaseCamera");
	mHeroCamera->setNearClipDistance(1);
	mHeroVieport = rtex->addViewport(mHeroCamera);
	mHeroVieport->setOverlaysEnabled(false);
	mHeroVieport->setClearEveryFrame(true);
	mHeroVieport->setBackgroundColour(Ogre::ColourValue::Black);

	// Get the last save and put the entity here
	mHeroEntity = mSceneManager->createEntity("MainMenuHeroEntity","Sinbad.mesh");

	mCamera = mSceneManager->createCamera("MainMenuCamera");
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode("MainMenuHeroSceneNode");
	mSceneNode->attachObject(mHeroEntity);
	mSceneNode->yaw(Ogre::Degree(90));

	mHeroCamera->setPosition(Ogre::Vector3(20,0,0));
	mHeroCamera->lookAt(Ogre::Vector3(0,0,0));

	CEGUI::Texture &guiTex = ((CEGUI::OgreRenderer*)(CEGUI::System::getSingleton().getRenderer()))->createTexture("RTT", mTexture);

	const CEGUI::Rectf rect(CEGUI::Vector2f(0.0f, 0.0f), guiTex.getOriginalDataSize());
	CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().create("BasicImage", "MainMenuHeroImage"));
	image->setTexture(&guiTex);
	image->setArea(rect);
	image->setAutoScaled(CEGUI::ASM_Both);
	
	mHeroWindow->setProperty("Image", "MainMenuHeroImage");
}
void LDMMainMenuState::createGUI(){
	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "MainMenu");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
	mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mWindow->setAlwaysOnTop(true);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("MainMenuLabelFont", 60, true, "GreatVibes-Regular.ttf");
	CEGUI::FontManager::getSingleton().createFreeTypeFont ("MainMenuButtonFont", 40, true, "GreatVibes-Regular.ttf");

	mLabel = mWindow->createChild("Generic/Label", "MainMenuLabel");
	mLabel->setProperty("Text", "Le Dernier Morkid");
	mLabel->setProperty("NormalTextColour", "FFFFFFFF");
	mLabel->setProperty("DisabledTextColour", "FFFFFFFF");
	mLabel->setProperty("Font", "MainMenuLabelFont");
	mLabel->setSize(CEGUI::USize(CEGUI::UDim(0.6,0), CEGUI::UDim(0.15,0)));
	mLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.05,0)));

	mPlayButton = mWindow->createChild("AlfiskoSkin/Button", "MainMenuPlayButton");
	mPlayButton->setProperty("Text", "Solo");
	mPlayButton->setProperty("Font", "MainMenuButtonFont");
	mPlayButton->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	mPlayButton->setVerticalAlignment(CEGUI::VA_CENTRE);
	mPlayButton->setXPosition(CEGUI::UDim(0.16,0));
	mPlayButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMMainMenuState::playButtonPressed, this));

	mHeroWindow = mWindow->createChild("TaharezLook/StaticImage", "MainMenuHeroWindow");
	mHeroWindow->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.9,0)));
	mHeroWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.65,0), CEGUI::UDim(0.05,0)));

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
}
void LDMMainMenuState::exit(){
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("MainMenu");
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("RTT");
	CEGUI::ImageManager::getSingleton().destroy("MainMenuHeroImage");
}

bool LDMMainMenuState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	return true;
}
bool LDMMainMenuState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	return true;
}

bool LDMMainMenuState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	return true;
}
bool LDMMainMenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	return true;
}
bool LDMMainMenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	return true;
}

void LDMMainMenuState::update(double timeSinceLastFrame){
	double numRoundPerMinutes = 10;
	double rotate = timeSinceLastFrame * (numRoundPerMinutes / 60.0 / 1000.0) * 360;

	mSceneNode->yaw(Ogre::Degree(rotate));
}

bool LDMMainMenuState::playButtonPressed(const CEGUI::EventArgs& e){
	changeState(findByName("GameState"));

	return true;
}