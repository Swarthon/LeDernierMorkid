#ifndef FILE_H
#define FILE_H

// C++ includes
#include <string>
#include <fstream>
#include <iostream>

// C includes
#include <stdlib.h>

// Boost includes
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#ifdef __linux__
#include <dirent.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace VersionManager {

	class File {
	public:
		// Construction methodes
		File();
		File(std::string name, std::string path);
		~File();

		// Return value methodes
		std::string getName();
		std::string getPath();

		// Various methodes
		bool exist();
		void create();
		std::string getRelativePath(File to);
		template<class Archive>
        	void serialize(Archive & ar, const unsigned int version) {
			ar & mName;
			ar & mPath;
        	}

	protected:
		std::string mName;
		std::string mPath;
	};

}

#endif // FILE_H
