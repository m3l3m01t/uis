#ifndef ABSTRACT_INPUT_DEV_H
#define ABSTRACT_INPUT_DEV_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <stdlib.h>

#include <uis/debug.h>
#include <uis/device/ev-dev.h>

class AbstractInputDev {
protected:
	AbstractInputDev (InputDev *dev) :_inputdev (dev) {
		LOG_DEBUG ("dev %p, bed %p", dev, dev->bed);
	};

public:
	virtual ~AbstractInputDev () {
		_inputdev->ops->free (_inputdev->bed);
		free (_inputdev);
	}

	virtual int ctrl (int cmd, ...) = 0;
	virtual int write (int type, const EvData *data, int count) = 0;

	DeviceType getDevClass () {
		return ev_dev_get_class (getEvDev());
	};

	const char *getDeviceName () {
		return ev_dev_get_name (getEvDev());
	}

	const char *getDeviceId () {
		return ev_dev_get_id (getEvDev());
	}

protected:
	BaseEvDev *getBED () {
		return _inputdev->bed;
	}

	EvDev *getEvDev () {
		return _inputdev->bed->evdev;
	}
protected:
	InputDev *_inputdev;
};

#include <map>

using namespace std;

class InputDevFactory {
public:
	InputDevFactory  (DeviceType t) :_type (t) {}
	virtual ~InputDevFactory () {}

	DeviceType getType () {return _type;}

	static AbstractInputDev *create (EvDev *dev) {
		DeviceType type = (DeviceType)dev->classes;
		if (_factories.count (type)) {
			return _factories[type]->internal_create (dev);
		} else {
			LOG_ERROR ("cannot find device factory by type 0x%08x", (uint32_t)type);
			return NULL;
		}
	}
	static AbstractInputDev *getDevice (uint32_t deviceClass) {
		if (!_scaned) {
			if (scanDevices () <= 0) {
				return NULL;
			}
			_scaned = true;
		}

		map<DeviceType, EvDev *>::iterator iter;

		LOG_DEBUG ("looking for EvDev supports class: 0x%08x", deviceClass);
		for (iter = _devices.begin (); iter != _devices.end (); iter ++) {
			LOG_DEBUG ("checking deviceClass 0x%08x", iter->first);
			if ((iter->first & deviceClass) == deviceClass) {
				/* one EvDev represents an event? device, shared by all AbstractInputDev instance created from it */

				LOG_DEBUG ("creating AbstractInputDev from EvDev %p", iter->second);
				AbstractInputDev *dev = create (iter->second);
				if (dev != NULL) {
					return dev;
				}
			}
		}

		return NULL;
	}

	static void regInputDevFactory(InputDevFactory& factory) {
		_factories[factory.getType ()] = &factory;
		LOG_DEBUG ("register device 0x%08x at %p", factory.getType(), &factory);
	}

protected:
	static int scanDevices () {
		struct dirent *dr;

		DIR *dirp = opendir (INPUT_DIR);	
		
		if (dirp == NULL) {
			LOG_ERROR ("cannot opendir %s:%d\n", INPUT_DIR, errno);
			return -1;
		}

		while ((dr = readdir(dirp))) {
			if (dr->d_type != DT_CHR)
				continue;
			/* is device name started with event? */
			if (strncmp (dr->d_name, EVDEV_PREF, strlen (EVDEV_PREF) - 1) == 0) {
				char devname [256];
				snprintf (devname, sizeof (devname) - 1, "%s/%s", INPUT_DIR, dr->d_name);
	
				EvDev *evdev = (EvDev *)malloc (sizeof (EvDev));
				if (ev_dev_open (devname, evdev) == 0) {
					if (_devices.count ((DeviceType)evdev->classes)) {
						if (ev_dev_unref (evdev) == 0)
							free (evdev);
					} else {
						_devices[(DeviceType)evdev->classes] = evdev;
					}
				}
			}
		}

		closedir (dirp);

		return _devices.size ();
	}


	virtual AbstractInputDev *internal_create (EvDev *) = 0;
protected:

	static map<DeviceType, InputDevFactory*>  _factories;
	static map<DeviceType, EvDev *>  _devices;
	static bool _scaned;

	const static char *INPUT_DIR;
	const static char *EVDEV_PREF;

	DeviceType _type;
};

#endif // ABSTRACT_INPUT_DEV_H
