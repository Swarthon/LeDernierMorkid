#include "VersionManager.h"

namespace VersionManager {

	// Construction methodes
	VersionManager::VersionManager(){
		mVersionsHistory = File("versions.versions", "./");
	}
	void VersionManager::loadFromInternet(std::string versionsFileURL){
		loadVersionsFromFile(versionsFileURL);
	}
	void VersionManager::loadVersions(File file){
		std::ifstream ifs((file.getPath() + file.getName()).c_str());
		if(!ifs){
			std::cout << "Could not open file " << file.getPath() + file.getName() << std::endl;
			return;
		}
                boost::archive::text_iarchive ia(ifs);
                ia >> mVersions;
	}
	void VersionManager::setDownloadPath(std::string downloadPath){
		mDownloadPath = downloadPath;
	}
	void VersionManager::setInstallPath(std::string installPath){
		mUnzipPath = installPath;
	}

	bool VersionManager::download(std::string name, int(*progressFunc)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t), void* userData){
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->getName() == name && mVersions[i]->isDownloaded())
				return true;

		Version* v = NULL;
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->getName() == name && !mVersions[i]->isDownloaded() && !mVersions[i]->isUnziped() && !mVersions[i]->isInstalled())
				v = mVersions[i];

		if(v){
			if(v->download(progressFunc, userData)){
				save();
				return true;
			}
			else
				return false;
		}
		return false;
	}
	bool VersionManager::unzip(std::string name){
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->getName() == name && mVersions[i]->isUnziped())
				return true;

		Version* v = NULL;
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->getName() == name && mVersions[i]->isDownloaded() && !mVersions[i]->isUnziped() && !mVersions[i]->isInstalled())
				v = mVersions[i];

		if(v){
			if(v->unzip()){
				save();
				return true;
			}
			else
				return false;
		}
		return false;
	}
	bool VersionManager::install(std::string name, std::string execName, std::string shortcutName, std::string execPath){
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->getName() == name && mVersions[i]->isInstalled())
				return true;

		Version* v = NULL;
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->getName() == name && mVersions[i]->isDownloaded() && mVersions[i]->isUnziped() && !mVersions[i]->isInstalled())
				v = mVersions[i];


		if(v){
			if(v->install(execName, shortcutName, execPath)){
				save();
				return true;
			}
			return false;
		}
		return false;
	}

	// Return value methodes
	std::vector<std::string> VersionManager::getLoadedVersionsName(){
		std::vector<std::string> v;
		for(unsigned int i = 0; i < mVersions.size(); i++)
			v.push_back(mVersions[i]->getName());

		return v;
	}
	std::vector<std::string> VersionManager::getUnzipedVersionsName(){
		std::vector<std::string> v;
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->isUnziped())
				v.push_back(mVersions[i]->getName());

		return v;
	}
	std::vector<std::string> VersionManager::getDownloadedVersionsName(){
		std::vector<std::string> v;
		for(unsigned int i = 0; i < mVersions.size(); i++)
			if(mVersions[i]->isDownloaded())
				v.push_back(mVersions[i]->getName());

		return v;
	}

	// Private methodes
	void VersionManager::save(File file){
		std::ofstream ofs((file.getPath() + file.getName()).c_str());
		boost::archive::text_oarchive oa(ofs);
                oa << mVersions;
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

			if(name != std::string()){
				bool b = false;
				for (size_t i = 0; i < mVersions.size(); i++) {
					if(mVersions[i]->getName() == name){
						b = true;
						break;
					}
				}
				if(!b)
					mVersions.push_back(new Version(name, url, mDownloadPath, mUnzipPath));
			}
		}
		ifs.close();
	}

}
