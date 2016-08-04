#ifndef ARMURE_H
#define ARMURE_H

// C++ includes
#include <sstream>

// My includes
#include "Object.h"

namespace Morkidios {

	class Armour : public Object {
	public:
		// Public enums, structs and classes
		enum Type {
			HELMET,
			BREASTPLATE,
			GREAVES,
			SHOES,
			NONE
		};
		class Features {
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
		void setType(Type t);
		
		// Return value methodes
		Type getType();
		Features& getFeatures();

		// Various methodes
	private:
		Type mType;
		Features mFeatures;
	};

}

#endif // ARMURE_H