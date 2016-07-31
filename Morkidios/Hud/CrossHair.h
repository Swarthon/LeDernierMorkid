#ifndef CROSSHAIR_H
#define CROSSHAIR_H

// Ogre includes
#include <OGRE/Ogre.h>

// CEGUI includes
#include <CEGUI/CEGUI.h>

// My includes
#include "Utils.h"

namespace Morkidios {

	class CrossHair {
	public:
		// Construction methodes
		CrossHair();
		~CrossHair();
		void init(std::string fileName);

		// Various methodes
		void show(bool b);
	private:
		CEGUI::Window* mCrossHair;
	};

}

#endif // CROSSHAIR_H