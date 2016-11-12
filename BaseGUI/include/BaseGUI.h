#ifndef BASEGUI_H
#define BASEGUI_H

// Morkdios includes
#include <Morkidios.h>

// Core includes
#include <Mod.h>

// Boost include
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS

// My includes
#include "Inventory.h"
#include "GraphicMenuState.h"
#include "LanguageChangeState.h"
#include "MainMenuState.h"
#include "PauseMenuState.h"
#include "OptionsMenuState.h"
#include "KeyboardMenuState.h"

class BaseGUI : public Core::Mod {
public:
	BaseGUI();
	~BaseGUI();
	static boost::shared_ptr<BaseGUI> load();
private:
};

BOOST_DLL_ALIAS(
	BaseGUI::load,
	loadMod
)

#endif // BASEGUI_H
