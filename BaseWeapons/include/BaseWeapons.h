#ifndef BASEWEAPONS_h
#define BASEWEAPONS_h

// Morkdios includes
#include <Morkidios.h>

// Core includes
#include <Mod.h>

// Boost include
#define BOOST_DLL_FORCE_ALIAS_INSTANTIATION
#include <boost/shared_ptr.hpp>
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS

// My includes
#include "Sword.h"
#include "Torch.h"

class BaseWeapons : public Core::Mod {
public:
	BaseWeapons();
	~BaseWeapons();
	static boost::shared_ptr<BaseWeapons> load();
private:
};

BOOST_DLL_ALIAS(
	BaseWeapons::load,
	loadMod
)

#endif // BASEWEAPONS_h
