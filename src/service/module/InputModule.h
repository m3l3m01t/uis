#ifndef _INPUT_MODULE_H
#define _INPUT_MODULE_H

#include <stdint.h>

#include <uis/device/ev-dev.h>
#include "InputData.h"
#include "EventSource.h"
#include "AbstractInputDev.h"

using namespace std;

class InputModule {
public:

	virtual bool isDeviceSupport (DeviceType type) {
		return ((_type & type) == type);
	};

	ModuleId getId () { return _id; };
	const char *getName () { return _name; };

	virtual int attach (EventSource *source) {
		ModulePrivData *data = source->get (_id);
		if (data == NULL) {
			source->set (getId(), allocPrivData ());
		}

		return 0;
	};

	virtual void detach (EventSource *source)  {
		ModulePrivData * data = source->erase (_id);
		if (data) {
			freePrivData (data);
		}
	}

	virtual int set (EventSource *source, int cmd, BaseInputData *data) = 0;   /* configurate */
	virtual int write (EventSource *source, int cmd, BaseInputData *data) = 0; /* write data */

protected:
	~InputModule () {};
	InputModule (AbstractInputDev *dev, DeviceType type) :  _type (type), _dev (dev) {
		_id = (uint32_t)this;	
	};
	
	virtual ModulePrivData *allocPrivData () = 0;
	virtual void freePrivData (ModulePrivData *) = 0;

protected:
	char _name[64];
	
	DeviceType _type;
	AbstractInputDev *_dev;	
private:
	ModuleId _id;  // unique module id
};

class ModuleFactory  {
public:
	ModuleFactory (uint32_t clazz) : _moduleClass(clazz) {};
	virtual ~ModuleFactory () {};

	InputModule *create (AbstractInputDev* dev) {
		return internal_create (dev);
	}

	uint32_t getModuleClass () { return _moduleClass;};
	virtual InputDevFactory &getDevFactory () = 0;

protected:
	virtual InputModule *internal_create (AbstractInputDev *) = 0;

	uint32_t _moduleClass;
};

#endif /* INPUT_MOD_H */
