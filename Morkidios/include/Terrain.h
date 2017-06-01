#ifndef TERRAIN_H
#define TERRAIN_H

// My includes
#include "Framework.h"
#include "AI/AI.h"
#include "Character/Hero.h"

#define FOR_EACH_AI 4

namespace Morkidios {

	class Terrain;

	class _MorkidiosExport DefaultNavigationAlgorithm : public NavigationAlgorithm {
	public:
		DefaultNavigationAlgorithm(){
			mTerrain = NULL;
		}
		DefaultNavigationAlgorithm(Terrain* t){
			mTerrain = t;
		};
		virtual void navigate(AI*){};
		virtual void searchForEnnemy(AI*){}

	protected:
		Terrain* mTerrain;
	};

	class _MorkidiosExport Terrain {
	public:
		// Construction methodes
		Terrain();
		virtual ~Terrain();
		virtual void init(Ogre::SceneManager* smgr, double worldSize = 50, double height = 10, std::string name = std::string());
		void addAI(AI* ai);

		// Return value methodes
		btDynamicsWorld* getWorld();
		Ogre::Vector3 getSize();

		// Various methodes
		virtual void update(double time);
		void addDroppedObject(Object* obj);
		void removeDroppedObject(Object* obj);
		virtual void heroAskForAttack(Character::Side s);

		// Static methodes
		static Terrain* getActiveTerrain();
		static void setActiveTerrain(Terrain* t);
	protected:
		std::vector<Object*> mDroppedObjects;

		std::string mName;

		// Ogre
		Ogre::SceneManager* mSceneManager;
		Ogre::StaticGeometry* mGeometry;
		Ogre::Vector3 mSize;

		// Bullet
		btDynamicsWorld* mWorld;
		btDefaultCollisionConfiguration* mCollisionConfiguration;
		btCollisionDispatcher* mDispatcher;
		btDbvtBroadphase* mBroadphase;
		btSequentialImpulseConstraintSolver* mSolver;

		// AI
		NavigationAlgorithm* mNavigationAlgorithm;
		std::vector<AI*> mAI;
		int mAIForEach;

		// Static member
		static Terrain* mActiveTerrain;
	};

}

#endif // TERRAIN_H
