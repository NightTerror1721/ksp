#include "map.h"

#include <stdlib.h>
#include <string.h>

/* PRIVATE FUNCTIONS */

#define INIT_SIZE 16
#define GRAVESTONE ((void*) -1)

typedef uint32_t hash_t;

typedef struct
{
	size_t size;
	void* value;
} value_t;
#define is_emplaced(_Value) ((_Value)->size > 0)

static hash_t hash(const char* str)
{
	// FNV hash //
	hash_t h = 0x811C9DC5;
	for (; *str; ++str)
	{
		h ^= *str;
		h *= 0x1000193;
	}
	return h;
}

static char* copy_key(const char* key)
{
	size_t s = strlen(key);
	char* nk = (char*)calloc(s, sizeof(char));
	strcpy_s(nk, s, key);
	return nk;
}
#define delete_key(_Key) (free(_Key))

static void delete_map_keys(char** keys, size_t size)
{
	for (size_t i = 0; i < size; ++i)
		delete_key(keys[i]);
	free(keys);
}

static void do_init_map(Map* map, Map* parent, size_t size)
{
	map->parent = parent;
	map->key = (char**)calloc(size, sizeof(char*));
	map->val = (value_t**)calloc(size, sizeof(value_t*));
	map->size = size;
	map->nelem = 0;
	map->nused = 0;
}

static void maybe_rehash(Map* map)
{
	if (!map->key)
	{
		map->key = (char**)calloc(INIT_SIZE, sizeof(char*));
		map->val = (value_t**)calloc(INIT_SIZE, sizeof(value_t*));
		map->size = INIT_SIZE;
		return;
	}

	if (map->nused < map->size * 0.7f)
		return;

	size_t newsize = map->nelem < map->size * 0.35f ? map->size : map->size * 2;
	char** newkey = (char**)calloc(newsize, sizeof(char*));
	void** newval = (value_t**)calloc(newsize, sizeof(value_t*));
	size_t mask = newsize - 1;

	for (size_t i = 0; i < map->size; ++i)
	{
		if (!map->key[i] || map->key[i] == GRAVESTONE)
			continue;

		hash_t h = hash(map->key[i]) & mask;
		for (;; h = (h + 1) & mask)
		{
			if (newkey[h])
				continue;

			newkey[h] = map->key[i];
			newval[h] = map->val[i];
		}
	}

	delete_map_keys(map->key, map->size);
	free(map->val);
	map->key = newkey;
	map->val = newval;
	map->size = newsize;
	map->nused = map->nelem;
}

static void* map_get_nostack(Map* map, const char* key)
{
	if (!map->key)
		return NULL;

	size_t mask = map->size - 1;

	for (hash_t i = hash(key) & mask; map->key[i]; i = (i + 1) & mask)
		if (map->key[i] != GRAVESTONE && strcmp(map->key[i], key) == 0)
			return map->val[i];
	return NULL;
}



/* PUBLIC FUNCTIONS */

void map_init_parent(Map* map, Map* parent) { do_init_map(map, parent, INIT_SIZE); }

Map* map_new()
{
	Map* map = (Map*)malloc(sizeof(Map));
	do_init_map(map, NULL, INIT_SIZE);
	return map;
}
Map* map_new_parent(Map* parent)
{
	Map* map = (Map*)malloc(sizeof(Map));
	do_init_map(map, parent, INIT_SIZE);
	return map;
}

void* map_get(const Map* map, const char* key)
{
	void* val = map_get_nostack(map, key);
	if (val)
		return val;
	return map->parent ? map_get(map->parent, key) : NULL;
}

BOOL map_put(Map* map, const char* key, void* value)
{
	maybe_rehash(map);

	size_t mask = map->size - 1;
	for (hash_t i = hash(key) & mask; ; i = (i + 1) & mask)
	{
		const char* k = map->key[i];
		if (!k || k == GRAVESTONE)
		{
			map->key[i] = key;
			map->val[i] = value;
			++map->nelem;

			if (!k)
				++map->nused;
			return;
		}

		if (!strcmp(key, k) != 0)
		{
			map->val[i] = value;
			return;
		}
	}
}
void* map_emplace(Map* map, const char* key, size_t size);

void* map_remove(Map* map, const char* key);
BOOL map_erase(Map* map, const char* key);

size_t map_len(const Map* map);
