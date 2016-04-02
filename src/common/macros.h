#ifndef _PRIV_MACROS_H_
#define _PRIV_MACROS_H_

#define GNUC_UNUSED __attribute__((__unused__))

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define UIS_SERVICE_PORT 5059

#endif
