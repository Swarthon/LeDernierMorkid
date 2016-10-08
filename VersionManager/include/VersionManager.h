#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include "Version.h"
#include "File.h"

namespace VersionManager {

	class VersionManager {
	public:
		// Construction methodes
		VersionManager(std::string versionsFileURL, std::string downloadPath = "./", std::string installPath = "./", File downloadedHistory = File("download.history","./"), File installedHistory = File("installed.history","./"));
		void setDownloadPath(std::string);
		void setInstallPath(std::string);

		bool download(std::string name);
		bool install(std::string name, std::string execName, std::string shortcutName, std::string execPath);

		// Return value methodes
		std::vector<std::string> getLoadedVersionsName();
		std::vector<std::string> getDownloadedVersionsName();
	protected:
		std::vector<Version*> mLoadedVersions;
		std::vector<Version*> mDownloadedVersions;
		Version* mInstalledVersion;

		File mDownloadedHistory;
		File mInstalledHistory;

		std::string mDownloadPath;
		std::string mInstallPath;

		// Private methodes
		void loadDownloadedVersions(File file = File("downloaded.history","./"));
		void loadInstalledVersion(File file = File("installed.history","./"));
		void saveDownloadedVersions();
		void saveInstalledVersion();
		void loadVersionsFromFile(std::string versionsFileURL);
	};

}

#endif // VERSION_MANAGER_H