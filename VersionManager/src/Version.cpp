#include "Version.h"

namespace VersionManager {

	// Construction methodes
	Version::Version(){
		mDownloaded = false;
		mInstalled = false;
	}
	Version::Version(std::string name, std::string url, std::string downloadPath, std::string installPath, bool downloaded, bool unziped, bool installed){
		mName = name;
		mURL = url;
		mInstallPath = installPath;
		mDownloadPath = downloadPath;
		mDownloaded = downloaded;
		mUnziped = unziped;
		mInstalled = installed;
	}
	Version::~Version(){
	}

	// Various methodes
	bool Version::download(int(*progressFunc)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t), void* userData){
		if(!mDownloaded && !mUnziped && !mInstalled){
			mDownloaded = Downloader::download(File(mName+".download",mDownloadPath),mURL,progressFunc, userData);
			if(!mDownloaded)
				return false;
		}
		return true;
	}
	bool Version::unzip(){
		if(mDownloaded && !mUnziped && !mInstalled){
			if(Zipper::unzip(File(mName+".download",mDownloadPath),File(std::string(),mInstallPath), mFiles)){
				std::ofstream ofs (mInstallPath+mName+"/name.txt");
				ofs << mName;
				ofs.close();
				mUnziped = true;
				return true;
			}
			return false;
		}
		return true;
	}
	bool Version::install(){
		if(mDownloaded && mUnziped && !mInstalled){
#ifdef _WIN32
            std::cout << "Installing " << mName << std::endl;
            if(!MoveFile((mInstallPath+mName).c_str(), (mInstallPath+"Installed").c_str()))
                std::cout << GetLastError() << std::endl;
#endif // _WIN32
                mInstalled = true;
		}
		return true;
	}
	bool Version::uninstall(){
        if(mInstalled){
#ifdef _WIN32
            std::cout << "Uninstalling " << mName << std::endl;
            if(MoveFile((mInstallPath+"Installed").c_str(), (mInstallPath+mName).c_str()))
                std::cout << GetLastError() << std::endl;
#endif // _WIN32
                mInstalled = false;
		}
		return true;
	}

	void Version::save(){
		std::ofstream ofs((mName + std::string(".version").c_str()));
		boost::archive::text_oarchive oa(ofs);
                oa << *this;
	}

	// Return value methodes
	std::string Version::getName(){
		return mName;
	}
	std::string Version::getDownloadPath(){
		return mDownloadPath;
	}
	std::string Version::getInstallPath(){
		return mInstallPath;
	}
	std::string Version::getURL(){
		return mURL;
	}
	bool Version::isDownloaded(){
		return mDownloaded;
	}
	bool Version::isUnziped(){
		return mUnziped;
	}
	bool Version::isInstalled(){
		return mInstalled;
	}
}
