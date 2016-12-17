#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Utils.h"

namespace Morkidios {

	class _MorkidiosExport CrossHair {
	public:
		// Construction methodes
		CrossHair();
		~CrossHair();
		void init(std::string fileName);
		void setSize(CEGUI::UDim u);

		// Various methodes
		void show(bool b);
	private:
		CEGUI::Window* mCrossHair;
	};

}

#endif // CROSSHAIR_H
