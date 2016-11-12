#include "LDMGame.h"

// Construction methodes
LDMGame::LDMGame(){
	new Morkidios::Framework;
	if(!Morkidios::Framework::getSingletonPtr()->initOgre(Ogre::String("Le Dernier Morkid"))){
		std::cout << "Framework initialization problem";
		exit(1);
	}

	Morkidios::Framework::getSingletonPtr()->mLog->logMessage("Le Dernier Morkid is started");

	initGui();

	Morkidios::GraphicOptions::getSingleton()->setRenderWindow(Morkidios::Framework::getSingletonPtr()->mRenderWindow);
	if(!Morkidios::GraphicOptions::getSingleton()->load())
		Morkidios::GraphicOptions::getSingleton()->save();
	Morkidios::GraphicOptions::getSingleton()->config();


	Core::Core co;
	co.load();

	mStateManager = new Morkidios::StateManager();

	GameState::create(mStateManager, "GameState");
	Morkidios::StateManager::loadAllStates(mStateManager);

	mStateManager->start(mStateManager->findByName("MainMenuState"));
}

// Private construction methodes
void LDMGame::initGui(){
	CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
	CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	CEGUI::SchemeManager::getSingleton().createFromFile("AlfiskoSkin.scheme");
	CEGUI::SchemeManager::getSingleton().createFromFile("Generic.scheme");

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
}
