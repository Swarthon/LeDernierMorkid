#ifndef ZIPPER_H
#define ZIPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Linux
#ifdef __linux__
#include <unistd.h>
#include <dirent.h>
#include <sys/resource.h>
#include <sys/types.h>
#endif

// Windows
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <zip.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

// My includes
#include "File.h"

namespace VersionManager {

	class Zipper {
	public:
		static bool unzip(File src, File dst, std::vector<File>& listOfFiles);
	protected:		
		static void createDir(std::string dir);
	private:
		Zipper();
		virtual ~Zipper() = 0;
	};

}

#endif // ZIPPER_H