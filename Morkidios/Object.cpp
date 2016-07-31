#include "Object.h"

namespace Morkidios {
	
	// Construction methodes
	Object::Object(){
		mName = std::string();

		mEntity = NULL;
		mSceneNode = NULL;

		mType = Default;
		mState = Hidden;

		mActiveCollision = false;
	}
	Object::~Object(){
		if(mEntity){
			mEntity->detachFromParent();
			mEntity->_getManager()->destroyEntity(mEntity);
		}
		if(mSceneNode){
			Utils::destroyAllAttachedMovableObjects(mSceneNode);			
			mSceneNode->getParent()->removeChild(mSceneNode);
		}
	}
	void Object::load(Ogre::SceneManager* smgr, btDynamicsWorld* world, std::string name, std::string path){
		if(!smgr){
			std::cerr << "Invalid scene manager for initialization of Object in load()";
			exit(1);
		}
		if(!world){
			std::cerr << "Invalid world for initialization of Object in load()";
			exit(1);
		}
		if(name == std::string()){
			std::cerr << "Invalid name for initialization of Object in load()";
			exit(1);
		}
		if(path == std::string()){
			std::cerr << "Invalid path for initialization of Object in load()";
			exit(1);
		}

		mEntity = smgr->createEntity(name, path);
		mEntity->setVisible(false);
		mSceneNode = smgr->getRootSceneNode()->createChildSceneNode(name);
		mName = name;
		mWorld = world;

		btCollisionShape* shape = OgreBulletCollisions::StaticMeshToShapeConverter(mEntity).createConvexDecomposition()->getBulletShape();	
		
		btVector3 localInertia(0,0,0);
		btScalar mass = 1.f;
		if(mass)
			shape->calculateLocalInertia(mass,localInertia);

		btTransform t;
		t.setIdentity();
		btMotionState* motionState = new ObjectMotionState(t, mSceneNode);
		mBody = new btRigidBody(1, motionState, shape, localInertia);
		mBody->setRestitution(0);
		mBody->setFriction(1);
		mBody->setUserPointer(this);
	}
	void Object::setType(Type t){
		mType = t;
	}

	// Return value methodes	
	std::string Object::getName(){
		return mName;
	}
	Ogre::Entity* Object::getEntity(){
		return mEntity;
	}
	Object::Type Object::getType(){
		return mType;
	}
	btRigidBody* Object::getRigidBody(){
		return mBody;
	}

	void Object::addToWorld(bool b){
		if(mActiveCollision != b){
			if(b){
				mWorld->addRigidBody(mBody);
				setSceneNodeAsEntityParent();
			}
			if(!b){
				mWorld->removeRigidBody(mBody);
				setSceneNodeAsEntityParent();
			}
		}
		mActiveCollision = b;
	}
	void Object::setPosition(Ogre::Vector3 pos){
		if(mSceneNode && mBody && mActiveCollision){
			mSceneNode->setPosition(pos);
			mBody->getWorldTransform().setOrigin(OgreBulletCollisions::OgreBtConverter::to(pos));
		}
	}
	void Object::setSceneNodeAsEntityParent(){
		if(mEntity->getParentSceneNode() != mSceneNode || mEntity->getParentSceneNode()){
			mEntity->detachFromParent();
			mSceneNode->attachObject(mEntity);
		}
	}
	void Object::drop(Ogre::Vector3 pos){
		addToWorld(true);
		setPosition(pos);
		mEntity->setVisible(true);
	}
	void Object::get(){
		addToWorld(false);
		mEntity->setVisible(false);
	}
}