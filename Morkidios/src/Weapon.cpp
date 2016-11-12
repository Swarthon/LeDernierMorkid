#include "Weapon.h"

std::vector<std::pair<std::string,Morkidios::Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*)>> Morkidios::Weapon::mLoadedWeaponsClass = std::vector<std::pair<std::string,Morkidios::Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*)>>();

namespace Morkidios {

	// Construction methodes
	Weapon::Weapon(){
		mFeatures = Features();
	}
	Weapon::~Weapon(){
	}
	void Weapon::addLoadedWeaponsClass(std::pair<std::string,Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*)>constructor){
		mLoadedWeaponsClass.push_back(constructor);
	}
	std::vector<std::pair<std::string,Weapon*(*)(Ogre::SceneManager*, btDynamicsWorld*)>> Weapon::getLoadedWeaponsClass(){
		return mLoadedWeaponsClass;
	}

	// Return value methodes
	Weapon::Features& Weapon::getFeatures(){
		return mFeatures;
	}

}
