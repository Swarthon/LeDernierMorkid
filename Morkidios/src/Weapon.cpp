#include "Weapon.h"

namespace Morkidios {

	// Construction methodes
	Weapon::Weapon(){
		mFeatures = Features();
	}
	Weapon::~Weapon(){
		if(mEntity){
			mEntity->detachFromParent();
			mEntity->_getManager()->destroyEntity(mEntity);
		}
	}

	// Return value methodes
	Weapon::Features& Weapon::getFeatures(){
		return mFeatures;
	}

}