#ifndef __TOUCH_DEV_H__
#define __TOUCH_DEV_H__

#include <stdint.h>
#include <linux/input.h>

#include <uis/device/ev-dev.h>
#include <uis/device/touch-data.h>

#define SLOTS_MAX 10
#define SLOT_ID_INVALID 0xffffffff

typedef enum {MT_PROTOCOL_INVALID = 0, MT_PROTOCOL_A = 1, MT_PROTOCOL_B = 2} MT_PROTOCOL;
typedef enum {TOUCHDEV_RESOLUTION_GET = 0, TOUCHDEV_SCREEN_RESOLUTION_SET, TOUCHDEV_PROTOCOL_GET} TOUCHDEV_CTRL_CMD;

typedef struct {
	int32_t x;
	int32_t y;
} IPoint;

#ifdef __cplusplus
extern "C" {
#endif

const char * touch_dev_probe ();
BaseEvDev* touch_dev_new (EvDev *);

int touch_dev_ctrl (BaseEvDev *dev, int cmd, ...);

int touch_dev_write (BaseEvDev *dev, int type, const EvData *data);
int touch_dev_write_n (BaseEvDev *dev, int type, const EvData *data, int count);
void touch_dev_touch(BaseEvDev *dev, int down, int x, int y);

//int touch_dev_close (EvDev *dev);

void touch_dev_free (BaseEvDev *dev);

#ifdef __cplusplus
}
#endif
#endif
