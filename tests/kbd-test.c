#include <uis/device/kbd-dev.h>
#include <uis/device/ev-dev.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>

#include <stdio.h>

const char *input_dir = "/dev/input";

char dev_prefix[] = "event";


static KbdDev *kbd_open (const char *devname, EvDev *dev) {
	KbdDev *kbd = NULL;
	if (ev_dev_open (devname, dev) == 0) {
		kbd =  kbd_dev_new (dev);
		if (kbd == NULL)
			ev_dev_unref (dev);
	}
	
	return kbd;
}

static void kbd_test (KbdDev *kbd) {
	KeyData data;
        uint32_t keycodes[] = {0x3, 0x52,0x14,0x17, 0x6f}; /* Android keycodes */
	for (uint32_t i = 0; i < sizeof (keycodes)/sizeof (uint32_t); i++) {
		uint32_t keycode = keycodes[i];

		data.sym = keycode_to_symcode (keycode, 0);
		data.value = KEY_ACTION_DOWN;
		kbd_dev_write ((BaseEvDev *)kbd, (EvData *)&data);

		usleep (200);

		data.value = KEY_ACTION_UP;
		kbd_dev_write ((BaseEvDev *)kbd, (EvData *)&data);

		usleep (2000);
	}
}

int main (int argc, char **argv) {
	char *name = NULL;

	EvDev dev;	
	
	if (argc > 1) {
		name = argv[1];
	}

	if (name) {
		KbdDev *kbd = kbd_open  (name, &dev);

		if (kbd) {
			kbd_test (kbd);	
			kbd_dev_free ((BaseEvDev *)kbd);

		} else {
			fprintf (stderr, "device %s not recognized as kbd:%d\n", name, errno);	
			return -1;
		}
	} else {
		struct dirent *dr;

		DIR *dirp = opendir (input_dir);	
		
		if (dirp == NULL) {
			fprintf (stderr, "cannot opendir %s:%d\n", input_dir, errno);
			return -1;
		}

		while ((dr = readdir(dirp))) {
			if (dr->d_type != DT_CHR)
				continue;
			if (strncmp (dr->d_name, "event", sizeof (dev_prefix) - 1) == 0) {
				char devname [256];
				snprintf (devname, sizeof (devname) - 1, "%s/%s", input_dir, dr->d_name);
	
				KbdDev *kbd = kbd_open  (devname, &dev);

				if (kbd) {
					kbd_test (kbd);	
					kbd_dev_free ((BaseEvDev *)kbd);
				} else {
					fprintf (stderr, "device %s not recognized as kbd:%d\n", devname, errno);	
				}
			}
		}
	}

	return 0;
}
