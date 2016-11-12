#include "GameState.h"

GameState::GameState(){
	mFrameEvent = Ogre::FrameEvent();
	mFPSVisible = false;
}
GameState::~GameState(){
}

void GameState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager("OctreeSceneManager");
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));

	createScene();
	createGUI();

	Morkidios::GraphicOptions::getSingleton()->config();
}
void GameState::createScene(){
	mTerrain = new LDMMaze;
	mTerrain->init("Maze", mSceneManager);
	Morkidios::Terrain::setActiveTerrain(mTerrain);

	Morkidios::Hero::getSingleton()->initGraphics("Hero", mSceneManager);
	Morkidios::Hero::getSingleton()->initCamera();
	Morkidios::Hero::getSingleton()->initCollisions(mTerrain->getWorld());
	Morkidios::Character::Features c;
	c.life = 10;
	c.intelligence = 1;
	c.agility = 1;
	c.force = 1;
	c.precision = 1;
	c.speed = 7.5;
	c.range = 10;
	Morkidios::Hero::getSingleton()->getTotal() = c;
	Morkidios::Hero::getSingleton()->getActual() = c;

	Morkidios::Hero::getSingleton()->addObject(Morkidios::Weapon::getLoadedWeaponsClass()[0].second(mSceneManager, mTerrain->getWorld()));
	Morkidios::Hero::getSingleton()->addObject(Morkidios::Weapon::getLoadedWeaponsClass()[1].second(mSceneManager, mTerrain->getWorld()));
	Morkidios::Hero::getSingleton()->addObject(Morkidios::Weapon::getLoadedWeaponsClass()[1].second(mSceneManager, mTerrain->getWorld()));
}
void GameState::createGUI(){
	mMap = new Morkidios::Map;
	mMap->init("Map.bmp", Ogre::Vector2(mTerrain->getSize().x, mTerrain->getSize().z));
	mMap->addPoint(Ogre::Vector2::ZERO, "Hero", "HeroMap.png");

	mCrossHair = new Morkidios::CrossHair;
	mCrossHair->init("CrossHair.png");
	Morkidios::GraphicOptions::getSingleton()->setCrossHair(mCrossHair);

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	// Debug
	mFPSWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("Generic/Label","FPSWindow");
	mFPSWindow->setVisible(false);
}
void GameState::exit(){
	Morkidios::Hero::destroySingleton();
	delete mTerrain;
	delete mMap;
	delete mCrossHair;
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild(mFPSWindow);
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
}
void GameState::resume(){
	Morkidios::Hero::getSingleton()->setCameraAsActual();
	mMap->show(true);
	mCrossHair->show(true);
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	mFPSWindow->setVisible(mFPSVisible);
}
bool GameState::pause(){
	mMap->show(false);
	mCrossHair->show(false);

	mFPSWindow->setVisible(false);

	return true;
}

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef){
	return true;
}
bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef){
	if(keyEventRef.key == OIS::KC_ESCAPE)
	        pushState(findByName("PauseMenuState"));
	if(keyEventRef.key == Morkidios::Input::getSingleton()->mKeyMap["Inventory"])
	        pushState(findByName("InventoryState"));

	if(keyEventRef.key == Morkidios::Input::getSingleton()->mKeyMap["Drop Left Hand"]){
		Morkidios::Object* o = Morkidios::Hero::getSingleton()->getLeftHandObject();
		if(o){
			Morkidios::Hero::getSingleton()->dropLeftHandObject();
			mTerrain->addDroppedObject(o);
		}
	}
	if(keyEventRef.key == Morkidios::Input::getSingleton()->mKeyMap["Drop Right Hand"]){
		Morkidios::Object* o = Morkidios::Hero::getSingleton()->getRightHandObject();
		if(o){
			Morkidios::Hero::getSingleton()->dropRightHandObject();
			mTerrain->addDroppedObject(o);
		}
	}
	if(keyEventRef.key == Morkidios::Input::getSingleton()->mKeyMap["Take"]){
		Morkidios::Object* o = Morkidios::Hero::getSingleton()->get();
		if(o)
			mTerrain->removeDroppedObject(o);
	}

	if(keyEventRef.key == Morkidios::Input::getSingleton()->mKeyMap["Screenshot"]){
		Morkidios::Framework::getSingletonPtr()->mRenderWindow->writeContentsToTimestampedFile("Screenshot_", ".jpg");
		return true;
	}

	// Debug
	if(keyEventRef.key == Morkidios::Input::getSingleton()->mKeyMap["Show FPS"]){
		mFPSWindow->setVisible(mFPSVisible = !mFPSVisible);
		return true;
	}

	return true;
}

bool GameState::mouseMoved(const OIS::MouseEvent &evt){
	heroRotate(evt);
	return true;
}
bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	return true;
}
bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	return true;
}

void GameState::update(double timeSinceLastFrame){
	Morkidios::Hero::getSingleton()->update(timeSinceLastFrame);

	mTerrain->update(timeSinceLastFrame);

	// Input
	heroMove();

	// Debug
	if(mFPSVisible)
		mFPSWindow->setText(std::string("FPS : ") + Morkidios::Utils::convertIntToString(Morkidios::Framework::getSingletonPtr()->mRenderWindow->getAverageFPS()));
}

void GameState::heroMove(){
	int d = 0;
	if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(Morkidios::Input::getSingleton()->mKeyMap["Forward"]))
		d |= FORWARD;
	if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(Morkidios::Input::getSingleton()->mKeyMap["Backward"]))
		d |= BACKWARD;
	if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(Morkidios::Input::getSingleton()->mKeyMap["Leftward"]))
		d |= LEFT;
	if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(Morkidios::Input::getSingleton()->mKeyMap["Rightward"]))
		d |= RIGHT;
	if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(Morkidios::Input::getSingleton()->mKeyMap["Jump"]))
		d |= UP;
	if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(Morkidios::Input::getSingleton()->mKeyMap["Run"]))
		d |= RUN;

	if(d != 0){
		Ogre::Vector3 pos = Morkidios::Hero::getSingleton()->getPosition();
		mMap->updatePoint(Ogre::Vector2(pos.x,pos.z),"Hero");
	}

	Morkidios::Hero::getSingleton()->move(d);
}
void GameState::heroRotate(const OIS::MouseEvent &evt){
	Morkidios::Hero::getSingleton()->rotate(evt);
}
