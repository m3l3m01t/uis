#include "ModuleManager.h"
#include "TouchModule.h"
#include "KeyboardModule.h"

//map<uint32_t, ModuleFactory *>  ModuleManager::_modules;
map<uint32_t, ModuleFactory *>  ModuleManager::_factories;
template<> ModuleManager* Singleton<ModuleManager>::_instance = NULL;

static KeyboardModuleFactory kmFactory;
static TouchModuleFactory tmFactory;

int ModuleManager::initialize ()
{
	registerModule (kmFactory);
	registerModule (tmFactory);

	return 0;
}
