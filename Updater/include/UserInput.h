#ifndef USERINPUT_H
#define USERINPUT_H

// C++ includes
#include <string>
#include <iostream>

#include "Process.h"

class UserInput : public Process {
public:
	void run();
private:
	std::string in();
	void out(std::string);
};

#endif // USERINPUT_H