#ifndef ARME_H
#define ARME_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Object.h"

namespace Morkidios {

	class Weapon : public Object {
	public:
		// Public classes
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
		Weapon();
		~Weapon();

		// Return value methodes
		Features& getFeatures();

		// Various methodes
	private:
		Features mFeatures;
	};

}

#endif // ARME_H
