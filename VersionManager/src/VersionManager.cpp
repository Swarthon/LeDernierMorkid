#include "VersionManager.h"

namespace VersionManager {
	
	// Construction methodes
	VersionManager::VersionManager(std::string versionsFileURL, std::string downloadPath, std::string installPath, File downloadedHistory, File installedHistory){
		mDownloadPath = downloadPath;
		mInstallPath = installPath;
		
		loadDownloadedVersions(downloadedHistory);
		loadInstalledVersion(installedHistory);

		if(versionsFileURL != std::string())
			loadVersionsFromFile(versionsFileURL);
	}

	bool VersionManager::download(std::string name){
		for(unsigned int i = 0; i < mDownloadedVersions.size(); i++)
			if(mDownloadedVersions[i]->getName() == name)
				return true;

		Version* v;
		for(unsigned int i = 0; i < mLoadedVersions.size(); i++)
			if(mLoadedVersions[i]->getName() == name)
				v = mLoadedVersions[i];

		if(v->getName() != std::string()){
			if(v->download()){
				mDownloadedVersions.push_back(v);
				saveDownloadedVersions();
				return true;
			}
			else
				return false;
		}
		return false;
	}
	bool VersionManager::install(std::string name, std::string execName, std::string shortcutName, std::string execPath){
		Version* v;
		for(unsigned int i = 0; i < mDownloadedVersions.size(); i++)
			if(mDownloadedVersions[i]->getName() == name)
				v = mDownloadedVersions[i];

		if(v->getName() != std::string()){
			if(v->install(execName, shortcutName, execPath)){
				mInstalledVersion = v;
				saveInstalledVersion();
				return true;
			}
			return false;
		}
		return false;
	}

	// Return value methodes
	std::vector<std::string> VersionManager::getLoadedVersionsName(){
		std::vector<std::string> v;
		for(unsigned int i = 0; i < mLoadedVersions.size(); i++)
			v.push_back(mLoadedVersions[i]->getName());

		return v;
	}
	std::vector<std::string> VersionManager::getDownloadedVersionsName(){
		std::vector<std::string> v;
		for(unsigned int i = 0; i < mDownloadedVersions.size(); i++)
			v.push_back(mDownloadedVersions[i]->getName());

		return v;
	}

	// Private methodes
	void VersionManager::loadDownloadedVersions(File file){
		mDownloadedHistory = file;
		std::ifstream ifs;
		ifs.open((file.getPath() + file.getName()).c_str());
		while(ifs){
			std::string name, url;
			char buffer;
			ifs >> name >> buffer >> url;
			if(name != std::string() && url != std::string())
				mDownloadedVersions.push_back(new Version(name, url, mDownloadPath, mInstallPath, true));
		}
		ifs.close();
	}
	void VersionManager::loadInstalledVersion(File file){
		mInstalledHistory = file;
		std::ifstream ifs;
		ifs.open((file.getPath() + file.getName()).c_str());
		if(!ifs)
			return;
		std::string name, url;
		char buffer;
		ifs >> name >> buffer >> url;
		if(name != std::string() && url != std::string())
			mInstalledVersion = new Version(name, url, mDownloadPath, mInstallPath, true, true);
		ifs.close();
	}
	void VersionManager::saveDownloadedVersions(){
		std::ofstream ofs;
		ofs.open((mDownloadedHistory.getPath() + mDownloadedHistory.getName()).c_str());
		for(unsigned int i = 0; i < mDownloadedVersions.size(); i++)
			ofs << mDownloadedVersions[i]->getName() << " = " << mDownloadedVersions[i]->getURL() << std::endl;
		ofs.close();
	}
	void VersionManager::saveInstalledVersion(){
		std::ofstream ofs;
		ofs.open((mInstalledHistory.getPath() + mInstalledHistory.getName()).c_str());
		ofs << mInstalledVersion->getName() << " = " << mInstalledVersion->getURL() << std::endl;
		ofs.close();
	}
	void VersionManager::loadVersionsFromFile(std::string versionsFileURL){
		Downloader::download(File("versions.txt", "./"), versionsFileURL);

		std::ifstream ifs;
		ifs.open("versions.txt");

		if(!ifs){
			std::cerr << "Error while opening file versions.txt" << std::endl;
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

			std::string name, url;
			char buffer;
			ifs >> name >> buffer >> url;
			mLoadedVersions.push_back(new Version(name, url, mDownloadPath, mInstallPath));
		}
		ifs.close();
	}

}