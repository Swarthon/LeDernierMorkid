#include "Torch.h"

int Torch::mNumber = 0;

// Construction methodes
Torch* Torch::create(Ogre::SceneManager* smgr, btDynamicsWorld* world){
	return new Torch(smgr, world);
}

// Various methodes
void Torch::update(double timeSinceLastFrame){
	/*if((mState == Held || mState == Dropped) && mLightTime > 0)
		mLightTime -= timeSinceLastFrame;
	if(mLightTime < 0)
		mLightTime = 0;
	if(!mLightTime){
		mLight->setVisible(false);
		mParticles->setVisible(false);
	}*/
}

// Construction methodes
Torch::Torch(Ogre::SceneManager* smgr, btDynamicsWorld* world){
	mName = "Torch";
	load(smgr, world, std::string("Torch") + Morkidios::Utils::convertIntToString(mNumber),"Torch.mesh");
	setType(Morkidios::Object::Weapon);

	mLight = smgr->createLight(std::string("TorchLight") + Morkidios::Utils::convertIntToString(mNumber));
	mLight->setType(Ogre::Light::LT_POINT);
	mLight->setDiffuseColour(Ogre::ColourValue(0.7, 0.7, 0.7));
	mLight->setSpecularColour(Ogre::ColourValue(0.7, 0.7, 0.7));
	mLight->setAttenuation(200, 1.0, 0.022, 0.0019);
	mLight->setVisible(false);
	mEntity->attachObjectToBone("Flame",mLight);
	mEntity->setCastShadows(false);

	mParticles = smgr->createParticleSystem(std::string("TorchFlame") + Morkidios::Utils::convertIntToString(mNumber), "Fire");
	mEntity->attachObjectToBone("Flame",mParticles);

	//mLightTime = 10;

	mNumber++;
}
Torch::~Torch(){
}
