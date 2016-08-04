#include "LDMGameState.h"

LDMGameState::LDMGameState(){
	mFrameEvent = Ogre::FrameEvent();
	mFPSVisible = false;
}
LDMGameState::~LDMGameState(){
}

void LDMGameState::enter(){
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager("OctreeSceneManager");
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	createScene();
	createGUI();
}
void LDMGameState::createScene(){	
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
	c.speed = 15;
	c.range = 10;

	Morkidios::Object* o = new Morkidios::Object();
	o->load(mSceneManager, mTerrain->getWorld(), "Epee","Sword.mesh");
	o->setType(Morkidios::Object::Weapon);
	Morkidios::Hero::getSingleton()->addObject(o);

	o = new Morkidios::Object();
	o->load(mSceneManager, mTerrain->getWorld(), "Sinbad","Sinbad.mesh");
	o->setType(Morkidios::Object::Weapon);
	Morkidios::Hero::getSingleton()->addObject(o);

	Morkidios::Hero::getSingleton()->getTotal() = c;
	Morkidios::Hero::getSingleton()->getActual() = c;
}
void LDMGameState::createGUI(){
	mMap = new Morkidios::Map;
	mMap->init("Map.bmp", Ogre::Vector2(mTerrain->getSize().x, mTerrain->getSize().z));
	mMap->addPoint(Ogre::Vector2::ZERO, "Hero", "HeroMap.png");

	mCrossHair = new Morkidios::CrossHair;
	mCrossHair->init("CrossHair.png");
	
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	// Debug
	mFPSWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("Generic/Label","FPSWindow");
	mFPSWindow->setVisible(false);
}
void LDMGameState::exit(){
	Morkidios::Hero::destroySingleton();
	delete mTerrain;
	delete mMap;
	delete mCrossHair;
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild(mFPSWindow);
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
}
void LDMGameState::resume(){
	Morkidios::Hero::getSingleton()->setCameraAsActual();
	mMap->show(true);
	mCrossHair->show(true);
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	mFPSWindow->setVisible(mFPSVisible);
}
bool LDMGameState::pause(){
	mMap->show(false);
	mCrossHair->show(false);

	mFPSWindow->setVisible(false);

	return true;
}

bool LDMGameState::keyPressed(const OIS::KeyEvent &keyEventRef){
	return true;
}
bool LDMGameState::keyReleased(const OIS::KeyEvent &keyEventRef){
	if(keyEventRef.key == OIS::KC_ESCAPE)
	        pushState(findByName("PauseMenuState"));
	if(keyEventRef.key == OIS::KC_E)
	        pushState(findByName("InventoryState"));

	if(keyEventRef.key == OIS::KC_U){
		Morkidios::Object* o = Morkidios::Hero::getSingleton()->getLeftHandObject();
		if(o){
			Morkidios::Hero::getSingleton()->dropLeftHandObject();
			mTerrain->addDroppedObject(o);
		}
	}
	if(keyEventRef.key == OIS::KC_I){
		Morkidios::Object* o = Morkidios::Hero::getSingleton()->getRightHandObject();
		if(o){
			Morkidios::Hero::getSingleton()->dropRightHandObject();
			mTerrain->addDroppedObject(o);
		}
	}
	if(keyEventRef.key == OIS::KC_F){
		Morkidios::Object* o = Morkidios::Hero::getSingleton()->get();
		if(o){
			mTerrain->removeDroppedObject(o);
		}
	}

	if(keyEventRef.key == OIS::KC_SYSRQ){
		Morkidios::Framework::getSingletonPtr()->mRenderWindow->writeContentsToTimestampedFile("Screenshot_", ".jpg");
		return true;
	}

	// Debug
	if(keyEventRef.key == OIS::KC_F3){
		mFPSWindow->setVisible(mFPSVisible = !mFPSVisible);
		return true;
	}

	return true;
}

bool LDMGameState::mouseMoved(const OIS::MouseEvent &evt){
	heroRotate(evt);
	heroMove();
	return true;
}
bool LDMGameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	return true;
}
bool LDMGameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	return true;
}

void LDMGameState::update(double timeSinceLastFrame){
	Morkidios::Hero::getSingleton()->update(timeSinceLastFrame);
	mTerrain->update(timeSinceLastFrame);

	Ogre::Vector3 pos = Morkidios::Hero::getSingleton()->getPosition();
	mMap->updatePoint(Ogre::Vector2(pos.x,pos.z),"Hero");

	// Input
	heroMove();

	// Debug
	if(mFPSVisible)
		mFPSWindow->setText(std::string("FPS : ") + Morkidios::Utils::convertIntToString(Morkidios::Framework::getSingletonPtr()->mRenderWindow->getAverageFPS()));

}

void LDMGameState::heroMove(){
	int d = 0;
	if(Morkidios::Framework::getSingletonPtr()->mKeyboard->isKeyDown(Morkidios::gForwardMoveKey))
		d |= FORWARD;
	if(Morkidios::Framework::getSingletonPtr()->mKeyboard->isKeyDown(Morkidios::gBackwardMoveKey))
		d |= BACKWARD;
	if(Morkidios::Framework::getSingletonPtr()->mKeyboard->isKeyDown(Morkidios::gLeftMoveKey))
		d |= LEFT;
	if(Morkidios::Framework::getSingletonPtr()->mKeyboard->isKeyDown(Morkidios::gRightMoveKey))
		d |= RIGHT;
	if(Morkidios::Framework::getSingletonPtr()->mKeyboard->isKeyDown(Morkidios::gJumpKey))
		d |= UP;

	Morkidios::Hero::getSingleton()->move(d);
}
void LDMGameState::heroRotate(const OIS::MouseEvent &evt){
	Morkidios::Hero::getSingleton()->rotate(evt);
}