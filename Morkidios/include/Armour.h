#ifndef ARMURE_H
#define ARMURE_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Object.h"

namespace Morkidios {

	class _MorkidiosExport Armour : public Object {
	public:
		// Public enums, structs and classes
		enum ArmourType {
			HELMET,
			BREASTPLATE,
			GREAVES,
			SHOES,
			NONE
		};
		class _MorkidiosExport Features {
		public:
			Features() : life(0), intelligence(0), agility(0), force(0), precision(0), speed(0), range(0){}

			double life;
			double intelligence;
			double agility;
			double force;
			double precision;
			double speed;
			double range;
		};

		// Construction methodes
		Armour();
		~Armour();
		void setType(ArmourType t);

		// Return value methodes
		ArmourType getArmourType();
		Features& getFeatures();

		// Various methodes
	private:
		ArmourType mArmourType;
		Features mFeatures;
	};

}

#endif // ARMURE_H
