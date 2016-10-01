#include "Input.h"

Morkidios::Input* Morkidios::Input::mSingleton = NULL;

namespace Morkidios {
	Input* Input::getSingleton(){
		if(!mSingleton)
			mSingleton = new Input();
		return mSingleton;
	}
	void Input::destroy(){
		delete mSingleton;
	}

	// Construction methodes
	void Input::setFileName(std::string fileName){
		mFileName = fileName;
	}

	// Various methodes
	void Input::save(){
		std::ofstream cfg(mFileName.c_str());
		cfg << "[Mouse]" << std::endl;
		cfg << "MouseSensibility=" << mMouseSensibility << std::endl;
		cfg << "[Keyboard]" << std::endl;
		for(std::map<std::string, OIS::KeyCode>::iterator it = mKeyMap.begin(); it != mKeyMap.end() ;it++)
			cfg << it->first << '=' << it->second << std::endl;
	}
	bool Input::load(){
		try {
			Ogre::ConfigFile cfg;
			cfg.load(mFileName.c_str());
			
			mMouseSensibility = Ogre::StringConverter::parseReal(cfg.getSetting("MouseSensibility", "Mouse"));
			Ogre::ConfigFile::SettingsIterator settings = cfg.getSettingsIterator("Keyboard");
			for(Ogre::ConfigFile::SettingsMultiMap::iterator it = settings.begin(); it != settings.end(); it++)
				mKeyMap[it->first] = (OIS::KeyCode)Ogre::StringConverter::parseInt(it->second);

			return true;
		}
		catch (Ogre::Exception& e){
			return false;
		}
	}

	// Construction methodes
	Input::Input(){
		mInputManager = 0;
		mKeyboard = 0;
		mMouse = 0;

		mFileName = "input.cfg";

		mMouseSensibility = 10;
		mKeyMap["Forward"] = OIS::KC_Z;
		mKeyMap["Backward"] = OIS::KC_S;
		mKeyMap["Leftward"] = OIS::KC_Q;
		mKeyMap["Rightward"] = OIS::KC_D;
	}
	Input::~Input(){
		if(mInputManager)
			OIS::InputManager::destroyInputSystem(mInputManager);
	}
}