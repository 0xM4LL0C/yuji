#pragma once

#include <stddef.h>
#include <stdbool.h>

#if !defined(YUJI_DYN_ARRAY_INITIAL_CAPACITY)
#define YUJI_DYN_ARRAY_INITIAL_CAPACITY 8
#endif

typedef struct {
  void** data;
  size_t size;
  size_t capacity;
} YujiDynArray;

#define YUJI_DYN_ARRAY_ITER(ARR, TYPE, VAR_NAME, BODY) \
  for (size_t i = 0; i < ARR->size; i++) { \
    TYPE* VAR_NAME = (TYPE*)yuji_dyn_array_get(ARR, i); \
    BODY\
  }

YujiDynArray* yuji_dyn_array_init();
void yuji_dyn_array_free(YujiDynArray* arr);

void yuji_dyn_array_push(YujiDynArray* arr, void* item);
void* yuji_dyn_array_pop(YujiDynArray* arr);

void* yuji_dyn_array_get(YujiDynArray* arr, size_t index);
bool yuji_dyn_array_set(YujiDynArray* arr, size_t index, void* item);
