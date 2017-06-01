#include "Sword.h"

// Construction methodes
Sword* Sword::create(Ogre::SceneManager* smgr, btDynamicsWorld* world){
	return new Sword(smgr,world);
}

// Construction methodes
Sword::Sword(Ogre::SceneManager* smgr, btDynamicsWorld* world){
	mName = "Sword";
	load(smgr, world, "Sword","Bow.mesh");
	setType(Morkidios::Object::Weapon);
}
Sword::~Sword(){
}
