#ifndef _COLLISIONTERRAIN_H_
#define _COLLISIONTERRAIN_H_

#include "Terrain.h"

class btDynamicsWorld;

class CollisionTerrain : public Terrain {
protected:
	btDynamicsWorld* mWorld;

	void createShape();

public:
	CollisionTerrain(btDynamicsWorld* world);
	void load(const Ogre::String&  texName,
	          const Ogre::Vector3  center,
	          const Ogre::Vector3& dimensions);
};

#endif
