#ifndef TOUCH_DEVICE_H
#define TOUCH_DEVICE_H

#include <uis/device/ev-dev.h>
#include <uis/device/touch-dev.h>

#include "AbstractInputDev.h"

class TouchDevice : public AbstractInputDev {
public:
	TouchDevice (InputDev *dev);

	int ctrl (int cmd, ...);
	int write (int, const EvData *data, int count);

	bool isMultitouch () {
		return (getDevClass () & INPUT_DEVICE_CLASS_TOUCHSCREEN_MT);
	}

	MT_PROTOCOL getProtocol () {
		return _proto;
	}

	int getWidth () {
		return _width;
	}

	int getHeight () {
		return _height;
	}

	friend class TouchFactory;

protected:
	int _width, _height;
	MT_PROTOCOL _proto;
};

class TouchFactory: public InputDevFactory {
public:
	TouchFactory () : InputDevFactory ((DeviceType) (INPUT_DEVICE_CLASS_TOUCHSCREEN |INPUT_DEVICE_CLASS_TOUCHSCREEN_MT)) {
		
	};

	AbstractInputDev *internal_create (EvDev *dev);
protected:
};

#endif
