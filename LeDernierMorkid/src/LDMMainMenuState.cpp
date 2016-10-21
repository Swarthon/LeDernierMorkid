#include "LDMMainMenuState.h"

LDMMainMenuState::LDMMainMenuState(){
	mFrameEvent = Ogre::FrameEvent();
}
LDMMainMenuState::~LDMMainMenuState(){
}

void LDMMainMenuState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager("OctreeSceneManager");
	mSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));
	mHeroSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager("OctreeSceneManager");
	mHeroSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));

	createGUI();
	createScene();

	mRollingMenu = new LDMRollingMenu(5,100,50);
	mRollingMenu->getFaces()[0]->setText("Solo");
	mRollingMenu->getFaces()[0]->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LDMMainMenuState::playButtonPressed, this));
	mRollingMenu->getFaces()[1]->setText("Options");
	mRollingMenu->getFaces()[1]->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LDMMainMenuState::optionsButtonPressed, this));
	mRollingMenu->getFaces()[2]->setText((CEGUI::utf8*)"Mettre à jour");
	mRollingMenu->getFaces()[2]->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LDMMainMenuState::updateButtonPressed, this));
	mRollingMenu->getFaces()[3]->setText((CEGUI::utf8*)"Multijoueur (Prochainement)");
	mRollingMenu->getFaces()[4]->setText("Quitter");
	mRollingMenu->getFaces()[4]->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LDMMainMenuState::quitButtonPressed, this));
}
void LDMMainMenuState::createScene(){
	mHeroTexture = Morkidios::Framework::getSingletonPtr()->mRoot->getTextureManager()->createManual("RTTMainMenu", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 512, 512, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *rtex = mHeroTexture->getBuffer()->getRenderTarget();

	mHeroCamera = mHeroSceneManager->createCamera("MainMenuHeroBaseCamera");
	mHeroCamera->setNearClipDistance(1);
	mHeroViewport = rtex->addViewport(mHeroCamera);
	mHeroViewport->setClearEveryFrame(true);
	mHeroViewport->setBackgroundColour(Ogre::ColourValue::Black);

	// Get the last save and put the entity here
	mHeroEntity = mHeroSceneManager->createEntity("MainMenuHeroEntity","Sinbad.mesh");

	mCamera = mSceneManager->createCamera("MainMenuCamera");
	mCamera->setPosition(Ogre::Vector3(200,0,0));
	mCamera->lookAt(Ogre::Vector3(0,0,0));
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

	mHeroSceneNode = mHeroSceneManager->getRootSceneNode()->createChildSceneNode("MainMenuHeroSceneNode");
	mHeroSceneNode->attachObject(mHeroEntity);
	mHeroSceneNode->yaw(Ogre::Degree(90));

	mHeroCamera->setPosition(Ogre::Vector3(20,0,0));
	mHeroCamera->lookAt(Ogre::Vector3(0,0,0));

	CEGUI::Texture &guiTex = ((CEGUI::OgreRenderer*)(CEGUI::System::getSingleton().getRenderer()))->createTexture("RTTMainMenu", mHeroTexture);

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
	mWindow->setMousePassThroughEnabled(true);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("MainMenuLabelFont", 60, true, "GreatVibes-Regular.ttf");
	CEGUI::FontManager::getSingleton().createFreeTypeFont ("MainMenuButtonFont", 40, true, "GreatVibes-Regular.ttf");

	mLabel = mWindow->createChild("Generic/Label", "MainMenuLabel");
	mLabel->setProperty("Text", "Le Dernier Morkid");
	mLabel->setProperty("NormalTextColour", "FFFFFFFF");
	mLabel->setProperty("DisabledTextColour", "FFFFFFFF");
	mLabel->setProperty("Font", "MainMenuLabelFont");
	mLabel->setSize(CEGUI::USize(CEGUI::UDim(0.6,0), CEGUI::UDim(0.15,0)));
	mLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.05,0)));

	mHeroWindow = mWindow->createChild("TaharezLook/StaticImage", "MainMenuHeroWindow");
	mHeroWindow->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.9,0)));
	mHeroWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.65,0), CEGUI::UDim(0.05,0)));

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
}
void LDMMainMenuState::exit(){
	delete mRollingMenu;

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("MainMenu");
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("RTTMainMenu");
	CEGUI::ImageManager::getSingleton().destroy("MainMenuHeroImage");
}
void LDMMainMenuState::resume(){
	mWindow->setVisible(true);
	mRollingMenu->show(true);
	Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
}
bool LDMMainMenuState::pause(){
	mWindow->setVisible(false);
	mRollingMenu->show(false);

	return true;
}

bool LDMMainMenuState::keyPressed(const OIS::KeyEvent &keyEventRef){
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
	context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
	mRollingMenu->keyPressed(keyEventRef);
	return true;
}
bool LDMMainMenuState::keyReleased(const OIS::KeyEvent &keyEventRef){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
	mRollingMenu->keyReleased(keyEventRef);
	return true;
}

bool LDMMainMenuState::mouseMoved(const OIS::MouseEvent &evt){
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	if (evt.state.Z.rel) // Scroll wheel.
		sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);

	mRollingMenu->mouseMoved(evt);
	return true;
}
bool LDMMainMenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
	mRollingMenu->mousePressed(evt, id);
	return true;
}
bool LDMMainMenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
	mRollingMenu->mouseReleased(evt, id);
	return true;
}

void LDMMainMenuState::update(double timeSinceLastFrame){
	// Heros Rotate
	double numRoundPerMinutes = 10;
	double rotate = timeSinceLastFrame * (numRoundPerMinutes / 60.0) * 360;

	mHeroSceneNode->yaw(Ogre::Degree(rotate));
	//------------------------------------------------
	// RollingMenu
	mRollingMenu->update(timeSinceLastFrame);
	//------------------------------------------------
}

bool LDMMainMenuState::playButtonPressed(const CEGUI::EventArgs& e){
	changeState(findByName("GameState"));

	return true;
}
bool LDMMainMenuState::optionsButtonPressed(const CEGUI::EventArgs& e){
	pushState(findByName("OptionsMenuState"));

	return true;
}
bool LDMMainMenuState::quitButtonPressed(const CEGUI::EventArgs& e){
	shutdown();

	return true;
}
bool LDMMainMenuState::updateButtonPressed(const CEGUI::EventArgs& e){
	pushState(findByName("UpdaterState"));

	return true;
}
