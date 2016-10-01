#ifndef ZIP_H
#define ZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __linux__
#include <unistd.h>
#include <dirent.h>
#include <sys/resource.h>
#include <sys/types.h>
#endif

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

class Zip {
public:
	Zip(std::string archiveName, std::string path = "");
	void extract();

private:
	std::string mName;
	std::string mPath;

	static void safe_create_dir(const char *dir);
};

#endif // ZIP_H
