#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include "Version.h"
#include "File.h"

namespace VersionManager {

	class VersionManager {
	public:
		// Construction methodes
		VersionManager();
		void loadFromInternet(std::string versionsFileURL);
		void loadVersions(File file = File("versions.versions","./"));
		void setDownloadPath(std::string downloadPath);
		void setInstallPath(std::string installPath);

		bool download(std::string name, int(*progressFunc)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t) = NULL, void* userData = NULL);
		bool unzip(std::string name);
		bool install(std::string name, std::string execName, std::string shortcutName, std::string execPath);

		// Return value methodes
		std::vector<std::string> getLoadedVersionsName();
		std::vector<std::string> getUnzipedVersionsName();
		std::vector<std::string> getDownloadedVersionsName();
	protected:
		std::vector<Version*> mVersions;

		File mVersionsHistory;

		std::string mDownloadPath;
		std::string mUnzipPath;

		// Private methodes
		void save(File file = File("versions.versions","./"));
		void loadVersionsFromFile(std::string versionsFileURL);
	};

}

#endif // VERSION_MANAGER_H
