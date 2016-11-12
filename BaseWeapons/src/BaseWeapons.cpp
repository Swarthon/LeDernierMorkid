#include "BaseWeapons.h"

BaseWeapons::BaseWeapons(){
	Morkidios::Weapon::addLoadedWeaponsClass(std::pair<std::string,Morkidios::Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*)>("Sword",(Morkidios::Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*))(&Sword::create)));
	Morkidios::Weapon::addLoadedWeaponsClass(std::pair<std::string,Morkidios::Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*)>("Torch",(Morkidios::Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*))(&Torch::create)));
}
BaseWeapons::~BaseWeapons(){
}
boost::shared_ptr<BaseWeapons> BaseWeapons::load(){
	return boost::shared_ptr<BaseWeapons>(new BaseWeapons);
}
