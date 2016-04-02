#ifndef _TOUCH_H_
#include <uis/device/touch-data.h>

struct _InputService;
typdef struct {
	uint32_t type;
	uint32_t device;
	int init (struct _InputService *, void *);
	int write (struct _InputService *, InputData *);
	int write_array (struct _InputService *, int count, InputData *);
	int free (struct _InputService *);
} InputService;

typedef struct {
	InputService _base;
  
} TouchService;

#ifdef __cplusplus
extern "C" {
#endif

int input_service_register (uint32_t type, InputService *);

TouchService *touch_service_new ();
int touch_service_init (TouchService *, const char *dev);
int touch_service_write (TouchService *, IN TouchData *);
int touch_service_free (TouchService *);

#ifdef __cplusplus
}
#endif

#endif
