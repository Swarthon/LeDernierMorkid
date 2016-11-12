#ifndef HEROS_H
#define HEROS_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Weapon.h"
#include "Armour.h"
#include "Framework.h"
#include "Character.h"
#include "Hand.h"
#define SPEED 0.025

namespace Morkidios {

	class Hero : public Character {
	public:
		// Classe public
		class Statistiques {
		public:
			// Construction methodes
			Statistiques();
			~Statistiques();
			void addKills(int i);
			void addCoupsRecus(int i);
			void addCoupsDonnes(int i);
			void addViesPerdus(double i);
			void addDegatsDonnes(double i);

			// Return value methodes
			int getKills();
			int getCoupsRecus();
			int getCoupsDonnes();
			double getViesPerdues();
			double getDegatsDonnes();
		private:
			int mNumOfKills;
			int mNumCoupsRecus;
			int mNumCoupsDonnes;
			double mNumViesPerdues;
			double mNumDegatsDonnes;
		};

		// Construction methodes
		static Hero* getSingleton();
		static void destroySingleton();
		void initCamera();
		void setCameraAsActual();
		void initGraphics(std::string name, Ogre::SceneManager* smgr);
		void initGraphics(std::string name, std::string path, Ogre::SceneManager* smgr);
		void disableCamera();

		// Various methodes
		void move(int d);
		void rotate(const OIS::MouseEvent &evt);
		void update(double timeSinceLastFrame);
		void synchronize();
		void subir(double coup);
		void unequipeRightHand();
		void equipeRightHand(Object* obj);
		void unequipeLeftHand();
		void equipeLeftHand(Object* obj);
		void drop(Object* o);
		void dropLeftHandObject();
		void dropRightHandObject();
		Object* get();

		// Return value methodes
		Ogre::Camera* getCamera();
		Object* getLeftHandObject();
		Object* getRightHandObject();
	private:
		// Singleton
		Hero();
		virtual ~Hero();
		static Hero* mSingleton;

		// Ogre
		Ogre::Camera* mCamera;

		// Variables de rotation
		Ogre::Radian mTotalRotationX;
		Ogre::Radian mTotalRotationY;

		Hand* mHand;

		// Private methodes
		void holdRightHand(Object* obj);
		void holdLeftHand(Object* obj);

		Statistiques mStats;
	};
}

#endif // HEROS_H
