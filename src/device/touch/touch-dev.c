#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>

#include <uis/device/touch-dev.h>
#include <uis/device/ev-dev.h>
#include <uis/debug.h>

#define TOUCH_DEBUG

#define USE_PROTOCOL_B 0

/* TODO: 
 * 1, multiple client support, store contacts in TSD
 * 2, protocol A/B support
 */
typedef struct {
	uint32_t ids[SLOTS_MAX];
	MT_PROTOCOL proto;

	struct input_absinfo x_absinfo;
	struct input_absinfo y_absinfo;
	
	IPoint screen_size;
} TSD;

typedef struct {
	BaseEvDev devh;
} TouchDev;

static MT_PROTOCOL mt_get_proto (EvDev *bed) {
	return ((TSD *)bed->data)->proto;
}

static int slot_get (EvDev *evdev, uint32_t id, int alloc) {
	int slot = -1 ;

	TSD *tsd = evdev->data;
	for (int i = 0; i < SLOTS_MAX; i++) {
		if (tsd->ids[i] == (uint32_t)id) {
			LOG_DEBUG ("found id %u at slot %d", id, i);
			return i;
		} else if ((slot == -1) && alloc && (tsd->ids[i] == SLOT_ID_INVALID)) {
			LOG_DEBUG ("found free slot %d", i);
			slot = i;
/*
		} else {
			LOG_DEBUG ("occupied slot %d:0x%08x", i, tsd->ids[i]);
*/
		}
	}
	if (slot != -1) {
		tsd->ids[slot] = id;
	} else {
		LOG_WARN ("get slot for id %d failed, alloc %d", id, alloc);
	}
	return slot;
}

static int slot_free (EvDev *evdev, int slot) {
	TSD *tsd = evdev->data;

	if (slot < 0 || slot >= SLOTS_MAX) {
		LOG_DEBUG ("invalid slot %d", slot);
		return -1;
	}

	tsd->ids[slot] = SLOT_ID_INVALID;
	return 0;
}

static int slot_free_id (EvDev *evdev, uint32_t id) {
	TSD *tsd = evdev->data;

	for (int i = 0; i < SLOTS_MAX; i++) {
		if (tsd->ids[i] == (uint32_t)id) {
			tsd->ids[i] = SLOT_ID_INVALID;
			return 0;
		}
	}
	return -1;
}

static void slot_free_all (EvDev *evdev) {
	TSD *tsd = evdev->data;
	for (int i = 0; i < SLOTS_MAX; i++) {
		tsd->ids[i] = SLOT_ID_INVALID;
/*		LOG_DEBUG ("slot %d:0x%08x", i, tsd->ids[i]); */
	}
}

static int mt_slot (EvDev *evdev, uint32_t id) {
	struct input_event ev;

	TSD *tsd = evdev->data;

	int slot =  slot_get (evdev, id, 1);
	if (slot == -1) {
		return -1;
	}
#if USE_PROTOCOL_B
	if (mt_get_proto (evdev) == MT_PROTOCOL_B) {
		memset (&ev, 0, sizeof (ev));

		gettimeofday(&ev.time,0);

		ev.type = EV_ABS;
		ev.code = ABS_MT_SLOT;
		ev.value = slot;

		if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
			LOG_ERROR ("write ABS_MT_SLOT, event failed, %s\n", strerror(errno));
			return -1;
		}
	}
#endif
#if 1
	memset (&ev, 0, sizeof (ev));

	gettimeofday(&ev.time,0);

	ev.type = EV_ABS;
	ev.code = ABS_MT_TRACKING_ID;
	ev.value = id;
	//ev.value = slot;

	if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
		LOG_ERROR ("write ABS_MT_TRACKING_ID, event failed, %s\n", strerror(errno));
		return -1;
	}
#endif

	LOG_DEBUG ("switched to slot %d, id %u", slot, id);
	return slot;
}

static int mt_xy (EvDev *evdev, int x, int y) {
	struct input_event ev;

	TSD *tsd = (TSD *)evdev->data;

	LOG_DEBUG ("touching (%d,%d)", x, y);
	memset (&ev, 0, sizeof (ev));

	gettimeofday(&ev.time,0);

	ev.type = EV_ABS;
	ev.code = ABS_MT_POSITION_X;
	ev.value = x;

	if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
		LOG_ERROR ("write ABS_MT_POSITION_Y event failed, %s\n", strerror(errno));
		return -1;
	}

	memset (&ev, 0, sizeof (ev));

	gettimeofday(&ev.time,0);

	ev.type = EV_ABS;
	ev.code = ABS_MT_POSITION_Y;
	ev.value = y;

	if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
		LOG_ERROR ("write ABS_MT_POSITION_Y event failed, %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

static int mt_sync_report (EvDev *evdev) {
	struct input_event ev;

//	LOG_DEBUG ("sending sync_report event");
	memset (&ev, 0, sizeof (ev));

	gettimeofday(&ev.time,0);

	ev.type = EV_SYN;
	ev.code = SYN_REPORT;

	if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
		LOG_ERROR ("write SYNC_REPORT event failed, %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

static int mt_sync_mt_report (EvDev *evdev) {
	struct input_event ev;

	memset (&ev, 0, sizeof (ev));

	gettimeofday(&ev.time,0);

	ev.type = EV_SYN;
	ev.code = SYN_MT_REPORT;

	if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
		LOG_ERROR ("write SYNC_MT_REPORT event failed, %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

static int mt_release_slot (EvDev *evdev, int slot) {
	struct input_event ev;

	if (mt_get_proto (evdev) == MT_PROTOCOL_B) {
		LOG_DEBUG ("release slot %d", slot);

		memset (&ev, 0, sizeof (ev));

		gettimeofday(&ev.time,0);

		ev.type = EV_ABS;
		ev.code = ABS_MT_TRACKING_ID;
		ev.value = -1;

		if (ev_dev_write(evdev, &ev, sizeof(ev)) < 0) {
			LOG_ERROR ("write SYNC_REPORT event failed, %s\n", strerror(errno));
			return -1;
		}
	}

	return 0;
}

static int mt_release (EvDev *evdev, uint32_t id) {
	int slot;

	slot = slot_get (evdev, id, 0);
	if (slot == -1) {
		return -1;
	}

	return mt_release_slot (evdev, slot);
}

static int get_abs_info (EvDev *evdev, uint32_t axis, struct input_absinfo *info) {
	memset (info, 0, sizeof (struct input_absinfo));
	if(ev_dev_ioctl(evdev, EVIOCGABS(axis), info) != 0) {
		LOG_ERROR ("cannot get axis %u info: %s\n", axis, strerror(errno));
		return -1;
	}

	if (info->maximum) {
		LOG_INFO  ("touchscreen axis %u range: [%u, %u]", axis, info->minimum, info->maximum);
	} else {
		LOG_INFO ("touchscreen axis %u has range: using emulator mode", axis);
	}

	return 0;
}

BaseEvDev *touch_dev_new (EvDev *evdev) {
	if ((ev_dev_get_class (evdev) & INPUT_DEVICE_CLASS_TOUCHSCREEN) == 0) {
		return NULL;
	}

	TouchDev *bed = (TouchDev *)malloc (sizeof (TouchDev));
	bed->devh.evdev = ev_dev_ref (evdev);
	
	if (evdev->data == NULL) {
		TSD *tsd = (TSD *) malloc (sizeof (TSD));
		memset (tsd, 0, sizeof (TSD));

		evdev->data = tsd;

		slot_free_all (evdev);

		LOG_DEBUG ("initialized structure");
		tsd->proto = MT_PROTOCOL_A;
#if USE_PROTOCOL_B
		if (evdev->classes & INPUT_DEVICE_CLASS_TOUCHSCREEN_MT) {
			if (test_bit(ABS_MT_SLOT, evdev->abs_bitmask)) {
				tsd->proto = MT_PROTOCOL_B;
			}
		}
#endif
		get_abs_info (evdev, ABS_X, &tsd->x_absinfo);
		tsd->screen_size.x = tsd->x_absinfo.maximum - tsd->x_absinfo.minimum;

		get_abs_info (evdev, ABS_Y, &tsd->y_absinfo);
		tsd->screen_size.y = tsd->y_absinfo.maximum - tsd->y_absinfo.minimum;

		LOG_DEBUG ("touch screen size %dx%d", tsd->screen_size.x, tsd->screen_size.y);
	}

	return (BaseEvDev *)bed;
}

int touch_dev_ctrl (BaseEvDev *bed, int cmd, ...) {
	va_list ap;

	TSD *tsd = bed->evdev->data;

	va_start (ap, cmd);

	switch (cmd) {
		case TOUCHDEV_RESOLUTION_GET: 
			{
				int *x, *y;
				x = va_arg (ap, int *);
				y = va_arg (ap, int *);
				*x = tsd->x_absinfo.maximum - tsd->x_absinfo.minimum;
				*y = tsd->y_absinfo.maximum - tsd->y_absinfo.minimum;
			}
			break;

		case TOUCHDEV_SCREEN_RESOLUTION_SET:  /* deprecated */
			{
				int x, y;
				x = va_arg (ap, int);
				y = va_arg (ap, int);

				if (x < 64 || y < 64) {
					va_end (ap);
					LOG_ERROR ("invalid screen size %dx%d", x, y);
					return -1;
				}
				tsd->screen_size.x = x;
				tsd->screen_size.y = y;
			}
			break;
/*
		case SCREEN_INFO_GET:
			screen = va_arg (ap, ScreenInfo *);
			memcpy (screen, &dev->size, sizeof (ScreenInfo));
			break;
		case SCREEN_INFO_SET:
			screen = va_arg (ap, ScreenInfo *);
			memcpy (&dev->size, screen, sizeof (ScreenInfo));
			break;
*/
		case TOUCHDEV_PROTOCOL_GET:
			{
				MT_PROTOCOL *proto;	
				proto = va_arg (ap, MT_PROTOCOL *);
				*proto = tsd->proto;
				va_end (ap);
			}
			break;
		default:
			LOG_ERROR ("invalid command %d", cmd);
			return -1;
	}
	va_end (ap);


	return 0;
}

static int touch_dev_write_data (BaseEvDev *bed, int action, const TouchData *data) {
	int slot;
	int result = -1;

	LOG_DEBUG ("write touch data: id %u, action %d, x %d, y %d",
			data->trackingId, action, data->x, data->y);

	EvDev *evdev = bed->evdev;
	if (action == TOUCH_ACTION_DOWN) {
		slot_free_all (evdev);
	}

	slot = mt_slot (evdev, data->trackingId);
	if (slot== -1) {
		LOG_ERROR ("cannot switch to slot id 0x%8x", data->trackingId);
		return -1;
	}

	switch (action) {
		case TOUCH_ACTION_DOWN:
			/* fall-through */
		case TOUCH_ACTION_MOVE:
			result = mt_xy (evdev, data->x, data->y);
			break;
		case TOUCH_ACTION_UP:
			result = mt_release_slot (evdev, slot);
			slot_free_all (evdev);
			break;
		case TOUCH_ACTION_CANCEL:
			result = mt_release_slot (evdev, slot);
			break;
		case TOUCH_ACTION_POINTER_DOWN:
			result = mt_xy (evdev, data->x, data->y);
			break;
		case TOUCH_ACTION_POINTER_UP:
			if (mt_get_proto (evdev) == MT_PROTOCOL_B) {
				result = mt_release_slot (evdev, slot);
			} else {
				/* ignore pointer up event for protocol A */
				return -1;
			}
			break;
		default:
			LOG_ERROR ("invalid action %d", action);
	}

	if (result == -1) {
		LOG_ERROR ("action %d failed", action);
	}
	return result;
}

int touch_dev_write (BaseEvDev *bed, int action, const EvData *evdata) {
	int result;

	result = touch_dev_write_data (bed, action, (const TouchData *)evdata);
	if (result == 0)
		mt_sync_mt_report (bed->evdev);

	return result;
}

int touch_dev_write_n (BaseEvDev *bed, int action, const EvData *evdata, int contacts) {
	int result = 0;

	const TouchData *data = (TouchData *)evdata;

	if (((TSD *)bed->evdev->data)->proto == MT_PROTOCOL_A)
		slot_free_all (bed->evdev);

	LOG_DEBUG ("write %d contacts, action 0x%08x", contacts, action);
	for (int i = 0; i < contacts && result == 0; i++) {
		result = touch_dev_write (bed, action, (EvData *)data++);
	}
	if (contacts == 0) {
		mt_sync_mt_report (bed->evdev);
		slot_free_all (bed->evdev);
	}
	if (result == 0)
		mt_sync_report (bed->evdev);

	return result;
}

void touch_dev_free (BaseEvDev *bed) {
	int result = 0;

	ev_dev_unref (bed->evdev);

	free (bed);
}

void touch_dev_touch(BaseEvDev *bed, int down, int x, int y)
{
#if 0
	struct input_event ev;

	memset(&ev, 0, sizeof(ev));

	// Then send a BTN_TOUCH
	gettimeofday(&ev.time,0);
	ev.type = EV_KEY;
	ev.code = BTN_TOUCH;
	ev.value = down;
	if(write(dev->fd, &ev, sizeof(ev)) < 0) {
		LOG_ERROR ("write event failed, %s\n", strerror(errno));
	}

	// Then send the X
	gettimeofday(&ev.time,0);
	ev.type = EV_ABS;
	ev.code = ABS_X;
	ev.value = x;
	if(write(dev->fd, &ev, sizeof(ev)) < 0)
	{
		LOG_ERROR ("write event failed, %s\n", strerror(errno));
	}

	// Then send the Y
	gettimeofday(&ev.time,0);
	ev.type = EV_ABS;
	ev.code = ABS_Y;
	ev.value = y;
	if(write(dev->fd, &ev, sizeof(ev)) < 0)
	{
		LOG_ERROR ("write event failed, %s\n", strerror(errno));
	}

	// Finally send the SYN
	gettimeofday(&ev.time,0);
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;
	if(write(dev->fd, &ev, sizeof(ev)) < 0)
	{
		LOG_ERROR ("write event failed, %s\n", strerror(errno));
	}
#else
#define VNC_MOUSE_DEV_ID 0x0e0e0e0e
	TouchData data;

	touch_data_init (&data, VNC_MOUSE_DEV_ID, down ? TOUCH_ACTION_DOWN : TOUCH_ACTION_UP);
	touch_data_set_xy (&data, x, y);

	if (down) slot_free_all (bed->evdev);
	touch_dev_write_n (bed, down ? TOUCH_ACTION_DOWN : TOUCH_ACTION_UP, (EvData *)&data, 1);
	if (!down) slot_free_all (bed->evdev);
#endif
}
