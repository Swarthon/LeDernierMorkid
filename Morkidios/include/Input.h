#ifndef INPUT_H
#define INPUT_H

// C++ includes
#include <map>
#include <string>
#include <iostream>

// OIS includes
#include <OIS/OIS.h>

// OGRE includes
#include <OGRE/Ogre.h>

namespace Morkidios {
	class Input {
	public:
		static Input* getSingleton();
		static void destroy();

		// Construction methodes
		void setFileName(std::string fileName);

		// Various methodes
		void save();
		bool load();

		OIS::InputManager*			mInputManager;
		OIS::Keyboard*				mKeyboard;
		OIS::Mouse*				mMouse;
	private:
		// Construction methodes
		Input();
		~Input();
		static Input* mSingleton;

		std::string mFileName;

	public:
		float mMouseSensibility;
		std::map <std::string, OIS::KeyCode> mKeyMap;
	};
}

#endif // INPUT_H