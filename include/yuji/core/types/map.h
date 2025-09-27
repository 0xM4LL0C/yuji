#pragma once

#include "yuji/core/types/dyn_array.h"

typedef struct {
  const char* key;
  void* value;
} YujiMapPair;

typedef struct {
  YujiDynArray* pairs;
} YujiMap;

YujiMapPair* yuji_map_pair_init(const char* key, void* value);
void yuji_map_pair_free(YujiMapPair* pair);

YujiMap* yuji_map_init();
void yuji_map_free(YujiMap* map);

void yuji_map_insert(YujiMap* map, const char* key, void* value);
void yuji_map_remove(YujiMap* map, const char* key);
size_t yuji_map_index_of(YujiMap* map, const char* key);
void* yuji_map_get(YujiMap* map, const char* key);
void yuji_map_set(YujiMap* map, const char* key, void* value);
