#include <stdlib.h>
#include "hashmap.h"
#define ISIZE 16

void hashmap_free(hashmap_t *map)
{
	free(map->data);
	free(map);
}

hashmap_t *hashmap_new()
{
	hashmap_t *map;
	
	if (!(map = calloc(1, sizeof(hashmap_t))))
		goto err;
	if (!(map->data = calloc(1, (map->t_size = ISIZE) * sizeof(element_t))))
		goto err;
	return map;
err:
	if (map) hashmap_free(map);
	return 0;
}

static long hashmap_hash_long(hashmap_t *map, long key) {

	/* Robert Jenkins' 32 bit Mix Function */
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);

	/* Knuth's Multiplicative Method */
	key = (key >> 3) * 2654435761UL;

	return (unsigned int)key % map->t_size;
}

static int hashmap_hash(hashmap_t *map, long key)
{
	long ret;
	int i;

	if (map->size == map->t_size) return -1;

	ret = hashmap_hash_long(map, key);

	for (i = 0; i < map->t_size; ++i) {
		if (map->data[ret].in_use == 0)
			return ret;
		if(map->data[ret].key == key && map->data[ret].in_use == 1)
			return ret;

		ret = (ret + 1) % map->t_size;
	}

	return -2;
}

static int hashmap_rehash(hashmap_t *map)
{
	int o_size, i;
	element_t *e, *t;

	if (!(t = calloc(1, (map->t_size << 1) * sizeof(element_t))))
		return -1;

	e = map->data;
	map->data = t;

	o_size = map->t_size;
	map->t_size <<= 1;
	map->size = 0;

	for (i = 0; i < o_size; ++i){
		int status;
		if ((status = hashmap_put(map, e[i].key, e[i].data)) < 0)
			return status;
	}

	free(e);
	return 0;
}

int hashmap_put(hashmap_t *map, long key, void *value)
{
	int idx;
	
	while ((idx = hashmap_hash(map, key)) < 0) {
		if ((idx = hashmap_rehash(map)) < 0)
			return idx;
	}

	map->data[idx].data = value;
	map->data[idx].key = key;
	map->data[idx].in_use = 1;
	++map->size; 

	return 0;
}

element_t *hashmap_find(hashmap_t *map, long key)
{
	long hsh;
	int i;
	hsh = hashmap_hash_long(map, key);

	for (i = 0; i < map->t_size; ++i) {
		if (map->data[hsh].key == key && map->data[hsh].in_use == 1)
			return &map->data[hsh];
		hsh = (hsh + 1) % map->t_size;
	}
	return 0;
}

void *hashmap_get(hashmap_t *map, long key)
{
	element_t *e = hashmap_find(map, key);
	return e ? e->data : 0;
}

int hashmap_remove(hashmap_t *map, long key)
{
	element_t *e = hashmap_find(map, key);
	if (e) {
		e->in_use = 0;
		e->data = 0;
		e->key = 0;
		--map->size;
	}
	return !e;
}

int hashmap_length(hashmap_t *map)
{
	return map ? map->size : 0;
}
