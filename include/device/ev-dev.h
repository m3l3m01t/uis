#ifndef __EV_DEV_H__
#define __EV_DEV_H__
#include <linux/input.h>

#ifndef SYN_MT_REPORT
#define SYN_MT_REPORT		2
#endif

#ifndef ABS_MT_SLOT

#define ABS_MT_SLOT		0x2f	/* MT slot being modified */
#define ABS_MT_TOUCH_MAJOR	0x30	/* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR	0x31	/* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR	0x32	/* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR	0x33	/* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION	0x34	/* Ellipse orientation */
#define ABS_MT_POSITION_X	0x35	/* Center X ellipse position */
#define ABS_MT_POSITION_Y	0x36	/* Center Y ellipse position */
#define ABS_MT_TOOL_TYPE	0x37	/* Type of touching device */
#define ABS_MT_BLOB_ID		0x38	/* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID	0x39	/* Unique ID of initiated contact */
#define ABS_MT_PRESSURE		0x3a	/* Pressure on contact area */
#define ABS_MT_DISTANCE		0x3b	/* Contact hover distance */

#endif

/* this macro is used to tell if "bit" is set in "array"
 * it selects a byte from the array, and does a boolean AND
 * operation with a byte that only has the relevant bit set.
 * eg. to check for the 12th bit, we do (array[1] & 1<<4)
 */
#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

/* this macro computes the number of bytes needed to represent a bit array of the specified size */
#define sizeof_bit_array(bits)  ((bits + 7) / 8)


typedef enum INPUT_DEVICE_CLASS {  
    INPUT_DEVICE_CLASS_UNKNOWN      =  0x00000000,
    /* The input device is a keyboard. */
    INPUT_DEVICE_CLASS_KEYBOARD      = 0x00000001,
                
    /* The input device is an alpha-numeric keyboard (not just a dial pad). */
    INPUT_DEVICE_CLASS_ALPHAKEY      = 0x00000002,
    
    /* The input device is a touchscreen (either single-touch or multi-touch). */
    INPUT_DEVICE_CLASS_TOUCHSCREEN   = 0x00000004,
    
    /* The input device is a trackball. */
    INPUT_DEVICE_CLASS_TRACKBALL     = 0x00000008,
    
    /* The input device is a multi-touch touchscreen. */
    INPUT_DEVICE_CLASS_TOUCHSCREEN_MT= 0x00000010,
                
    /* The input device is a directional pad (implies keyboard, has DPAD keys). */
    INPUT_DEVICE_CLASS_DPAD          = 0x00000020,
        
    /* The input device is a gamepad (implies keyboard, has BUTTON keys). */
    INPUT_DEVICE_CLASS_GAMEPAD       = 0x00000040, 
            
    /* The input device has switches. */
    INPUT_DEVICE_CLASS_SWITCH        = 0x00000080,
} DeviceType;

typedef struct {
	char name[128];  /* event device pathname */
	char devname[128];
	char id[128];

	int version;

	int fd;
	DeviceType classes;

        uint8_t abs_bitmask[sizeof_bit_array(ABS_MAX + 1)];
        uint8_t key_bitmask[sizeof_bit_array(KEY_MAX + 1)];
        uint8_t rel_bitmask[sizeof_bit_array(REL_MAX + 1)];

	int ref;
	void *data;
} EvDev;

typedef struct {
	EvDev *evdev;
} BaseEvDev;

/* 
GCC permits a C structure to have no members:

     struct empty {
     };

The structure will have size zero. In C++, empty structures are part of the 
language. G++ treats empty structures as if they had a single member of type char. 
*/
#if 0
typedef struct {

} EvData;

#else
typedef void EvData;
#endif

typedef struct _InputDev InputDev;

typedef struct {
        BaseEvDev *(*devnew) (EvDev *dev);
        int (*ctrl) (BaseEvDev *dev, int32_t cmd, ...);

        int (*write) (BaseEvDev *dev, int type, const EvData *data);
        int (*write_n) (BaseEvDev *dev, int type, const EvData *data, int count);

//        int (*close) (EvDev *dev);
        void (*free) (BaseEvDev *dev);
} EvDevOps ;

struct _InputDev {
	BaseEvDev *bed;
	EvDevOps *ops;
};

#ifdef __cplusplus
extern "C" {
#endif
int ev_dev_open (const char *pathname, EvDev *dev);
int ev_dev_write (EvDev *dev, const struct input_event *ev, int size);
#define ev_dev_ioctl(dev, cmd, ...) ioctl (dev->fd, cmd, __VA_ARGS__)

EvDev *ev_dev_ref (EvDev *dev);
int ev_dev_unref (EvDev *dev);

DeviceType ev_dev_get_class (EvDev *dev);
const char *ev_dev_get_name (EvDev *dev);
const char *ev_dev_get_id (EvDev *dev);


#ifdef __cplusplus
}
#endif

#endif  /* __EV_DEV_H__ */
