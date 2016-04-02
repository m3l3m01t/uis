#include <stdio.h>
#include <stdlib.h>

#include <uis/device/touch-data.h>
#include "client/UICTransport.h"
#include "client/UICDBusTransport.h"
#include "client/UICTcpTransport.h"
#include "client/UICPeer.h"
#include "client/UICDevice.h"
#include "client/UICTouchDevice.h"
#include "client/UICKeyboardDev.h"

int main (int argc, char **argv) {
	UICTransport *transport = new UICTcpTransport();
	transport->initialize ();
	
	UICPeer *peer = transport->openPeer ("127.0.0.1");
	if (peer == NULL) {
		fprintf (stderr, "cannot open UIS connection\n");
		return -1;
	}

	if (1) {
		UICDevice *dev = new UICTouchDev (peer);
		if (dev->open () == 0) {
			int buf[2] = {640, 480};

			BaseInputData *data; 

			fprintf (stderr, "device opened\n");

			data = BID_alloc (TOUCH_ACTION_DOWN, 1, sizeof (buf), buf);
			dev->write (UIS_CMD_INPUT, data);
			free (data);

			data = BID_alloc (TOUCH_ACTION_UP, 1, sizeof (buf), buf);
			dev->write (UIS_CMD_INPUT, data);

			free (data);

//			dev->close ();
		} else {
			fprintf (stderr, "failed to open touchscreen device\n");
		}
	}

	if (1) {
		UICDevice *dev = new UICKeyboardDev (peer);
		if (dev->open () == 0) {
			KeyPressBody buf;

			BaseInputData *data; 

			fprintf (stderr, "device opened\n");

#ifndef 	XK_Escape
#define XK_Escape 0xff1b
#endif
			buf.symcode = XK_Escape;
			buf.scancode = 0;
			buf.value = 0;

			data = BID_alloc (KBD_CMD_KEY_PRESS, 1, sizeof (buf), &buf);
			dev->write (UIS_CMD_INPUT, data);
			free (data);
		
			buf.value = 1;
			data = BID_alloc (KBD_CMD_KEY_PRESS, 1, sizeof (buf), &buf);
			dev->write (UIS_CMD_INPUT, data);
			free (data);
			
//			dev->close ();
		} else {
			fprintf (stderr, "cannot open keyboard\n");
		}
	}
	return 0;
}

