#ifndef OBJECT_H
#define OBJECT_H

// Ogre includes
#include <OGRE/Ogre.h>

// Bullet includes
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

// OgreBullet includes
#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsCapsuleShape.h>
#include <Shapes/OgreBulletCollisionsConvexHullShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsGImpactShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>

#include <iostream>

#include "Utils.h"
#include "MotionStates/ObjectMotionState.h"

namespace Morkidios {

	class Object {
	public:
		// Enums
		enum Type {
			Weapon = 0,
			Armour,
			Quest,
			Various,
			Eatable,
			Utils,
			Default,
		};
		enum State {
			Held,
			Dropped,
			Hidden,
		};

		// Construction methodes
		Object();
		virtual ~Object();
		void load(Ogre::SceneManager* smgr, btDynamicsWorld* world, std::string name, std::string path);
		void setType(Type t);

		// Return value methodes
		std::string getName();
		Ogre::Entity* getEntity();
		Ogre::SceneNode* getSceneNode();
		Type getType();
		btRigidBody* getRigidBody();

		// Méthdes diverses
		void addToWorld(bool b);
		void setPosition(Ogre::Vector3 pos);
		void drop(Ogre::Vector3 pos);
		void get();
		void show(bool b);
	protected:
		std::string mName;
		Type mType;
		State mState;
		bool mActiveCollision;

		// Ogre
		Ogre::Entity* mEntity;
		Ogre::SceneNode* mSceneNode;

		// Bullet
		btRigidBody* mBody;
		btDynamicsWorld* mWorld;
		Ogre::SceneManager* mSceneManager;
		
		void attachEntityToSceneNode();
	};

}

#endif // OBJECT_H
