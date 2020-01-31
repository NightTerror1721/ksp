#ifndef KSP_SUPPORT_MAP_H
#define KSP_SUPPORT_MAP_H

#include <stdlib.h>

#include "ctypes.h"

typedef struct Map
{
	struct Map* parent;
	char** key;
	void** val;
	size_t size;
	size_t nelem;
	size_t nused;
} Map;

void map_init_parent(Map* map, Map* parent);
inline void map_init(Map* map) { map_init_parent(map, NULL); }

Map* map_new();
Map* map_new_parent(Map* parent);

void* map_get(const Map* map, const char* key);
inline BOOL map_has(const Map* map, const char* key) { return BOOL_TEST(map_get(map, key)); }

BOOL map_put(Map* map, const char* key, void* value);
void* map_emplace(Map* map, const char* key, size_t size);

void* map_remove(Map* map, const char* key);
BOOL map_erase(Map* map, const char* key);

size_t map_len(const Map* map);


#endif KSP_SUPPORT_MAP_H