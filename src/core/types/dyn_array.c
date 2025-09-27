#include "yuji/core/types/dyn_array.h"
#include "yuji/core/memory.h"

#include <stdlib.h>

YujiDynArray* yuji_dyn_array_init() {
  YujiDynArray* arr = yuji_malloc(sizeof(YujiDynArray));

  arr->data = yuji_malloc(sizeof(void*) * YUJI_DYN_ARRAY_INITIAL_CAPACITY);
  arr->capacity = YUJI_DYN_ARRAY_INITIAL_CAPACITY;
  arr->size = 0;

  return arr;
}

void yuji_dyn_array_free(YujiDynArray *arr) {
  yuji_free(arr->data);
  yuji_free(arr);
}


void yuji_dyn_array_push(YujiDynArray *arr, void* item) {
  yuji_check_memory(arr);

  if (arr->size == arr->capacity) {
    arr->capacity *= 2;
    arr->data = yuji_realloc(arr->data, sizeof(void*) * arr->capacity);
  }

  arr->data[arr->size++] = item;
}

void* yuji_dyn_array_pop(YujiDynArray *arr) {
  yuji_check_memory(arr);

  if (arr->size == 0) {
    return NULL;
  }

  void* item = arr->data[arr->size - 1];
  arr->size--;

  return item;
}

void* yuji_dyn_array_get(YujiDynArray* arr, size_t index) {
  yuji_check_memory(arr);

  if (index >= arr->size) {
    return NULL;
  }

  return arr->data[index];
}

bool yuji_dyn_array_set(YujiDynArray *arr, size_t index, void* item) {
  yuji_check_memory(arr);

  if (index >= arr->size) {
    return false;
  }

  arr->data[index] = item;
  return true;
}

void yuji_dyn_array_remove(YujiDynArray* arr, size_t index) {
  yuji_check_memory(arr);

  if (index >= arr->size) {
    return;
  }

  for (size_t i = index; i < arr->size - 1; i++) {
    arr->data[i] = arr->data[i + 1];
  }

  arr->size--;
}
