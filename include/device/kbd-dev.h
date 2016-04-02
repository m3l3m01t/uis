#ifndef __KBD_DEV_H__
#define __KBD_DEV_H__
#include <stdint.h>
#include <uis/device/ev-dev.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	char name[50];

} KbdLayout;

enum KEY_ACTIONS{KEY_ACTION_UP = 0, KEY_ACTION_DOWN} ;
typedef struct {
  uint32_t sym;     /* sym code defined in keysym.h */
  uint16_t scancode;     /* sym code defined in keysym.h */
  uint16_t value;   /* Up /Down */
} KeyData;

enum { KEYSYM_SET = 0 };

BaseEvDev *kbd_dev_new (EvDev *evdev);

//int kbd_dev_init (EvDev *evdev);
int kbd_dev_ctrl (BaseEvDev *evdev, int32_t cmd, ...);

int kbd_dev_write (BaseEvDev *evdev, int type, const EvData *data);
int kbd_dev_write_n (BaseEvDev *evdev, int type, const EvData *data, int count);

//int kbd_dev_close (EvDev *evdev);

void kbd_dev_free (BaseEvDev *evdev);


/* this function will be moved to client API */
uint16_t keycode_to_symcode(uint32_t keycode, int shift);
#ifdef __cplusplus
}
#endif

#endif  /*__KBD_DEV_H__*/
