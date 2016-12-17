#ifndef TERRAIN_H
#define TERRAIN_H

// My includes
#include "Framework.h"
#include "Character/AI.h"
#include "Character/Hero.h"

namespace Morkidios {

	class _MorkidiosExport Terrain {
	public:
		// Construction methodes
		Terrain();
		virtual ~Terrain();
		virtual void init(Ogre::SceneManager* smgr, double worldSize = 50, double height = 10, std::string name = std::string());
		void setHero(Hero* h);

		// Return value methodes
		btDynamicsWorld* getWorld();
		Ogre::Vector3 getSize();

		// Various methodes
		void update(double time);
		void addDroppedObject(Object* obj);
		void removeDroppedObject(Object* obj);

		// Static methodes
		static Terrain* getActiveTerrain();
		static void setActiveTerrain(Terrain* t);
	protected:
		std::vector<AI*> mAI;
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

		// Static member
		static Terrain* mActiveTerrain;
	};

}

#endif // TERRAIN_H
