#include "Terrain.h"

Morkidios::Terrain* _MorkidiosExport Morkidios::Terrain::mActiveTerrain = NULL;

namespace Morkidios {

	// Construction methodes
	Terrain::Terrain(){
		// Ogre
		mGeometry = NULL;

		// Bullet
		mWorld = NULL;
		mDispatcher = NULL;
		mBroadphase = NULL;
		mSolver = NULL;

		// AI
		mAIForEach = 0;
		mNavigationAlgorithm = new DefaultNavigationAlgorithm(this);
	}
	Terrain::~Terrain(){
		for(int i = 0; i < mAI.size(); i++)
			if(mAI[i])
				delete mAI[i];

		mSceneManager->destroyStaticGeometry(mGeometry);

		// Bullet
 		for (int i=mWorld->getNumCollisionObjects()-1; i>=0 ;i--){
			btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
			mWorld->removeCollisionObject( obj );
			delete obj;
		}

		delete mWorld;
	}
	void Terrain::init(Ogre::SceneManager* smgr, double worldSize, double height, std::string name){
		if(!smgr){
			std::cerr << "Invalid SceneManager for initialization of Terrain in init()\n";
			exit(1);
		}
		mSceneManager = smgr;
		mName = name;
		if(name == std::string()){
			static int i = 0;
			std::ostringstream os;
			os << i;

			mName = std::string("Terrain") + os.str();
			i++;
		}

		mGeometry = mSceneManager->createStaticGeometry(mName.c_str());
		mSize = Ogre::Vector3(worldSize, height, worldSize);

		// Bullet
		mCollisionConfiguration = new btDefaultCollisionConfiguration();
		mDispatcher = new btCollisionDispatcher( mCollisionConfiguration );
		mBroadphase = new btDbvtBroadphase();
		mSolver = new btSequentialImpulseConstraintSolver();
		mWorld = new btDiscreteDynamicsWorld( mDispatcher, mBroadphase, mSolver, mCollisionConfiguration );
		mWorld->setGravity( btVector3(0.0,-9.8,0.0));
	}
	void Terrain::addAI(AI* ai){
		mAI.push_back(ai);
	}

	// Return value methodes
	btDynamicsWorld* Terrain::getWorld() {
		return mWorld;
	}
	Ogre::Vector3 Terrain::getSize(){
		return mSize;
	}

	// Various methodes
	void Terrain::update(double time){
		// Bullet
		mWorld->stepSimulation (time, 1, btScalar(1.)/btScalar(100.));

		// Objects
		for (size_t i = 0; i < mDroppedObjects.size(); i++)
			mDroppedObjects[i]->update(time);

		// AI
		for (size_t i = 0; i < mAI.size(); i++){
			if(mAI[i]->isDead())
				continue;
			AnimationInformations infos;
			infos.timeSinceLastFrame = time;
			mAI[i]->animation(&infos);
		}
		// TODO Threads
		for(int i = 0; i < mAI.size(); i++){
			if(mAI[i]->isDead())
				continue;
			mAI[i]->navigation(mNavigationAlgorithm);
		}
	}
	void Terrain::addDroppedObject(Object* obj){
		if(obj)
			mDroppedObjects.push_back(obj);
	}
	void Terrain::removeDroppedObject(Object* obj){
		if(obj)
			mDroppedObjects.erase(std::find(mDroppedObjects.begin(),mDroppedObjects.end(),obj));
	}
	void Terrain::heroAskForAttack(Character::Side s){
		Hero* h = Hero::getSingleton();
		double range = h->getActual().range;
		btVector3 start = OgreBulletCollisions::OgreBtConverter::to(h->getSceneNode()->getPosition());
		btVector3 end = OgreBulletCollisions::OgreBtConverter::to(h->getSceneNode()->getPosition() + (h->getSceneNode()->getOrientation()*Ogre::Vector3::NEGATIVE_UNIT_Z).normalisedCopy() * h->getActual().range);
		Utils::ClosestNotMeRayResultCallback rayCallback(h->getGhostObject());
		mWorld->rayTest(start, end, rayCallback);
		if(rayCallback.hasHit()){
			for(size_t i = 0; i < mAI.size(); i++){
				if(mAI[i]->getGhostObject() == rayCallback.m_collisionObject){
					if(s == Character::Left)
						mAI[i]->suffer(h->getLeftHandDammage());
					else
						mAI[i]->suffer(h->getRightHandDammage());
				}
			}
		}
	}

	// Static methodes
	Terrain* Terrain::getActiveTerrain(){
		return mActiveTerrain;
	}
	void Terrain::setActiveTerrain(Terrain* t){
		mActiveTerrain = t;
	}
}
