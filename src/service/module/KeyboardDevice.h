#ifndef KEYBOARD_DEVICE_H
#define KEYBOARD_DEVICE_H

#include <uis/device/ev-dev.h>

#include <AbstractInputDev.h>

class KeyboardDevice : public AbstractInputDev {
public:
	KeyboardDevice (InputDev *dev) :AbstractInputDev (dev) {};

	int ctrl (int cmd, ...);
	int write (int type, const EvData *data, int count);

	friend class KeyboardFactory;
};

class KeyboardFactory: public InputDevFactory {
public:
	KeyboardFactory () : InputDevFactory (INPUT_DEVICE_CLASS_KEYBOARD) {
		
	};

	AbstractInputDev *internal_create (EvDev *dev);

protected:
	static KeyboardFactory _kbdFactory;
};

#endif
