#include "BaseGUI.h"

BaseGUI::BaseGUI(){
	Morkidios::StateManager::addToLoadState("InventoryState",&Inventory::create);
	Morkidios::StateManager::addToLoadState("GraphicMenuState",&GraphicMenuState::create);
	Morkidios::StateManager::addToLoadState("LanguageMenuState",&LanguageChangeState::create);
	Morkidios::StateManager::addToLoadState("MainMenuState",&MainMenuState::create);
	Morkidios::StateManager::addToLoadState("PauseMenuState",&PauseMenuState::create);
	Morkidios::StateManager::addToLoadState("OptionsMenuState",&OptionsMenuState::create);
	Morkidios::StateManager::addToLoadState("KeyboardMenuState",&KeyboardMenuState::create);
}
BaseGUI::~BaseGUI(){
}
boost::shared_ptr<BaseGUI> BaseGUI::load(){
	return boost::shared_ptr<BaseGUI>(new BaseGUI);
}
