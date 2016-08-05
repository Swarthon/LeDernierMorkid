#include "Installer.h"

#include <iostream>

int main(){
	Installer installer ("https://sourceforge.net/projects/lederniermorkid/files/latest/download");
	if(!installer.run()){
		std::cerr << "Error !\n";
	}
}