#include "include/VersionManager.h"

int main(){
	VersionManager::VersionManager vm ("https://sourceforge.net/projects/lederniermorkid/files/versions.txt/download","../Downloads/","../Versions/");
	if(!vm.download("v0.5")){
		std::cout << "Fail to download\n";
		return 1;
	}
	if(!vm.install("v0.5", "LeDernierMorkid.exe", "Le Dernier Morkid", "../")){
		std::cout << "Fail to install\n";
		return 2;
	}
	return 0;
}