#ifndef CORE_H
#define CORE_H

// C++ includes
#include <string>
#include <vector>

// OGRE includes
#include <Ogre.h>

// Boost includes
#include <boost/dll/import.hpp> // for import_alias
#include <boost/function.hpp>

// My includes
#include "Mod.h"

namespace Core {
	class Core {
	public:
		// Construction methodes
		Core();
		~Core();
		void setConfigFileName(std::string name);
		bool load();

		// Return value methodes
	private:
		// Private methodes
		Mod* loadMod(std::string path);

		// Private members
		std::string mConfigFileName;
		std::vector<Mod*> mMods;
	};
}

#endif // CORE_H
