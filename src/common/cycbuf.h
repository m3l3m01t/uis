#ifndef _CYCLE_BUF_H
#define _CYCLE_BUF_H

#include <stdint.h>

typedef struct {
	int size;
	int rpos;
	int wpos;
	uint8_t *data;
} cycbuf;

#ifdef __cplusplus
extern "C" {
#endif
int cycbuf_init (cycbuf *buf, int capacity) ;
void cycbuf_destroy (cycbuf *buf) ;

int cycbuf_empty (cycbuf *buf);

/* return avaliable data length */
int cycbuf_available (cycbuf *buf);

/* return freespace size can be written into */
int cycbuf_freespace (cycbuf *buf, int rpos);

/* read at most size bytes into out, return length read */
int cycbuf_read (cycbuf *buf, void *out, int size);

/* write at most size bytes from in, return length written */
int cycbuf_write (cycbuf *buf, void *in, int size);

/* put first available address into pptr, returns length */
int cycbuf_readable_region (cycbuf *buf, uint8_t **pptr);

/* put free space address into pptr, returns space size */
int cycbuf_writable_region (cycbuf *buf, uint8_t **pptr);
void cycbuf_skip (cycbuf *buf, int len);

#ifdef __cplusplus
}
#endif

#endif
