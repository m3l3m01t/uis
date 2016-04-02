#ifndef _UIC_C_H_
#define _UIC_C_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <uis/common/InputData.h>

#define UIC_TRANSPORT_DBUS 0
#define UIC_TRANSPORT_TCP  1
#define UIC_TRANSPORT_SDIO 2

struct uic_transport {
	void *self;
};

int uic_transport_init (struct uic_transport *trans, int type, void *opaque);
void uic_transport_destroy (struct uic_transport *trans);

struct uic_peer {
	void *self;
	int ref;
};

struct uic_dev {
	void *self;
	struct uic_peer *peer;
};

#define uic_touch_dev uic_dev
#define uic_kbd_dev uic_dev

int uic_peer_init (struct uic_peer *peer, struct uic_transport *trans, void *opaque);
void uic_peer_unref (struct uic_peer *peer);

int uic_touch_dev_init (struct uic_dev *dev, struct uic_peer *peer);
//int uic_touch_dev_open (struct uic_dev *dev, const char *keyword);
//int uic_touch_dev_close (struct uic_dev *dev);

int uic_touch_dev_touch (struct uic_dev *dev, int32_t x, int32_t y, int down);
int uic_touch_dev_set_resolution (struct uic_dev *dev, uint32_t w, uint32_t h);

//int uic_touch_dev_write (struct uic_dev *dev, int cmd, const BaseInputData *data);

//void uic_touch_dev_destroy (struct uic_dev *dev);


int uic_kbd_dev_init (struct uic_dev *dev, struct uic_peer *peer);

//int uic_kbd_dev_open (struct uic_dev *dev, const char *keyword);

int uic_kbd_dev_send_key (struct uic_dev *dev, uint16_t sym, uint16_t scancode, int down);
//int uic_kbd_dev_write (struct uic_dev *dev, int cmd, const BaseInputData *data);

//int uic_kbd_dev_close (struct uic_dev *dev);

int uic_dev_open (struct uic_dev *dev, const char *keyword);

int uic_dev_write (struct uic_dev *dev,int cmd, const BaseInputData *data);

int uic_dev_close (struct uic_dev *dev);

void uic_dev_destroy (struct uic_dev *dev);

#define uic_touch_dev_open uic_dev_open
#define uic_touch_dev_write uic_dev_write
#define uic_touch_dev_close uic_dev_close
#define uic_touch_dev_destroy uic_dev_destroy

#define uic_kbd_dev_open uic_dev_open
#define uic_kbd_dev_write uic_dev_write
#define uic_kbd_dev_close uic_dev_close
#define uic_kbd_dev_destroy uic_dev_destroy

#ifdef __cplusplus
}
#endif

#endif
