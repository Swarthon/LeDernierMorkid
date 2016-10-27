#include "VersionManagerGUI.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
	try {
		new Morkidios::Framework;
		if(!Morkidios::Framework::getSingletonPtr()->initOgre(Ogre::String("Le Dernier Morkid"))){
			std::cout << "Framework initialization problem";
			exit(1);
		}


		CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
		CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");

		Morkidios::StateManager* sm = new Morkidios::StateManager();

		VersionManager::VersionManagerGUI::create(sm, "VersionManagerState");
		sm->start(sm->findByName("VersionManagerState"));

	} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}
