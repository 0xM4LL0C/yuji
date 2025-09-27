#include "yuji/core/types/map.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"
#include <stdlib.h>
#include <string.h>

YujiMapPair* yuji_map_pair_init(const char* key, void* value) {
  YujiMapPair* pair = yuji_malloc(sizeof(YujiMapPair));

  pair->key = strdup(key);
  pair->value = value;

  return pair;
}

void yuji_map_pair_free(YujiMapPair* pair) {
  yuji_free((void*)pair->key);
  yuji_free(pair);
}

YujiMap* yuji_map_init() {
  YujiMap* map = yuji_malloc(sizeof(YujiMap));

  map->pairs = yuji_dyn_array_init();

  return map;
}

void yuji_map_free(YujiMap* map) {
  if (map) {
    for (size_t i = 0; i < map->pairs->size; i++) {
      YujiMapPair* pair = yuji_dyn_array_get(map->pairs, i);
      yuji_map_pair_free(pair);
    }

    yuji_dyn_array_free(map->pairs);
    yuji_free(map);
  }
}

void yuji_map_insert(YujiMap* map, const char* key, void* value) {
  size_t index = yuji_map_index_of(map, key);

  if (index == (size_t) -1) {
    YujiMapPair* pair = yuji_map_pair_init(key, value);

    if (!pair) {
      return;
    }

    yuji_dyn_array_push(map->pairs, pair);
    return;
  }

  YujiMapPair* pair = yuji_dyn_array_get(map->pairs, index);
  pair->value = value;
}

void yuji_map_remove(YujiMap* map, const char* key) {
  size_t index = yuji_map_index_of(map, key);

  if (index == (size_t) -1) {
    return;
  }

  yuji_dyn_array_remove(map->pairs, index);
}

size_t yuji_map_index_of(YujiMap *map, const char* key) {
  for (size_t i = 0; i < map->pairs->size; i++) {
    YujiMapPair* pair = yuji_dyn_array_get(map->pairs, i);

    if (strcmp(pair->key, key) == 0) {
      return i;
    }
  }

  return (size_t) -1;
}


void* yuji_map_get(YujiMap* map, const char* key) {
  for (size_t i = 0; i < map->pairs->size; i++) {
    YujiMapPair* pair = yuji_dyn_array_get(map->pairs, i);

    if (strcmp(pair->key, key) == 0) {
      return pair->value;
    }
  }

  return NULL;
}

void yuji_map_set(YujiMap* map, const char* key, void* value) {
  size_t index = yuji_map_index_of(map, key);

  if (index == (size_t) -1) {
    YujiMapPair* pair = yuji_map_pair_init(key, value);

    if (!pair) {
      return;
    }

    yuji_dyn_array_push(map->pairs, pair);
    return;
  }

  YujiMapPair* pair = yuji_dyn_array_get(map->pairs, index);
  pair->value = value;
}
