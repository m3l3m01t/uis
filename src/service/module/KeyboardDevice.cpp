#include <uis/device/ev-dev.h>
#include <uis/device/kbd-dev.h>
#include "KeyboardDevice.h"

KeyboardFactory _kbdFactory;

int KeyboardDevice::ctrl (int cmd, ...) {
/* nothing to do for now */
	return 0;
}

int KeyboardDevice::write (int type, const EvData *data, int count = 1) {
	_inputdev->ops->write_n (getBED (), type, data, count);
	return 0;
}

static EvDevOps _kbdOps = {
	kbd_dev_new,
	kbd_dev_ctrl,
	kbd_dev_write,
	kbd_dev_write_n,
	kbd_dev_free,
};

AbstractInputDev *KeyboardFactory::internal_create (EvDev *evdev) {
	InputDev *dev = (InputDev *)malloc (sizeof (InputDev));
	dev->ops = &_kbdOps;
	dev->bed = dev->ops->devnew (evdev);

	return new KeyboardDevice (dev);
}
