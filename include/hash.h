#ifndef _LINUX_GHASH_H_
#define _LINUX_GHASH_H_
#include <string.h>

#ifndef __USE_ISOC99
#define inline
#endif

typedef struct _hashentry {
	void   *key;
	void   *data;
	struct _hashentry *next;
} hashentry;

typedef struct _hashtable {
	unsigned int 	(*gethash)(void *);
	int 		(*compare)(void *, void *);
	int 		(*update)(hashentry *, hashentry *);

	int 		hashsize;
	int 		count;

	hashentry 	**hashlist;
} hashtable;

/*
#define create_hashtable(hsize) \
         hash_create(lh_strhash, equal_str, hsize)

unsigned int lh_strhash(void *src);
int equal_str(void *k1, void *k2);
*/

#ifdef __cplusplus
extern "C" {
#endif
hashtable *hash_create(unsigned int (*keyfunc)(void *),
		int (*comparefunc)(void *,void *),
		int (*update)(hashentry *, hashentry *),
		int size);

void hash_free (hashtable *tab);

void hash_insert (hashtable *tab, hashentry *);
void hash_remove (hashtable *tab, void *key);

const void *hash_value (hashtable *tab, void *key);

int  hash_count (hashtable *tab);

void hash_for_each_do (hashtable *tab, int (cb)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif
