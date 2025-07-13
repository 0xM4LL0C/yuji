#pragma once

#include "yuji/dyn_array.h"
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
