#include "UICTouchDevice.h"
#include "UICKeyboardDev.h"
#ifdef HAVE_DBUS
#include "UICDBusTransport.h"
#endif
#include "UICTcpTransport.h"
#include "UICPeer.h"
#include "uic-c.h"

extern "C" {

int uic_transport_init (struct uic_transport *trans, int type, void *opaque) {
	int ret;
	switch (type) {
#ifdef HAVE_DBUS
		case UIC_TRANSPORT_DBUS:
			trans->self = new UICDBusTransport ();
			break;
#endif
		case UIC_TRANSPORT_TCP:
			trans->self = new UICTcpTransport ();
			break;
		case UIC_TRANSPORT_SDIO:
		default:
			return -1;
	}
	ret = ((UICTransport *)trans->self)->initialize ();
	if (ret != 0) {
		uic_transport_destroy (trans);
	}
	return ret;
}

void uic_transport_destroy (struct uic_transport *trans) {
	if (trans && trans->self) {
		delete (UICTransport *)trans->self;
		trans->self = NULL;
	}
}

int uic_peer_init (struct uic_peer *peer, struct uic_transport *trans, void *opaque) {
	peer->ref = 0;

	if (trans->self == NULL) {
		LOG_ERROR ("transport not initialized");
		return -1;
	}
	peer->self = ((UICTransport *)trans->self)->openPeer (opaque);
	if (peer->self) {
		peer->ref++;
		return 0;
	} else {
		return  -1;
	}
}

static UICPeer *uic_peer_self (struct uic_peer *peer) {
	return (UICPeer *)peer->self;
}

static void uic_peer_destroy (struct uic_peer *peer) {
	if (peer->self) {
		delete (UICPeer *)peer->self;
		peer->self = NULL;
	}
}

void uic_peer_unref (uic_peer *peer) {
	if (peer) {
		if (-- peer->ref <= 0) {
			/* TODO: close connection */
			uic_peer_destroy (peer);
		}
	}
}

static struct uic_peer *uic_peer_ref (uic_peer *peer) {
	if (peer && peer->self) {
		peer->ref ++;
		return peer;
	} else {
		return NULL;
	}
}

int uic_dev_open (struct uic_dev *dev, const char *keyword) {
	return ((UICDevice *)dev->self)->open (keyword);
}

int uic_dev_write (struct uic_dev *dev,int cmd, const BaseInputData *data) {
	return ((UICDevice *)dev->self)->write (cmd, (BaseInputData *)data);
}

int uic_dev_close (struct uic_dev *dev) {
	return ((UICDevice *)dev->self)->close ();
}

void uic_dev_destroy (struct uic_dev *dev) {
	if (dev->self) {
		delete ((UICDevice *)dev->self);
		dev->self = NULL;
	}
	if (dev->peer) {
		uic_peer_unref (dev->peer);
		dev->peer = NULL;
	}
}

/* keyboard device */
int uic_kbd_dev_init (struct uic_dev *dev, struct uic_peer *peer) {
	dev->self = new UICKeyboardDev ((UICPeer *)uic_peer_self (peer));
	dev->peer = uic_peer_ref (peer);

	return 0;
}

int uic_kbd_dev_send_key (struct uic_dev *dev, uint16_t sym, uint16_t scancode, int down) {
	int retval;
	KeyPressBody body = {0, 0, 0};
	UICDevice *kbd = ((UICDevice *)dev->self);

	body.symcode = sym;
	if (scancode) {
		body.scancode = scancode;
	}
	body.value = down ? 1 : 0;

	BaseInputData *data = BID_alloc (KBD_CMD_KEY_PRESS, 1, sizeof (body), &body);
	retval = kbd->write (UIS_CMD_INPUT, data);
	free (data);

	return retval;
}

#if 0
int uic_kbd_dev_close (struct uic_dev *dev) {
	return ((UICDevice *)dev->self)->close ();
}

#endif

/* touchscreen device */
int uic_touch_dev_init (struct uic_dev *dev, struct uic_peer *peer) {
	dev->self = new UICTouchDev ((UICPeer *)uic_peer_self (peer));
	dev->peer = uic_peer_ref (peer);

	return 0;
}

#if 0
int uic_touch_dev_open (struct uic_dev *dev, const char *keyword) {
	return ((UICDevice *)dev->self)->open (keyword);
}

int uic_touch_dev_write (struct uic_dev *dev,int cmd, const BaseInputData *data) {
	return ((UICDevice *)dev->self)->write (cmd, (BaseInputData *)data);
}

int uic_touch_dev_close (struct uic_dev *dev) {
	return ((UICDevice *)dev->self)->close ();
}

#endif

int uic_touch_dev_touch (struct uic_dev *dev, int32_t x, int32_t y, int down) {
	return ((UICTouchDev *)dev->self)->touch (x, y, down);
}

int uic_touch_dev_set_resolution (struct uic_dev *dev, uint32_t width, uint32_t height) {
	Size sz;

	sz.width = width;
	sz.height = height;

	BaseInputData *data = BID_alloc (TOUCH_MOD_CMD_RESOLUTION, 1, sizeof (sz), &sz);

	int retval = uic_touch_dev_write (dev, UIS_CMD_IOCTL, data);
	free (data);

	return retval;
}
}
