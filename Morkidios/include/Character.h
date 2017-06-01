#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Weapon.h"
#include "Armour.h"
#include "Framework.h"
#include "Utils.h"

namespace Morkidios {

	class _MorkidiosExport Character {
	public:
		// Public classes
		class _MorkidiosExport Features {
		public:
			Features() : life(0), intelligence(0), agility(0), force(0), precision(0), speed(0), range(0){}

			double life;
			double intelligence;
			double agility;
			double force;
			double precision;
			double speed;
			double range;
		};
		enum Side {
			Left,
			Right,
		};

		// Construction methodes
		Character();
		virtual ~Character() = 0;
		virtual void initGraphics(std::string name, Ogre::SceneManager* smgr);
		virtual void initGraphics(std::string name, std::string path, Ogre::SceneManager* smgr);
		virtual void initCollisions(btDynamicsWorld* world);
		virtual void setBoneName(std::string part, std::string bone);
		virtual void setArmour(Armour* a, bool searchInReserve = false);

		// Return value methodes
		Features& getActual();
		Features& getTotal();
		std::vector<Object*> getObjects();
		Ogre::Vector3 getPosition();
		Ogre::SceneNode* getSceneNode();
		Ogre::Entity* getEntity();
		btPairCachingGhostObject* getGhostObject();
		virtual bool isDead();

		// Various methodes
		virtual void setDead() = 0;
		virtual double getLeftHandDammage();
		virtual double getRightHandDammage();
		virtual void suffer(double coup);
		virtual void addObject(Object* obj);
		virtual void synchronize();

		// Transformation methodes
		void setPosition(Ogre::Vector3 newPos);
	protected:
		Object* mLeftHandObject;
		Object* mRightHandObject;
		Armour* mHelmet;
		Armour* mBreastplate;
		Armour* mGreaves;
		Armour* mShoes;

		std::vector<Object*> mObjects;

		Features mActual;
		Features mTotal;

		bool mRunning;
		bool mDead;

		// Ogre
		Ogre::Entity* mEntity;
		Ogre::SceneNode* mSceneNode;
		Ogre::SceneManager* mSceneManager;

		// Collision
		btPairCachingGhostObject* mGhostObject;
		btKinematicCharacterController* mCharacter;
		btDynamicsWorld* mWorld;
		double mStepHeight;

		// Mesh variables
		std::map<std::string, std::string> mBoneName;
	};

}

#endif // PERSONNAGE_H
