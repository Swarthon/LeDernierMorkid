#ifndef MOD_H
#define MOD_H

// C++ includes
#include <string>
#include <iostream>

#include <boost/function.hpp>

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
		boost::function<boost::shared_ptr<Mod>()> mCreator;
	};
}

#endif // MOD_H
