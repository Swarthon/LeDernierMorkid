#ifndef PIPE_H
#define PIPE_H

// C++ includes
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

// C includes
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Downloader.h"
#include "Version.h"
#include "Process.h"

class Pipe : public Process {
public:
	void run(const char* pipeName);
private:
	std::string read(const char* pipeName);
	void send(const char* pipeName, std::string data);
};

#endif // PIPE_H