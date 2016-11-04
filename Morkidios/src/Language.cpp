#include "Language.h"

Morkidios::Language* Morkidios::Language::mSingleton = NULL;

namespace Morkidios {
	Language* Language::getSingleton(){
		if(!mSingleton)
			mSingleton = new Language();
		return mSingleton;
	}

	// Construction methodes
	void Language::setLanguage(std::string name){
		if(mLanguages[name] != std::string()){
			mActiveLanguage = std::pair<std::string,std::string>(name,mLanguages[name]);
			setlocale (LC_MESSAGES, mActiveLanguage.second.c_str());
		}
		else
			std::cout << "Language " << name << " was not found\n";
	}
	void Language::setFileName(std::string fileName){
		mFileName = fileName;
	}

	// Various methodes
	void Language::save(){
		std::ofstream cfg(mFileName.c_str());
		cfg << "[Language]" << std::endl;
		cfg << "Language=" << mActiveLanguage.first << std::endl;
	}
	bool Language::load(){
		try {
			Ogre::ConfigFile cfg;
			cfg.load(mFileName.c_str());

			setLanguage(cfg.getSetting("Language", "Language"));

			return true;
		}
		catch (Ogre::Exception& e){
			return false;
		}
	}

	// Return value methodes
	std::map<std::string,std::string> Language::getLanguages(){
		return mLanguages;
	}

	// Construction methodes
	Language::Language(){
		mFileName = "language.cfg";

		mLanguages["English"] = "C";
		mLanguages["Français"] = "fr_FR";
		mLanguages["Bosanski jezik"] = "bs_BA";

		bindtextdomain ("LeDernierMorkid", "./");
		bind_textdomain_codeset("LeDernierMorkid", "utf-8");
		textdomain ("LeDernierMorkid");
	}
	Language::~Language(){
	}
}
