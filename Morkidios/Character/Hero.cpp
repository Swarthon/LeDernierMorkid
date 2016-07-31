#include "Hero.h"

Morkidios::Hero* Morkidios::Hero::mSingleton = NULL;

namespace Morkidios {

	// Construction methodes
	Hero* Hero::getSingleton(){
		if(!mSingleton)
			mSingleton = new Hero;
		return mSingleton;
	}
	void Hero::destroySingleton(){
		delete mSingleton;
		mSingleton = NULL;
	}
	void Hero::initCamera(){
		if(!mSceneNode){
			std::cerr << "SceneNode has not been initialized for Hero in initCamera\nPlease use initGraphics\n";
			exit(1);
		}
		mCamera = mSceneManager->createCamera("HeroCamera");
		mCamera->setNearClipDistance(0.05);
		mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
		mSceneNode->attachObject(mCamera);

		Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
	}
	void Hero::setCameraAsActual(){
		Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
	}
	void Hero::initGraphics(std::string name, Ogre::SceneManager* smgr){
		Character::initGraphics(name, smgr);
		
		mHand = Hand::load("hand.xml", mSceneManager);
		mHand->setParent(mSceneNode);
	}
	void Hero::initGraphics(std::string name, std::string path, Ogre::SceneManager* smgr){
		Character::initGraphics(name, path, smgr);
		
		mHand = Hand::load("hand.xml", mSceneManager);
		mHand->setParent(mSceneNode);
	}
	void Hero::disableCamera(){
		if(!mSceneNode){
			std::cerr << "SceneNode has not been initialized for Hero in disableCamera\nPlease use initGraphics\n";
			exit(1);
		}
		if(!mCamera){
			std::cerr << "Camera has not been initialized for Hero in disableCamera\nPlease use initCamera\n";
			exit(1);
		}
		mSceneNode->detachObject(mCamera);
		mSceneManager->destroyCamera(mCamera);
	}

	// Various methodes
	void Hero::update(double timeSinceLastFrame){
		// Synchronize bullet and ogre
		synchronize();

		if(mHand)
			mHand->addTime(timeSinceLastFrame);
	}
	void Hero::move(int d){
		if(!mCharacter) {
			std::cerr << "Invalid character for Hero in move()\n";
			exit(1);
		}
		if(!mGhostObject) {
			std::cerr << "Invalid ghost object for Hero in move()\n";
			exit(1);
		}

		// Deplacement
		btTransform ghostTransform = mGhostObject->getWorldTransform();
		btVector3 forward = ghostTransform.getBasis()[2];
		forward[0] *= -1;
		forward.normalize();
	
		btVector3 direction = btVector3(0,0,0);
		if((d & FORWARD) != 0)
			direction -= forward;
		if((d & BACKWARD) != 0)
			direction += forward;
		if((d & LEFT) != 0){
			btTransform moveTransform = ghostTransform;
			moveTransform.setRotation(moveTransform.getRotation() * btQuaternion(btVector3(0,1,0), Ogre::Degree(90).valueRadians()));
			btVector3 moveForward = moveTransform.getBasis()[2];
			moveForward[0] *= -1;
			direction -= moveForward;
		}
		if((d & RIGHT) != 0){
			btTransform moveTransform = ghostTransform;
			moveTransform.setRotation(moveTransform.getRotation() * btQuaternion(btVector3(0,1,0), Ogre::Degree(90).valueRadians()));
			btVector3 moveForward = moveTransform.getBasis()[2];
			moveForward[0] *= -1;
			direction += moveForward;
		}
	
		if((d & UP) != 0)
			mCharacter->jump();

		mCharacter->setWalkDirection(direction*mActual.speed*SPEED);
		// ---------

		if(direction == btVector3(0,0,0))
			mHand->setAnimation("Walk", false);
		else 
			mHand->setAnimation("Walk", true);
	}
	void Hero::rotate(const OIS::MouseEvent &evt){
		const OIS::MouseState &mouseState = evt.state;
		Ogre::Radian rotationX = Ogre::Degree(-mouseState.Y.rel * gMouseSensibility);
		Ogre::Radian rotationY = Ogre::Degree(-mouseState.X.rel * gMouseSensibility);

		mTotalRotationX += rotationX;
		mTotalRotationY += rotationY;

		if(mTotalRotationX > Ogre::Degree(90))
			mTotalRotationX = Ogre::Degree(90);
		if(mTotalRotationX < -Ogre::Degree(90))
			mTotalRotationX = -Ogre::Degree(90);
	}
	void Hero::subir(double coup){
		Character::subir(coup);
	}
	void Hero::synchronize(){
		if(mSceneNode && mGhostObject){
			Ogre::Quaternion oq = Ogre::Quaternion::IDENTITY;
			oq = oq * Ogre::Quaternion(mTotalRotationY, Ogre::Vector3(0,1,0));
			oq = oq * Ogre::Quaternion(mTotalRotationX, Ogre::Vector3(1,0,0));

			mSceneNode->setOrientation(oq);
			btQuaternion bq = btQuaternion::getIdentity();
			bq = bq * btQuaternion(btVector3(0,1,0), mTotalRotationY.valueRadians());
			btTransform transform = mGhostObject->getWorldTransform();
			transform.setRotation(bq);
			mGhostObject->setWorldTransform(transform);

			mSceneNode->setPosition(OgreBulletCollisions::BtOgreConverter::to(mGhostObject->getWorldTransform().getOrigin() + btVector3(0,Utils::getBoundingBox(mGhostObject->getCollisionShape()).y()/2,0)));
		}
	}
	void Hero::unequipeRightHand(){
		mHand->setRightHandObject(NULL);
		mRightHandObject = NULL;
	}
	void Hero::equipeRightHand(Object* obj){
		if(mRightHandObject)
			unequipeRightHand();
		mHand->setRightHandObject(obj->getEntity());
		mRightHandObject = obj;
	}
	void Hero::unequipeLeftHand(){
		mHand->setLeftHandObject(NULL);
		mLeftHandObject = NULL;
	}
	void Hero::equipeLeftHand(Object* obj){
		if(mLeftHandObject)
			unequipeLeftHand();
		mHand->setLeftHandObject(obj->getEntity());
		mLeftHandObject = obj;
	}
	void Hero::drop(Object* o){
		o->drop(mSceneNode->getPosition() + Ogre::Vector3(0,0,5) * (mSceneNode->getOrientation()*Ogre::Vector3::NEGATIVE_UNIT_Z));
		std::vector<Object*>::iterator it = std::find(mObjects.begin(), mObjects.end(), o);
		if(it != mObjects.end())
			mObjects.erase(it);
	}
	void Hero::dropLeftHandObject(){
		Object* o = mLeftHandObject;
		unequipeLeftHand();
		drop(o);
	}
	void Hero::dropRightHandObject(){
		Object* o = mRightHandObject;
		unequipeRightHand();	
		drop(o);
	}
	Object* Hero::get(){
		btVector3 start = OgreBulletCollisions::OgreBtConverter::to(mSceneNode->getPosition());
		btVector3 end = OgreBulletCollisions::OgreBtConverter::to(mSceneNode->getPosition() + (mSceneNode->getOrientation()*Ogre::Vector3::NEGATIVE_UNIT_Z).normalisedCopy() * 10);
		btCollisionWorld::ClosestRayResultCallback rayTest = btCollisionWorld::ClosestRayResultCallback(start, end);
		mWorld->rayTest(start, end, rayTest);
		if(rayTest.hasHit()){
			if(rayTest.m_collisionObject->getUserPointer()){
				Object* o = (Object*)rayTest.m_collisionObject->getUserPointer();
				o->get();
				mObjects.push_back(o);
				return o;
			}
		}

		return NULL;
	}

	// Construction methodes
	Hero::Statistiques::Statistiques(){
		mNumOfKills = 0;
		mNumCoupsRecus = 0;
		mNumCoupsDonnes = 0;
		mNumViesPerdues = 0;
		mNumDegatsDonnes = 0;
	}
	Hero::Statistiques::~Statistiques(){
	}
	void Hero::Statistiques::addKills(int i){
		mNumOfKills += i;
	}
	void Hero::Statistiques::addCoupsRecus(int i){
		mNumCoupsRecus += i;
	}
	void Hero::Statistiques::addCoupsDonnes(int i){
		mNumCoupsDonnes += i;
	}
	void Hero::Statistiques::addViesPerdus(double i){
		mNumViesPerdues += i;
	}
	void Hero::Statistiques::addDegatsDonnes(double i){
		mNumDegatsDonnes += i;
	}

	// Return value methodes
	int Hero::Statistiques::getKills(){
		return mNumOfKills;
	}
	int Hero::Statistiques::getCoupsRecus(){
		return mNumCoupsRecus;
	}
	int Hero::Statistiques::getCoupsDonnes(){
		return mNumCoupsDonnes;
	}
	double Hero::Statistiques::getViesPerdues(){
		return mNumViesPerdues;
	}
	double Hero::Statistiques::getDegatsDonnes(){
		return mNumDegatsDonnes;
	}
	Ogre::Camera* Hero::getCamera(){
		return mCamera;
	}
	Object* Hero::getLeftHandObject(){
		return mLeftHandObject;
	}
	Object* Hero::getRightHandObject(){
		return mRightHandObject;
	}

	// Singleton
	Hero::Hero(){
		mLeftHandObject = NULL;
		mRightHandObject = NULL;
		mHelmet = NULL;
		mBreastplate = NULL;
		mGreaves = NULL;
		mShoes = NULL;

		mEntity = NULL;
		mSceneNode = NULL;

		mGhostObject = NULL;
		mCharacter = NULL;

		mCamera = NULL;

		mHand = NULL;
	}
	Hero::~Hero(){
	}

	// Private methodes
	void Hero::holdRightHand(Object* obj){
		if(!mHand){
			return;
		}

		std::vector<Object*>::iterator it = std::find(mObjects.begin(), mObjects.end(), obj);
		if(it != mObjects.end()){
			mHand->setRightHandObject(obj->getEntity());
			mRightHandObject = obj;
		}
		else
			std::cout << "WARNING : No object " << obj->getName() << " owned by Hero\n";
	}
	void Hero::holdLeftHand(Object* obj){
		if(!mHand){
			return;
		}

		std::vector<Object*>::iterator it = std::find(mObjects.begin(), mObjects.end(), obj);
		if(it != mObjects.end()){
			mHand->setLeftHandObject(obj->getEntity());
			mLeftHandObject = obj;
		}
		else
			std::cout << "WARNING : No object " << obj->getName() << " owned by Hero\n";
	}
}