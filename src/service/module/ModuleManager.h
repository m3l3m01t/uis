/* vim: set ts=2 sw=2 et: */
#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H
#include "AbstractInputDev.h"
#include "InputModule.h"
#include "Singleton.h"

class ModuleManager : 
  public Singleton <ModuleManager>, 
  public IdStore <ModuleId, InputModule> 
{
  public:
    static ModuleFactory *getFactory (uint32_t deviceClass) {
      ModuleFactory *factory = NULL;
      map<uint32_t, ModuleFactory *>::iterator iter;

      for (iter = _factories.begin (); iter != _factories.end (); iter++) {
        if (iter->first & deviceClass) {
          factory = iter->second;
          return factory;
        }
      }

      return NULL;
    }

    InputModule *getModuleByType (uint32_t deviceClass, const char *keyword = NULL) {
      map <ModuleId, InputModule *>::iterator iter;

      for (iter = _ids.begin (); iter != _ids.end (); iter ++) {
        InputModule *module = iter->second;

        if (module->isDeviceSupport ((DeviceType) deviceClass)) {
          if (keyword && strstr (module->getName (), keyword) == NULL) {
            LOG_DEBUG ("module \"%s\" doesn't contain keyword %s", module->getName (), keyword);
            continue;
          }
          LOG_INFO ("found already opened module 0x%08x", module->getId ());
          return module;
        }
      }

      LOG_INFO ("create module deviceClass: 0x%08x", deviceClass);
      ModuleFactory *factory = getFactory (deviceClass);
      if (factory == NULL) {
        LOG_ERROR ("cannot find module factory for deviceClass: 0x%08x", deviceClass);
        return NULL;
      }

      AbstractInputDev *device = InputDevFactory::getDevice (deviceClass, keyword);
      if (device == NULL) {
        LOG_ERROR ("cannot find deviceClass: 0x%08x", deviceClass);
        return NULL;
      }

      InputModule *module = factory->create (device);	

      set (module->getId (), module);

      return module;
    }

    static InputModule *getModule (uint32_t moduleId, uint32_t deviceClass, const char *keyword = NULL) {
      if (moduleId == MODULE_ID_INVALID && deviceClass == INPUT_DEVICE_CLASS_UNKNOWN) {
        return NULL;
      }

      ModuleManager *mm = ModuleManager::instance ();
      if (moduleId != MODULE_ID_INVALID) {
        return mm->get (moduleId);
      }

      return mm->getModuleByType (deviceClass, keyword);
    }
#if 0
    /* create new InputModule by factory, and stores in an id/module map */
    static InputModule *createModule (uint32_t deviceClass) {
      ModuleFactory *factory = NULL;

      map<uint32_t, ModuleFactory *>::iterator iter;
      for (iter = _factories.begin (); iter != _factories.end (); iter++) {
        if (iter->first & deviceClass) {
          factory = iter->second;
          break;
        }
      }

      if (factory == NULL)
        return NULL;

      AbstractInputDev *device = InputDevFactory::getDevice (deviceClass);
      if (device == NULL)
        return NULL;
      InputModule *module = factory->create (device);	
      _modules[module->getId ()] = module;

      return module;
    }

    /* lookup InputModule by moduleId or deviceClass */
    static InputModule *getModule (uint32_t moduleId, uint32_t deviceClass) {
      if (moduleId == MODULE_ID_INVALID && deviceClass == INPUT_DEVICE_CLASS_UNKNOWN) {
        return NULL;
      }
      if (moduleId != MODULE_ID_INVALID) {
        if (_modules.count (moduleId)) {
          return _modules[moduleId];
        } else 
          return NULL;
      }

      map<ModuleId, InputModule *>::iterator iter;
      for (iter = _modules.begin (); iter != _modules.end (); iter++) {
        if (iter->second->isDeviceSupport ((DeviceType)deviceClass)) {
          return iter->second;
        }
      }
      return createModule (deviceClass);
    }
#endif

    static void registerModule (ModuleFactory &factory) {
      _factories[factory.getModuleClass()] = &factory;
      InputDevFactory::regInputDevFactory (factory.getDevFactory ());
    }

    static int initialize ();
  protected:

    /* deviceClass to factory map */
    static map<uint32_t, ModuleFactory *>  _factories;

    /* moduleId to module map */
    //static map<ModuleId, InputModule *>  _modules;
};

#endif
