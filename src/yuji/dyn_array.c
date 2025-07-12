#include "yuji/dyn_array.h"
#include <stdbool.h>
#include <stdlib.h>

DynArr* dyn_array_init() {
  DynArr *arr = malloc(sizeof(DynArr));

  if (!arr) {
    return NULL;
  }

  arr->size = 0;
  arr->capacity = YUJI_DYN_ARRAY_MIN_CAPACITY;
  arr->data = malloc(arr->capacity * sizeof(void*));

  if (!arr->data) {
    free(arr);
    return NULL;
  }

  return arr;
}

void dyn_array_free(DynArr *arr) {
  free(arr->data);
  free(arr);
}

bool dyn_array_append(DynArr *arr, void* value) {
  if (arr->capacity < arr->size + 1) {
    arr->capacity *= 2;
    void** data = realloc(arr->data, arr->capacity * sizeof(void*));

    if (!data) {
      return false;
    }

    arr->data = data;
  }

  arr->data[arr->size++] = value;
  return true;
}

void* dyn_array_get(const DynArr *arr, size_t index) {
  if (arr->size <= index) {
    return NULL;
  }

  return arr->data[index];
}

bool dyn_array_set(DynArr *arr, size_t index, void* value) {
  if (arr->size <= index) {
    return false;
  }

  arr->data[index] = value;
  return true;
}

void dyn_array_remove(DynArr *arr, size_t index) {
  if (arr->size <= index) {
    return;
  }

  for (size_t i = index + 1; i < arr->size; ++i) {
    arr->data[i - 1] = arr->data[i];
  }

  arr->size--;
}
