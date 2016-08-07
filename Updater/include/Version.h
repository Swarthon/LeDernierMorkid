#ifndef VERSION_H
#define VERSION_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "Downloader.h"

class Version {
public:
	// Methodes
	void downloadFile();
	void downloadVersion();

	// Static public methodes
	static std::vector<Version> createVersions(std::string versionsFile);
	static Version searchVersionByName(std::vector<Version> versions, std::string name);

	// Operators
	friend std::istream& operator>>(std::istream& is, Version& v);
	
	std::string mName;
	std::string mFileName;
	std::string mFileWebPath;
	std::string mVersionWebPath;
private:
	// Static private methodes
	static std::string getStringTo(std::string base, char to, bool includeTo = false);
	static std::string readTo(std::istream& is, char to, bool includeTo = false);
};

#endif // VERSION_H