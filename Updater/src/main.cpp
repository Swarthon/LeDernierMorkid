#include "Downloader.h"
#include "Version.h"

#include <iostream>

int main(){
	Downloader downloader ("https://sourceforge.net/projects/lederniermorkid/files/versions.txt/download", "versions.txt");
	downloader.run();

	std::vector<Version> v = Version::createVersions("versions.txt");
	for(int i = 0; i < v.size(); i++){
		std::cout << "\n\nDowloading version " << v[i].mName << std::endl << std::endl;
		v[i].downloadFile();
		v[i].downloadVersion();
	}

	return 0;
}