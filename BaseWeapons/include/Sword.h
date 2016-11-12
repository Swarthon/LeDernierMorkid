#ifndef SWORD_H
#define SWORD_H

// Morkdios includes
#include <Morkidios.h>

class Sword : public Morkidios::Weapon {
public:
	// Construction methodes
	static Sword* create(Ogre::SceneManager* smgr, btDynamicsWorld* world);
private:
	// Construction methodes
	Sword(Ogre::SceneManager* smgr, btDynamicsWorld* world);
	~Sword();
};

#endif // SWORD_H
