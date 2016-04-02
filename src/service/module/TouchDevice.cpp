#include <uis/device/ev-dev.h>
#include <uis/device/touch-dev.h>
#include "TouchDevice.h"

TouchDevice::TouchDevice (InputDev *dev):
	AbstractInputDev (dev),
	_width (0),
	_height (0)
{
	touch_dev_ctrl (getBED(), TOUCHDEV_RESOLUTION_GET, &_width, &_height);
	touch_dev_ctrl (getBED(), TOUCHDEV_PROTOCOL_GET, &_proto);
	LOG_DEBUG ("touch device protocol %d, width %d, height %d", _proto, _width, _height);
}

int TouchDevice::ctrl (int cmd, ...) {
	va_list ap;

	va_start (ap, cmd);

	switch (cmd) {
		case TOUCHDEV_RESOLUTION_GET: 
			{
				int *x, *y;
				x = va_arg (ap, int *);
				y = va_arg (ap, int *);
				*x = _width;
				*y = _height;
			}
			break;

		default:
			LOG_ERROR ("invalid command %d", cmd);
			return -1;
	}
	va_end (ap);
	return 0;
}

int TouchDevice::write (int type, const EvData *data, int count = 1) {
	return _inputdev->ops->write_n (getBED (), type, data, count);
}

static EvDevOps _touchOps = {
	touch_dev_new,
	touch_dev_ctrl,
	touch_dev_write,
	touch_dev_write_n,
	touch_dev_free,
};

AbstractInputDev *TouchFactory::internal_create (EvDev *evdev) {
	InputDev *dev = (InputDev *)malloc (sizeof (InputDev));
	dev->ops = &_touchOps;
	dev->bed = dev->ops->devnew (evdev);

	return new TouchDevice (dev);
}
