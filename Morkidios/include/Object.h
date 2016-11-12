#ifndef OBJECT_H
#define OBJECT_H

#include <MorkidiosPrerequisites.h>

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
		virtual void load(Ogre::SceneManager* smgr, btDynamicsWorld* world, std::string name, std::string path);
		virtual void setType(Type t);

		// Return value methodes
		virtual std::string getName();
		virtual Ogre::Entity* getEntity();
		virtual Ogre::SceneNode* getSceneNode();
		virtual Type getType();
		virtual btRigidBody* getRigidBody();

		// Various methodes
		virtual void addToWorld(bool b);
		virtual void drop(Ogre::Vector3 pos);
		virtual void get();
		virtual void equipe();				// This is just a callback when equiped
		virtual void unequipe();			// This is just a callback when unequiped
		virtual void show(bool b);
		virtual void update(double timeSinceLastFrame);
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

		// Private methodes
		void attachEntityToSceneNode();
		void setPosition(Ogre::Vector3 pos);
	};

}

#endif // OBJECT_H
