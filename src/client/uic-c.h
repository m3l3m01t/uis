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


int uic_peer_init (struct uic_peer *peer, struct uic_transport *trans, void *opaque);
void uic_peer_unref (struct uic_peer *peer);

struct uic_touch_dev {
	void *self;
	struct uic_peer *peer;
};

int uic_touch_dev_init (struct uic_touch_dev *dev, struct uic_peer *peer);
int uic_touch_dev_open (struct uic_touch_dev *dev);
int uic_touch_dev_close (struct uic_touch_dev *dev);

int uic_touch_dev_touch (struct uic_touch_dev *dev, int32_t x, int32_t y, int down);

int uic_touch_dev_write (struct uic_touch_dev *dev, int cmd, const BaseInputData *data);

void uic_touch_dev_destroy (struct uic_touch_dev *dev);

struct uic_kbd_dev {
	void *self;
	struct uic_peer *peer;
};

int uic_kbd_dev_init (struct uic_kbd_dev *dev, struct uic_peer *peer);

int uic_kbd_dev_open (struct uic_kbd_dev *dev);

int uic_kbd_dev_send_key (struct uic_kbd_dev *dev, uint16_t sym, uint16_t scancode, int down);
int uic_kbd_dev_write (struct uic_kbd_dev *dev, int cmd, const BaseInputData *data);

int uic_kbd_dev_close (struct uic_kbd_dev *dev);

#ifdef __cplusplus
}
#endif

#endif
