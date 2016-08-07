#include "Downloader.h"
#include "Version.h"
#include "Zip.h"
#include "Pipe.h"

#include <iostream>

// LeDernierMorkid versions.txt url = "https://sourceforge.net/projects/lederniermorkid/files/versions.txt/download"

int userInput(){
	std::cout << "Please write the versions.txt file's URL\n";
	std::string url;
	std::cin >> url;
	Downloader downloader (url, "versions.txt");
	downloader.run();

	std::vector<Version> versions = Version::createVersions("versions.txt");
	if(versions.size() == 0)
		return 0;
	std::cout << "Here are the available versions :\n";
	for(int i = 0; i < versions.size(); i++)
		std::cout << '\t' << versions[i].mName << std::endl;
	std::cout << "Which version do you want to download ?\n";
	std::string name;
	std::cin >> name;
	Version v = Version::searchVersionByName(versions, name);
	if(v.mName == std::string())
		return 1;
	v.downloadFile();
	v.downloadVersion();
	std::cout << "Version downloaded\n";
	std::cout << "Do you want to extract it ? [y/n](Default is yes)\n";
	std::string answer;
	std::cin >> answer;
	if(answer == "n")
		return 0;
	std::cout << "Where do you want to extract it ?\n";
	std::string path;
	std::cin >> path;
	Zip z(v.mName, path);
	z.extract();
	std::cout << "Work is done\n";

	return 0;
}
int pipe(const char* pipeName){
	// P: Version	-> C: List of versions
	// P: V#	-> C: Download + Done
	// P: Stop	-> C: Shutdown

	Pipe p;
	p.run(pipeName);
}

int main(int argc, char** argv){
	if(argc == 1){
		std::cout << "No parameter given.\n\t" << argv[0] << " [userInput/pipe]\n";
		return 1;
	}
	if(memcmp(argv[1],"userInput",sizeof("userInput")) == 0)
		return userInput();

	else if(memcmp(argv[1],"pipe",sizeof("pipe")) == 0){
		if(argc <= 2)
			std::cout << "No parameter given.\n\t" << argv[0] << ' ' << argv[1] << " [pipeName]\n";
		return pipe(argv[2]);
	}

	else {		
		std::cout << "No parameter given.\n\t" << argv[0] << " [userInput/configFile/parameters]\n";
		return 1;
	}

	return 0;
}