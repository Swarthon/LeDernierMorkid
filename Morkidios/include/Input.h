#ifndef INPUT_H
#define INPUT_H

#include <MorkidiosPrerequisites.h>

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
		float mMouseSensibility;	// Belong to the interval [0;mMouseSensibilityMax]
		const float mMouseSensibilityMax;
		std::map <std::string, OIS::KeyCode> mKeyMap;
	};
}

#endif // INPUT_H
