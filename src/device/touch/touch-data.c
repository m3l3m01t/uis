#include <stdlib.h>
#include <uis/device/touch-data.h>

TouchData *touch_data_new (int32_t identity, TouchAction action) {
	TouchData *data = (TouchData *) malloc (sizeof (TouchData));
//	data->index = -1;
	data->trackingId = identity;
	//data->action = action;	

	return data;
}

void touch_data_init (TouchData *data, int32_t identity, TouchAction action) {
//	data->index = -1;
	data->trackingId = identity;
//	data->action = action;	
}

inline void touch_data_set_xy (IN TouchData *data, int32_t x, int32_t y) {
	data->x = x;
	data->y = y;
}

/*
inline void touch_data_set_action (IN TouchData *data, TouchAction action) {
	data->action = action;	
}

inline void touch_data_set_index (IN TouchData *data, int32_t index) {
	data->index = index;
}
*/

inline void touch_data_free (INOUT TouchData *data) {
	free (data);
}
