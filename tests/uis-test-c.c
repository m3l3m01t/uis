#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client/uic-c.h"

#include "common/InputData.h"
#include "uis/device/touch-data.h"

int main (int argc, char **argv) {
	struct uic_transport trans;
	struct uic_peer peer;
/*
	struct uic_touch_dev dev;
	struct uic_kbd_dev kbd;

*/
	struct uic_dev dev;
	struct uic_dev kbd;

	//uic_transport_init (&trans, UIC_TRANSPORT_DBUS, NULL);
	//if (uic_peer_init (&peer, &trans, NULL) != 0) {

	/* USE_DBUS must be defined to "1" in src/client/Android.mk to use DBus client */
	int type = UIC_TRANSPORT_DBUS;
	
	if (argc == 2 && strcmp (argv[1], "tcp") == 0) {
		type = UIC_TRANSPORT_TCP;
	}
	if (uic_transport_init (&trans, type, NULL) == -1) {
		fprintf (stderr, "cannot initialize transport %d\n", type);	
	} else {
		fprintf (stderr, "transport initialized\n");
	}
	if (uic_peer_init (&peer, &trans, "localhost") != 0) {
		fprintf (stderr, "cannot initialize connection\n");
		return -1;
	}
	fprintf (stderr, "peer initialized\n");

	uic_touch_dev_init (&dev, &peer);

	fprintf (stderr, "opening non exists device\n");
	if (uic_touch_dev_open (&dev, "No Such Device") != 0) {
		fprintf (stderr, "cannot open non exists touchscreen device\n");
	}

	fprintf (stderr, "opening virtual device\n");
	if (uic_touch_dev_open (&dev, "virtual") != 0) {
		fprintf (stderr, "cannot open touchscreen device\n");
		goto cleanup;
	} else {
		BaseInputData *data; 
		/* set resolution to */ {
#if 0
			int buf[2] = {480, 320}; /* touch screen resolution */

			fprintf (stderr, "multi touchscreen device opened\n");

			data = BID_alloc (TOUCH_MOD_CMD_RESOLUTION, 1, sizeof (buf), buf);

			uic_touch_dev_write (&dev, UIS_CMD_IOCTL, data);
			free (data);

#else
			uic_touch_dev_set_resolution (&dev, 480, 320);
#endif
		} 

		/* touch 450, 300 */ {
			TouchData buf = {1234, 0, 450, 300};
			buf.x = buf.x * scaleX;

			data = BID_alloc (TOUCH_ACTION_DOWN, 1, sizeof (buf), &buf);

			uic_touch_dev_write (&dev, UIS_CMD_INPUT, data);
			free (data);

			sleep (1);
			data = BID_alloc (TOUCH_ACTION_UP, 1, sizeof (buf), &buf);

			uic_touch_dev_write (&dev, UIS_CMD_INPUT, data);
			free (data);
		}

		uic_touch_dev_close (&dev);
	}

/* keyboard C API is not tested yet */
	uic_kbd_dev_init (&kbd, &peer);
	if (uic_kbd_dev_open (&kbd, NULL) != 0) {
		fprintf (stderr, "cannot open keyboard\n");
		goto cleanup;
	} else {

		/* send key */ {
#ifndef 	XK_Escape
#define XK_Escape 0xff1b
#endif
			BaseInputData *data;
			/* ESC key down */
			KeyPressBody buf = {XK_Escape, 0, 1};

			data = BID_alloc (KBD_CMD_KEY_PRESS, 1, sizeof (buf), &buf);
			uic_kbd_dev_write (&kbd, UIS_CMD_INPUT, data);
			free (data);
		
			/* UIS service translate symcode to scancode if not set */
			uic_kbd_dev_send_key (&kbd, XK_Escape, 0, 0);
		}
		uic_kbd_dev_close (&kbd);
	}

cleanup:
	uic_touch_dev_destroy (&dev);
	uic_peer_unref (&peer);
	uic_transport_destroy (&trans);

	return 0;
}
