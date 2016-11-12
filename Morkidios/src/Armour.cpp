#include "Armour.h"

namespace Morkidios {

	// Construction methodes
	Armour::Armour(){
		mArmourType = NONE;
		mFeatures = Features();
	}
	Armour::~Armour(){
	}
	void Armour::setType(ArmourType t){
		mArmourType = t;
	}

	// Return value methodes
	Armour::ArmourType Armour::getArmourType(){
		return mArmourType;
	}
	Armour::Features& Armour::getFeatures(){
		return mFeatures;
	}

}
