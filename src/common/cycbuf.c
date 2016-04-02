#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "cycbuf.h"
#include "macros.h"
//#include "netutils.h"

int cycbuf_init (cycbuf *buf, int capacity) {
	if (capacity <= 0 || buf == NULL) {
		return -1;
	}

	buf->data = NULL;
	buf->data = (uint8_t *)malloc (capacity + 1);
	if (buf->data) {
		buf->size = capacity + 1;
		buf->rpos = 0;
		buf->wpos = 0;
		return 0;
	} else {
		return -1;
	}
}

void cycbuf_destroy (cycbuf *buf) {
	if (buf->data != NULL) {
		free (buf->data);
		buf->data = NULL;
	}
}

int cycbuf_empty (cycbuf *buf)
{
	return buf->rpos == buf->wpos ? 1 : 0;
}

int cycbuf_available (cycbuf *buf) {
	int available = buf->wpos - buf->rpos;
	return available >= 0 ? available : buf->size + available;
}

int cycbuf_freespace (cycbuf *buf, int rpos) {
	int offset = rpos - buf->wpos;
	return offset > 0 ? offset - 1: buf->size + offset - 1;
}

static int read_block (cycbuf *buf, void *out, int size) {
		memcpy (out, buf->data + buf->rpos, size);	
		buf->rpos = (buf->rpos + size) % buf->size;

		return size;
}

/* 
 * read size bytes from buffer into out,  
 *
 * returns bytes read
 */
int cycbuf_read (cycbuf *buf, void *out, int size) {
	int wpos = buf->wpos;
	int available = wpos - buf->rpos;

	int l = 0;
	if (available < 0) {
		l = MIN (buf->size - buf->rpos, size);
		read_block (buf, out, l);

		size -= l;
	}

	available = wpos - buf->rpos;

	if (size > 0 && available > 0) {
		l += read_block (buf, (uint8_t *)out + l, MIN (size, available));
	}

	return l;
}


static int write_block (cycbuf *buf, void *in, int size) {
	if (in) {
		memcpy (buf->data + buf->wpos, in, size);	
	}

	buf->wpos = (buf->wpos + size) % buf->size;

	return size;
}

/* 
 * write size bytes from in into buffer
 *
 * returns bytes written 
 */
int cycbuf_write (cycbuf *buf, void *in, int size) {
	int l = 0;
	int rpos = buf->rpos;

	if (rpos <= buf->wpos) {
		if (rpos == 0) {
			l = MIN (buf->size - buf->wpos - 1, size);
			size -= write_block (buf, in, l);
			return l;
		} else {
			/* write into free space till end of buffer */
			l = MIN (buf->size - buf->wpos, size);
			size -= write_block (buf, in, l);
		}
	}

	/* rpos > wpos now */
	if (size > 0) {
		int freeSpace = rpos - buf->wpos - 1;
		if (freeSpace > 0)
			l += write_block (buf, (uint8_t *)in + l, MIN (size, freeSpace));
	}

	return l;
}

int cycbuf_writable_region (cycbuf *buf, uint8_t **pptr) {
	int rpos = buf->rpos;

	*pptr = buf->data + buf->wpos;
	if (rpos > buf->wpos) {
		return rpos - buf->wpos - 1;
	} else {
		return (rpos == 0) ? buf->size - buf->wpos - 1: buf->size - buf->wpos;
	}
}

int cycbuf_readable_region (cycbuf *buf, uint8_t **pptr) {
	int wpos = buf->wpos;

	*pptr = buf->data + buf->rpos;
	if (wpos >= buf->rpos) {
		return wpos - buf->rpos;
	} else {
		return buf->size - buf->rpos;
	}
}

void cycbuf_skip (cycbuf *buf, int len) {
	buf->rpos = (buf->rpos + len) % buf->size;
}
