#pragma once
#include <stdbool.h>
#include <stddef.h>

#ifndef YUJI_DYN_ARRAY_MIN_CAPACITY
#define YUJI_DYN_ARRAY_MIN_CAPACITY 8
#endif

typedef struct {
  void** data;
  size_t size;
  size_t capacity;
} DynArr;

DynArr* dyn_array_init();
void dyn_array_free(DynArr* arr);

bool dyn_array_append(DynArr* arr, void* value);
void* dyn_array_get(const DynArr* arr, size_t index);
bool dyn_array_set(DynArr* arr, size_t index, void* value);
void dyn_array_remove(DynArr* arr, size_t index);

#define DYN_ARR_ITER(arr, type, var, body) \
  for (size_t i = 0; i < arr->size; i++) { \
    type* var = (type*)dyn_array_get(arr, i); \
    body;\
  }
