#ifndef INPUT_DATA_H
#define INPUT_DATA_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UIS_CMD_INIT             0x10000001
#define UIS_CMD_OPEN_DEVICE      0x10000002
#define UIS_CMD_CLOSE_DEVICE     0x10000003
#define UIS_CMD_CLOSE_SOURCE     0x10000004
#define UIS_CMD_IOCTL            0x10000005

#define UIS_CMD_INPUT            0x00000001

#define UIS_CMD_MASK_SET         0xff000000
#define UIS_CMD_MASK_CMD         0x00ffffff

#define TOUCH_MOD_CMD_RESOLUTION 0x100
#define TOUCH_MOD_CMD_RECT 	 0x200

#define KBD_CMD_SET_KEYMAP 	0x100
#define KBD_CMD_KEY_PRESS 	0x2

typedef struct {
	uint32_t width;
	uint32_t height;
} Size;

typedef struct {
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
} IRect;


typedef struct {
	uint16_t type;
	uint16_t count;
	uint32_t body_len;
	uint8_t body[0];
} BaseInputData;

typedef struct {
	uint32_t symcode;
	uint16_t scancode;
} KeymapBody;

typedef struct {
	uint32_t symcode;
	uint16_t scancode;
	uint16_t value;
} KeyPressBody;

typedef struct ATTR_PACKED {
	uint32_t devtype;
	int16_t  len;
//	int16_t  reserved;
	char     keyword[0];
} OpenDeviceBody;

typedef struct {
	BaseInputData head;
	union {
		KeymapBody keymap[0];
		KeyPressBody keys[0];
	};
} KeyInputData;


#define BID_HEADER_LEN sizeof(BaseInputData)
static BaseInputData *BID_alloc (uint16_t type, uint16_t count, uint32_t bodyLen, const void *p) {
	BaseInputData *instance = (BaseInputData *)malloc (BID_HEADER_LEN + bodyLen);
	instance->type = type;
	instance->count = count;
	instance->body_len = bodyLen;
	if (bodyLen > 0) {
		memcpy (instance->body, p, bodyLen);
	}
	return instance;
}

inline static void BID_free (BaseInputData *bid) {
	free (bid);
}

#define BID_DataLen(bid) (bid)->body_len
#define BID_Len(bid) ((bid)->body_len + sizeof (BaseInputData))
#define BID_HeadLen() (sizeof (BaseInputData))

#ifdef __cplusplus
}
#endif

#endif

