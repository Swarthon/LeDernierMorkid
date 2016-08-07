#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <zip.h>
#include <string>

class Zip {
public:
	Zip(std::string archiveName, std::string path = "");
	void extract();

private:
	std::string mName;
	std::string mPath;

	static void safe_create_dir(const char *dir);
};