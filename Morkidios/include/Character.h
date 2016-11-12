#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Weapon.h"
#include "Armour.h"
#include "Framework.h"
#include "Utils.h"

namespace Morkidios {

	class Character {
	public:
		// Public classes
		class Features {
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

		// Construction methodes
		Character();
		virtual ~Character();
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

		// Various methodes
		virtual void setDead(){};
		double attack();
		virtual void subir(double coup);
		void addObject(Object* obj);

		// Transformation methodes
		void setPosition(Ogre::Vector3 newPos);

		// Static methodes
		static void combat(Character* attaquant, Character* victime);
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

		// Ogre
		Ogre::Entity* mEntity;
		Ogre::SceneNode* mSceneNode;
		Ogre::SceneManager* mSceneManager;

		// Collision
		btPairCachingGhostObject* mGhostObject;
		btKinematicCharacterController* mCharacter;
		btDynamicsWorld* mWorld;

		// Mesh variables
		std::map<std::string, std::string> mBoneName;
	};

}

#endif // PERSONNAGE_H
