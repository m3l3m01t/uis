#ifndef _TOUCH_DATA_H_
#define _TOUCH_DATA_H_

#include <stdint.h>

#define TOUCH_ACTION_POINTER_INDEX_MASK 0xff00
#define TOUCH_ACTION_POINTER_INDEX_SHIFT 8
#define TOUCH_ACTION_MASK       0xff

#define TOUCH_ACTION_DOWN   	0
#define TOUCH_ACTION_UP		1
#define TOUCH_ACTION_MOVE	2
#define TOUCH_ACTION_CANCEL	3
#define TOUCH_ACTION_POINTER_DOWN	5
#define TOUCH_ACTION_POINTER_UP	6

/*
enum { TOUCH_MOD_ACTION_DOWN  = 0, TOUCH_MOD_ACTION_UP, TOUCH_MOD_ACTION_MOVE,
	TOUCH_MOD_ACTION_CANCEL, TOUCH_MOD_ACTION_POINTER_DOWN, 
	TOUCH_MOD_ACTION_POINTER_UP};
*/

#define IN
#define OUT
#define INOUT

typedef enum {Down = 0, Up, Move, Cancel, PointerDown = 5, PointerUp} TouchAction;

/*-----------------------------------------------------------------------------
 * TouchEvent
 */
typedef struct {
	int32_t trackingId;
	int32_t pressure;
	uint32_t x;
	uint32_t y;
} TouchData;

#ifdef __cplusplus
extern "C" {
#endif

TouchData *touch_data_new (int32_t identity, TouchAction action);
void touch_data_init (TouchData *data, int32_t identity, TouchAction action);

void touch_data_set_xy (IN TouchData *data, int32_t x, int32_t y);

void touch_data_set_action (IN TouchData *data, TouchAction action);

//void touch_data_set_index (IN TouchData *data, int32_t index);

void touch_data_free (INOUT TouchData *data);

#ifdef __cplusplus
}
#endif

#endif /* _TOUCH_DATA_H */
