#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#include <uis/device/ev-dev.h>

#include <uis/debug.h>

static int all_zero(const uint8_t* array, uint32_t startIndex, uint32_t endIndex) {
    const uint8_t* end = array + endIndex;
    array += startIndex;
    while (array != end) {
        if (*(array++) != 0) {
            return 0;
        }
    }
    return 1;
}


int ev_dev_open (const char *name, EvDev *dev) {
	int fd = -1;
	memset (dev, 0, sizeof (EvDev));

	fd = open (name, O_RDWR);

	if (fd == -1) {
		LOG_ERROR ("cannot open %s: %s", name, strerror (errno));
		return -1;
	}

	if(ioctl(fd, EVIOCGVERSION, &dev->version)) {
		LOG_ERROR("could not get driver version for %s, %s\n", name, strerror(errno));
		close (fd);
		return -1;
	}

	if(ioctl(fd, EVIOCGID, &dev->id)) {
		LOG_ERROR("could not get driver id for %s, %s\n", name, strerror(errno));
		close (fd);
		return -1;
	}

	if(ioctl(fd, EVIOCGNAME(sizeof(dev->devname) - 1), &dev->devname) < 1) {
		LOG_ERROR("could not get device name for %s, %s\n", name, strerror(errno));
	}

	memset (dev->abs_bitmask, 0, sizeof(dev->abs_bitmask));
    	if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(dev->abs_bitmask)), dev->abs_bitmask) < 0) {
		LOG_ERROR ("cannot get abs bitmask: %s", strerror (errno));
	}

	memset (dev->key_bitmask, 0, sizeof(dev->key_bitmask));
    	if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(dev->key_bitmask)), dev->key_bitmask) < 0) {
		LOG_ERROR ("cannot get key bitmask: %s", strerror (errno));
	}

	memset (dev->rel_bitmask, 0, sizeof(dev->rel_bitmask));
    	if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(dev->rel_bitmask)), dev->rel_bitmask) < 0) {
		LOG_ERROR ("cannot get rel bitmask: %s", strerror (errno));
	}

	dev->fd = fd;
	strncpy (dev->name, name, sizeof (dev->name) - 1);

	if (test_bit(ABS_MT_POSITION_X, dev->abs_bitmask)
                && test_bit(ABS_MT_POSITION_Y, dev->abs_bitmask)) {
            dev->classes |= INPUT_DEVICE_CLASS_TOUCHSCREEN | INPUT_DEVICE_CLASS_TOUCHSCREEN_MT;
	} else if (test_bit(BTN_TOUCH, dev->key_bitmask)
                && test_bit(ABS_X, dev->abs_bitmask) && test_bit(ABS_Y, dev->abs_bitmask)) {
            dev->classes |= INPUT_DEVICE_CLASS_TOUCHSCREEN;
        }

	// See if this is a trackball (or mouse).
	if (test_bit(BTN_MOUSE, dev->key_bitmask)) {
		if (test_bit(REL_X, dev->rel_bitmask) && test_bit(REL_Y, dev->rel_bitmask)) {
			dev->classes |= INPUT_DEVICE_CLASS_TRACKBALL;
		}
	}

        // See if this is a keyboard.  Ignore everything in the button range except for
        // gamepads which are also considered keyboards.
        if (!(all_zero(dev->key_bitmask, 0, sizeof_bit_array(BTN_MISC))
                && all_zero(dev->key_bitmask, sizeof_bit_array(BTN_GAMEPAD),
                        sizeof_bit_array(BTN_DIGI))
                && all_zero(dev->key_bitmask, sizeof_bit_array(KEY_OK),
                        sizeof_bit_array(KEY_MAX + 1)))) {
            dev->classes |= INPUT_DEVICE_CLASS_KEYBOARD;
        }


	dev->ref = 1;

	LOG_INFO ("device %s: version %d, name %s, class 0x%08x", name, 
			dev->version, dev->name, dev->classes);
	LOG_INFO ("vendor %04hx product %04hx version %04hx",
			dev->id.vendor, dev->id.product,
			dev->id.version);
	return 0;
}

/**
 * ev_dev_ref
 *
 * increase device refence count
 * 
 * return: refernce count
 */
EvDev *ev_dev_ref (EvDev *dev) {
	++dev->ref;
	return dev;
}

/**
 * ev_dev_unref
 *
 * decrease device refence count, close device on reaches 0
 * 
 * return: refernce count
 */
int ev_dev_unref (EvDev *dev) {
	if (dev == NULL)
		return -1;
	if (--dev->ref <= 0) {;
		close (dev->fd);

		return 0;
	}
	return dev->ref;
}

DeviceType ev_dev_get_class (const EvDev *dev) {
	return (DeviceType) dev->classes;
}

const char *ev_dev_get_name (const EvDev *dev) {
	return dev->devname;
}

void ev_dev_get_id (const EvDev *dev, struct input_id *id) {
	memcpy (id, &dev->id, sizeof (struct input_id));
}

int ev_dev_write (const EvDev *dev, const struct input_event *ev, int size) {
	int result = write(dev->fd, ev, size);
	if (result == -1) {
		LOG_ERROR ("write event failed, %s\n", strerror(errno));
	}

	return result;
}

