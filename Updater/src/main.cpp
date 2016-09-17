#include "Downloader.h"
#include "Version.h"
#include "Zip.h"
#include "Pipe.h"
#include "UserInput.h"

#include <iostream>

// LeDernierMorkid versions.txt url = "https://sourceforge.net/projects/lederniermorkid/files/versions.txt/download"

int userInput(){
	UserInput u;
	u.run();

	return 0;
}
int pipe(const char* pipeName){
	Pipe p;
	p.run(pipeName);

	return 0;
}

/*int main(int argc, char** argv){
	if(argc == 1){
		std::cout << "No parameter given.\n\t" << argv[0] << " [userInput/pipe]\n";
		return 1;
	}
	if(memcmp(argv[1],"userInput",sizeof("userInput")) == 0)
		return userInput();

	else if(memcmp(argv[1],"pipe",sizeof("pipe")) == 0){
		if(argc <= 2){
			std::cout << "No parameter given.\n\t" << argv[0] << ' ' << argv[1] << " [pipeName]\n";
			return 1;
		}
		return pipe(argv[2]);
	}

	else {		
		std::cout << "No parameter given.\n\t" << argv[0] << " [userInput/configFile/parameters]\n";
		return 1;
	}

	return 0;
}*/