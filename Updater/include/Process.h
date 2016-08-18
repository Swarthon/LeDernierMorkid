#ifndef PROCESS_H
#define PROCESS_H

// C++ includes
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

// C include
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Version.h"

class Process {
public:
	std::string process(std::string);
};

#endif // PROCESS_H