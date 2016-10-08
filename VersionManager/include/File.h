#ifndef FILE_H
#define FILE_H

// C++ includes
#include <string>
#include <fstream>

// C includes
#include <stdlib.h>

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
	protected:
		std::string mName;
		std::string mPath;
	};

}

#endif // FILE_H