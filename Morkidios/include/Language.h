#ifndef LANGUAGE_H
#define LANGUAGE_H

// Morkidios includes
#include <MorkidiosPrerequisites.h>

namespace Morkidios {

	class _MorkidiosExport Language {
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
