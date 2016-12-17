#include "Terrain.h"

Morkidios::Terrain* _MorkidiosExport Morkidios::Terrain::mActiveTerrain = NULL;

namespace Morkidios {

	// Construction methodes
	Terrain::Terrain(){
		// Ogre
		mGeometry = NULL;

		// Bullet
		mWorld = NULL;
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
		mWorld->stepSimulation (time, 1, btScalar(1.)/btScalar(60.));

		// AI

		// Objects
		for (size_t i = 0; i < mDroppedObjects.size(); i++) {
			mDroppedObjects[i]->update(time);
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

	// Static methodes
	Terrain* Terrain::getActiveTerrain(){
		return mActiveTerrain;
	}
	void Terrain::setActiveTerrain(Terrain* t){
		mActiveTerrain = t;
	}
}
