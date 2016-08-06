#include "Version.h"

// Methodes
void Version::downloadFile(){
	std::cerr << mName << ' ' << mFileWebPath << std::endl << std::endl;
	Downloader d(mFileWebPath, mName);
	d.run();
}
void Version::downloadVersion(){
	std::cerr << mName << ' ' << mVersionWebPath << std::endl << std::endl;
	Downloader d(mVersionWebPath, mName + "version.txt");
	d.run();
}

// Static public methodes
std::vector<Version> Version::createVersions(std::string versionsFile){
	std::vector<Version> versions;
	std::ifstream ifs;
	ifs.open(versionsFile.c_str());

	if(!ifs){
		std::cerr << "Error while opening file " << versionsFile << std::endl;
		exit(-1);
	}

	while(ifs){
		char c;
		c = ifs.get();

		if(c == '#'){
			std::string buffer;
			getline(ifs, buffer);
			continue;
		}

		ifs.putback(c);

		Version v;
		ifs >> v;
		versions.push_back(v);
	}

	return versions;
}

// Operators
std::istream& operator>>(std::istream& is, Version& v){
	char c;	
	is >> v.mName >> c >> v.mVersionWebPath >> c >> v.mFileWebPath;
	std::cout << v.mName << ' ' << v.mVersionWebPath << ' ' << v.mFileWebPath << std::endl;

	// is.setstate(std::ios::failbit);

	return is;
}

// Static private methodes
std::string Version::getStringTo(std::string base, char to, bool includeTo){
	std::string s;
	for(int i = 0; i < base.size() && base[i] != to; i++)
		s += base[i];
	if(includeTo)
		s += to;

	return s;
}
std::string Version::readTo(std::istream& is, char to, bool includeTo){
	std::string s;
	char c = (to == ' ' ? 'a' : ' ');
	while(is && c != to)
		s += (c = is.get());
	if(!includeTo){
		is.putback(s[s.size()-1]);
		s.erase(s[s.size()-1]);
	}
}