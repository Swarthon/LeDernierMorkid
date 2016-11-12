#ifndef TORCH_H
#define TORCH_H

// Morkdios includes
#include <Morkidios.h>

class Torch : public Morkidios::Weapon {
public:
	// Construction methodes
	static Torch* create(Ogre::SceneManager*, btDynamicsWorld*);

	// Various methodes
	void update(double timeSinceLastFrame);
private:
	// Construction methodes
	Torch(Ogre::SceneManager*, btDynamicsWorld*);
	~Torch();

	//double mLightTime;
	Ogre::Light* mLight;
	Ogre::ParticleSystem* mParticles;

	static int mNumber;
};

#endif // Torch_H
