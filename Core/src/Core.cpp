#include "Core.h"

namespace Core {
	// Construction methodes
	Core::Core(){
		mConfigFileName = "mods.cfg";
	}
	Core::~Core(){
	}
	void Core::setConfigFileName(std::string name){
		mConfigFileName = name;
	}
	bool Core::load(){
		try {
			Ogre::ConfigFile cfg;
			cfg.load(mConfigFileName.c_str());
			Ogre::ConfigFile::SettingsIterator settings = cfg.getSettingsIterator("Mods");
			for(Ogre::ConfigFile::SettingsMultiMap::iterator it = settings.begin(); it != settings.end(); it++)
				mMods.push_back(loadMod(it->second));

			return true;
		} catch (Ogre::Exception& e){
			return false;
		}
	}

	// Private methodes
	Mod* Core::loadMod(std::string path){
		typedef boost::shared_ptr<Mod> (mod_create)();
		boost::function<mod_create> creator = boost::dll::import_alias<mod_create>(path,"loadMod");
		Mod* m = creator().get();
		mLoadedCreators.push_back(creator);
		return m;
	}
}
