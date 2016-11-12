#ifndef MOD_H
#define MOD_H

// C++ includes
#include <string>
#include <iostream>

namespace Core {
	class Mod {
	public:
		// Construction methodes
		Mod();
		~Mod();
		bool load();

		// Return value methodes
	private:

		// Private members
		std::string mPath;
	};
}

#endif // MOD_H
