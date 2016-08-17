#ifndef VERSION_H
#define VERSION_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "Downloader.h"
#include "Zip.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)  && defined(__MACH__)
#endif

class Version {
public:
	// Methodes
	void downloadFile(std::string path = std::string());
	void downloadVersion(std::string path = std::string());
	void install(std::string executable, std::string path = std::string());

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
	static void createShortcut(std::string strSrcFile, std::string strDstPath, std::string strName);
	static std::string getStringTo(std::string base, char to, bool includeTo = false);
	static std::string readTo(std::istream& is, char to, bool includeTo = false);
};

#endif // VERSION_H