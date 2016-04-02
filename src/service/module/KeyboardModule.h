#ifndef KEYBOARD_MOD_H
#define KEYBOARD_MOD_H

#include <map>
#include "InputModule.h"
#include "KeyboardDevice.h"

using namespace std;

class KbdPrivData : public ModulePrivData {
public:
	void setScanCode (uint32_t symcode, uint16_t scancode) {
		_codeMap[symcode] = scancode;
	}
	
	uint16_t getScanCode (uint32_t symcode) {
		if (_codeMap.count (symcode)) {
			return _codeMap[symcode];
		} else {
			return 0;
		}
	}
protected:
	map<uint32_t, uint16_t> _codeMap;   // symcode to scancode map
	
	friend class KeyboardModule;
};

//typedef struct {
//        BaseInputData head;
//        uint8_t data[0];
//} KeyPressData;

class KeyboardModule : public InputModule {
public:
	virtual ~KeyboardModule () {};

	ModulePrivData *allocPrivData () {
		ModulePrivData* data = new KbdPrivData ();

		return data;
	};

	void freePrivData (ModulePrivData *data) {
		if (data) {
			delete data;
		}
	};

	virtual int set (EventSource *source, int cmd, BaseInputData *data);

	virtual int write (EventSource *source, int cmd, BaseInputData *data);

protected:
	KeyboardModule (AbstractInputDev *dev, DeviceType moduleClass) : InputModule(dev, moduleClass) {};
	friend class KeyboardModuleFactory; 
private:
	int injectKeyEvent (KbdPrivData *privData, KeyInputData *data);
	void setKeymapData (KbdPrivData *privData, KeyInputData *data);
};

class KeyboardModuleFactory : public ModuleFactory {
public:
	KeyboardModuleFactory () :
		ModuleFactory (INPUT_DEVICE_CLASS_KEYBOARD  | INPUT_DEVICE_CLASS_ALPHAKEY |INPUT_DEVICE_CLASS_DPAD) {};

	InputDevFactory &getDevFactory () {
		return _kbdFactory;
	}

protected:
	InputModule * internal_create (AbstractInputDev *dev) {
		return new KeyboardModule (dev, (DeviceType)_moduleClass);
	}

	static KeyboardFactory _kbdFactory;
};

#endif /* KEYBOARD_MOD_H */
