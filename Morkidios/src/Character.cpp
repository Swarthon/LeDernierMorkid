#include "Character.h"

namespace Morkidios {

	// Construction methodes
	Character::Character(){
		mLeftHandObject = NULL;
		mRightHandObject = NULL;
		mHelmet = NULL;
		mBreastplate = NULL;
		mGreaves = NULL;
		mShoes = NULL;

		mEntity = NULL;
		mSceneNode = NULL;
		mSceneManager = NULL;

		mGhostObject = NULL;
		mCharacter = NULL;

		mRunning = false;
	}
	Character::~Character(){
		for(int i = 0; i < mObjects.size(); i++)
			if(mObjects[i])
				delete mObjects[i];
		if(mHelmet)
			delete mHelmet;
		if(mBreastplate)
			delete mBreastplate;
		if(mGreaves)
			delete mGreaves;
		if(mShoes)
			delete mShoes;

		if(mSceneNode){
			Utils::destroyAllAttachedMovableObjects(mSceneNode);
			mSceneManager->getRootSceneNode()->removeChild(mSceneNode);
		}

	}
	void Character::initGraphics(std::string name, Ogre::SceneManager* smgr){
		if(name == std::string()){
			std::cerr << "Invalid name for initialization of Character in initGraphics()\n";
			exit(1);
		}
		if(!smgr){
			std::cerr << "Invalid SceneManager for initialization of Character in initGraphics()\n";
			exit(1);
		}
		mSceneManager = smgr;
		mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(name);
	}
	void Character::initGraphics(std::string name, std::string path, Ogre::SceneManager* smgr){
		if(name == std::string()){
			std::cerr << "Invalid name for initialization of Character in initGraphics()\n";
			exit(1);
		}
		if(path == std::string()){
			std::cerr << "Invalid path for initialization of Character in initGraphics()\n";
			exit(1);
		}
		if(!smgr){
			std::cerr << "Invalid SceneManager for initialization of Character in initGraphics()\n";
			exit(1);
		}

		mSceneManager = smgr;
		mEntity = mSceneManager->createEntity(name.c_str(), path.c_str());
		mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(name);
		mSceneNode->attachObject(mEntity);
	}
	void Character::initCollisions(btDynamicsWorld* world){
		if(!world){
			std::cerr << "Invalid world for initialization of Character in initCollisions()\n";
			exit(1);
		}

		mGhostObject = new btPairCachingGhostObject();
		world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		if(mEntity){
			OgreBulletCollisions::StaticMeshToShapeConverter* smtc = new OgreBulletCollisions::StaticMeshToShapeConverter(mEntity);
			btConvexShape* shape = (btConvexShape*)smtc->createConvex()->getBulletShape();
			mGhostObject->setCollisionShape(shape);
			mGhostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

			world->addCollisionObject(mGhostObject);
			mCharacter = new btKinematicCharacterController(mGhostObject, shape, 0.75);
		}
		else {
			btCapsuleShape* shape = new btCapsuleShape(0.75,1.80);
			mGhostObject->setCollisionShape(shape);
			mGhostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

			world->addCollisionObject(mGhostObject);
			mCharacter = new btKinematicCharacterController(mGhostObject, shape, 0.75);
		}

		world->addAction(mCharacter);

		mWorld = world;
	}
	void Character::setBoneName(std::string part, std::string bone){
		mBoneName[part] = bone;
	}
	void Character::setArmour(Armour* a, bool searchInReserve){
		if(!a){
			std::cerr << "Invalid arme for initialization of Character in setArmour()\n";
			exit(1);
		}

		if(searchInReserve){
			bool b = false;
			for(int i = 0; i < mObjects.size(); i++)
				if(mObjects[i] == (Object*)a)
					b = true;
			if(!b)
				return;
		}

		// TODO Clothing
		switch(a->getType()){
		case Armour::HELMET:
			mHelmet = a;
			break;
		case Armour::BREASTPLATE:
			mBreastplate = a;
			break;
		case Armour::GREAVES:
			mGreaves = a;
			break;
		case Armour::SHOES:
			mShoes = a;
			break;
		default:
			std::cerr << "Invalid type of Armour " << a->getName() << " for initialization of Character in setArmour()\n";
			exit(1);
			break;
		}
	}

	// Return value methodes
	Character::Features& Character::getActual(){
		return mActual;
	}
	Character::Features& Character::getTotal(){
		return mTotal;
	}
	std::vector<Object*> Character::getObjects(){
		return mObjects;
	}
	Ogre::Vector3 Character::getPosition(){
		return mSceneNode->getPosition();
	}
	Ogre::SceneNode* Character::getSceneNode(){
		return mSceneNode;
	}

	// Various methodes
	double Character::attack(){/*
		std::srand(std::time(NULL));
		double r = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) + 0.5;

		if(mWeapon)
			return (mActual.force + mWeapon->getFeatures().force) * (r + mActual.precision) / 2;
		else
			return mActual.force * (r + mActual.precision) / 2;*/
	}
	void Character::subir(double coup){
		// TODO use algorithm
		if(coup < 0)
			coup = 0;
		mActual.life -= coup;

		if(mActual.life <= 0)
			setDead();
	}
	void Character::addObject(Object* obj){
		std::vector<Object*>::iterator it = std::find(mObjects.begin(), mObjects.end(), obj);
		if(it == mObjects.end())
			mObjects.push_back(obj);
	}

	// Transformation methodes
	void Character::setPosition(Ogre::Vector3 newPos){
		if(!mSceneNode){
			std::cerr << "Invalid scenenode for transformation of Character in setPosition()\n";
			exit(1);
		}
		if(!mCharacter){
			std::cerr << "Invalid character for transformation of Character in setPosition()\n";
			exit(1);
		}
		if(!mGhostObject){
			std::cerr << "Invalid ghostobject for transformation of Character in setPosition()\n";
			exit(1);
		}

		btTransform transform = mGhostObject->getWorldTransform();
		transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(newPos));
		mGhostObject->setWorldTransform(transform);

		mSceneNode->setPosition(newPos);
	}

	// Static methodes
	void combat(Character* attaquant, Character* victime){
		victime->subir(attaquant->attack());
	}

}
