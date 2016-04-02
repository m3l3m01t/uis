#include <string.h>
#ifdef   DMALLOC
#include <dmalloc.h>
#else
#include <stdlib.h>
#endif
#include <uis/hash.h>
#ifndef __USE_ISOC99
#define inline
#endif

#define hashindex(tab, key) ((tab->gethash)(key) % (tab->hashsize -1))

static unsigned int lh_strhash(void *src) {
	int i, l;
	unsigned long ret = 0;
	unsigned short *s;
	char *str = (char *)src;
	if (str == NULL)
		return(0);
	l = (strlen(str) + 1) / 2;
	s = (unsigned short *)str;

	for (i = 0; i < l; i++)
		ret ^= s[i]<<(i&0x0f);

	return(ret);
}

static int equal_str(void *k1, void *k2) {
	return (0 == strcmp((char *)k1, (char *)k2));
}

inline hashentry *hashentry_new(void *key, void *data) {
	hashentry *entry = malloc(sizeof(hashentry));
	entry->key = key;
	entry->data = data;
	entry->next = NULL;
	return entry;
}

static hashentry *hashentry_free(hashtable *table, hashentry *entry) {
	hashentry *next = entry->next;
	table->update (entry, NULL);
	return next;
}

void hlist_append(hashentry **root, hashentry *entry) {
	hashentry *pos;

	if (*root == NULL) {
		*root = entry;
	} else {
		for(pos = *root; pos->next != NULL; pos = pos->next);
		pos->next = entry;
	}
}

int hlist_update(hashtable *table, hashentry *root, hashentry *entry) {
	hashentry *pos;
	for(pos = root; pos != NULL; pos = pos->next ) {
		if (table->compare (entry->key, pos->key)) {
			table->update (pos, entry);
			return 0;
		}
	}
	return -1;
}

int hlist_remove(hashtable *tab, hashentry **root, void *key) {
	hashentry *pos ,*prev;

	if (NULL == *root) return -1;

	if (tab->compare((*root)->key, key)) {
		*root = hashentry_free(tab, *root);
		return 0;
	}

	prev = *root;
	for (pos = prev->next; NULL != pos; pos = pos->next) {
		if (tab->compare(pos->key, key)) {
			prev->next = hashentry_free(tab, pos);
			return 0;
		}
		prev = pos;
	}
	return -1;
}

hashtable *hash_create(unsigned int (*keyfunc)(void *),
		int (*comparefunc)(void *, void *),
		int (*update)(hashentry *, hashentry *),
		int size) {
	int i;
	int len = sizeof(hashentry *) * size;

	hashtable *tab = malloc(sizeof(hashtable));
	memset(tab, 0, sizeof(hashtable));
	tab->hashlist = malloc(len);

	if (tab->hashlist == NULL) {
		free(tab);
		return NULL;
	}

	memset(tab->hashlist, 0, len );

	tab->hashsize = size;
	tab->count    = 0;

	tab->gethash = keyfunc;
	tab->compare = comparefunc;
	tab->update = update;

	return tab;
}

void hash_free(hashtable *tab) {
	int i;
	hashentry *pos;

	for (i = 0; i < tab->hashsize; i++) {
		pos = tab->hashlist[i]; 
		while (pos) {
			pos = hashentry_free(tab, pos);
		}
	}

	free(tab->hashlist);
	free(tab);
}

void hash_insert(hashtable *tab, hashentry *entry) {
	unsigned int index = hashindex(tab, entry->key);
	hashentry *root = tab->hashlist[index];

	if (hlist_update(tab, root, entry) != 0) { //(1)
		hlist_append(&(tab->hashlist[index]), entry);
		tab->count++;
	}
}

void hash_remove(hashtable *tab, void *key) {
	unsigned int index = hashindex(tab, key);

	if (hlist_remove(tab, &(tab->hashlist[index]), key) == 0) {
		tab->count--;
	}
}

const void *hash_value(hashtable *tab, void *key) {
	hashentry *pos;
	unsigned int index = hashindex(tab, key);

	for (pos = tab->hashlist[index]; NULL != pos; pos = pos->next) {
		if (tab->compare(key, pos->key)) {
			return (pos->data);
		}
	}
	return NULL;
}

void hash_for_each_do(hashtable *tab, int(cb)(const void *, const void *)) {
	int i = 0;
	hashentry *pos;

	for (i = 0; i < tab->hashsize; i++) {
		for (pos = tab->hashlist[i]; NULL != pos; pos = pos->next ) {
			cb(pos->key, pos->data);
		}
	}
}

inline int hash_count(hashtable *tab) {
	return tab->count;
}
