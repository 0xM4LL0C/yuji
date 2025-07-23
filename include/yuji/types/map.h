#pragma once

#include "yuji/types/dyn_array.h"
#include <stdio.h>
typedef struct {
  const char* key;
  void* value;
} MapPair;

typedef struct {
  DynArr* pairs;
} Map;

MapPair* map_pair_init(const char* key, void* value);
void map_pair_free(MapPair* pair);


Map* map_init();
void map_free(Map* map);

void map_insert(Map* map, const char* key, void* value);
void map_remove(Map* map, const char* key);
size_t map_index_of(Map* map, const char* key);
void* map_get(Map* map, const char* key);

#define MAP_ITER(map, value_type, key_name, value_name, body) \
  DYN_ARR_ITER(map->pairs, MapPair, pair, { \
    const char* key = pair->key; \
    value_type* value = (value_type*)pair->value; \
    (void)key; \
    (void)value; \
    body; \
  })
