#ifndef VERSION_H
#define VERSION_H

#include <vector>

#include "File.h"
#include "Downloader.h"
#include "Zipper.h"

namespace VersionManager {

	class Version {
	public:
		// Construction methodes
		Version();
		Version(std::string name, std::string url, std::string downloadPath = "./", std::string installPath = "./", bool downloaded = false, bool installed = false);
		~Version();

		// Various methodes
		bool download();
		bool install(std::string execName, std::string shortcutName, std::string execPath);

		// Return value methodes
		std::string getName();
		std::string getDownloadPath();
		std::string getInstallPath();
		std::string getURL();
	
	protected:
		void createShortcut(std::string strSrcFile, std::string strDstPath, std::string strName);

		std::vector<File> mFiles;
		std::string mName;
		std::string mURL;
		std::string mDownloadPath;
		std::string mInstallPath;
		bool mDownloaded;
		bool mInstalled;
	};

}

#endif // VERSION_H