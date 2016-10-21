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
		Version(std::string name, std::string url, std::string downloadPath = "./", std::string installPath = "./", bool downloaded = false, bool unziped = false, bool installed = false);
		~Version();

		// Various methodes
		bool download(int(*progressFunc)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t) = NULL, void* userData = NULL);
		bool unzip();
		bool install(std::string execName, std::string shortcutName, std::string execPath);

		void save();

		friend class boost::serialization::access;
        	template<class Archive>
        	void serialize(Archive & ar, const unsigned int version) {
			ar & mName;
			ar & mURL;
			ar & mDownloadPath;
			ar & mInstallPath;
			ar & mDownloaded;
			ar & mUnziped;
			ar & mInstalled;
			ar & mFiles;
        	}

		// Return value methodes
		std::string getName();
		std::string getDownloadPath();
		std::string getInstallPath();
		std::string getURL();
		bool isDownloaded();
		bool isUnziped();
		bool isInstalled();

	protected:
		void createShortcut(std::string strSrcFile, std::string strDstPath, std::string strName);

		std::vector<File> mFiles;
		std::string mName;
		std::string mURL;
		std::string mDownloadPath;
		std::string mInstallPath;
		bool mDownloaded;
		bool mUnziped;
		bool mInstalled;
	};

}

#endif // VERSION_H
