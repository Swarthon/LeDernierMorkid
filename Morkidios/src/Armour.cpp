#include "Armour.h"

namespace Morkidios {

	// Construction methodes
	Armour::Armour(){
		mType = NONE;
		mFeatures = Features();
	}
	Armour::~Armour(){
	}
	void Armour::setType(Armour::Type t){
		mType = t;
	}

	// Return value methodes
	Armour::Type Armour::getType(){
		return mType;
	}
	Armour::Features& Armour::getFeatures(){
		return mFeatures;
	}

}