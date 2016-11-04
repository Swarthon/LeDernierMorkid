#ifndef LANGUAGE_H
#define LANGUAGE_H

// C++ includes
#include <map>
#include <string>
#include <iostream>
#include <fstream>

// C includes
#include <libintl.h>
#include <locale.h>

// OGRE
#include <Ogre.h>

namespace Morkidios {

	class Language {
	public:
		static Language* getSingleton();

		// Construction methodes
		void setLanguage(std::string name);
		void setFileName(std::string fileName);

		// Various methodes
		void save();
		bool load();

		// Return value methodes
		std::map<std::string,std::string> getLanguages();
	private:
		// Construction methodes
		Language();
		~Language();
		static Language* mSingleton;

		std::pair<std::string,std::string> mActiveLanguage;
		std::map<std::string,std::string> mLanguages;

		std::string mFileName;
	};

}

#endif // LANGUAGE_H
