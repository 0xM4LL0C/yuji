#include "yuji/map.h"
#include "yuji/dyn_array.h"
#include <stdlib.h>
#include <string.h>

MapPair* map_pair_init(const char* key, void* value) {
  MapPair* pair = malloc(sizeof(MapPair));

  if (!pair) {
    return NULL;
  }

  pair->key = strdup(key);
  pair->value = value;

  return pair;
}

void map_pair_free(MapPair* pair) {
  if (pair) {
    free((void*)pair->key);
    free(pair);
  }
}

Map* map_init() {
  Map* map = malloc(sizeof(Map));

  if (!map) {
    return NULL;
  }

  map->pairs = dyn_array_init();

  if (!map->pairs) {
    free(map);
    return NULL;
  }

  return map;
}

void map_free(Map* map) {
  if (map) {
    for (size_t i = 0; i < map->pairs->size; i++) {
      MapPair* pair = dyn_array_get(map->pairs, i);
      map_pair_free(pair);
    }

    dyn_array_free(map->pairs);
    free(map);
  }
}

void map_insert(Map* map, const char* key, void* value) {
  size_t index = map_index_of(map, key);

  if (index == (size_t) -1) {
    MapPair* pair = map_pair_init(key, value);

    if (!pair) {
      return;
    }

    dyn_array_append(map->pairs, pair);
    return;
  }

  MapPair* pair = dyn_array_get(map->pairs, index);
  pair->value = value;
}

void map_remove(Map* map, const char* key) {
  size_t index = map_index_of(map, key);

  if (index == (size_t) -1) {
    return;
  }

  dyn_array_remove(map->pairs, index);
}

size_t map_index_of(Map *map, const char* key) {
  for (size_t i = 0; i < map->pairs->size; i++) {
    MapPair* pair = dyn_array_get(map->pairs, i);

    if (strcmp(pair->key, key) == 0) {
      return i;
    }
  }

  return -1;
}


void* map_get(Map* map, const char* key) {
  for (size_t i = 0; i < map->pairs->size; i++) {
    MapPair* pair = dyn_array_get(map->pairs, i);

    if (strcmp(pair->key, key) == 0) {
      return pair->value;
    }
  }

  return NULL;
}
