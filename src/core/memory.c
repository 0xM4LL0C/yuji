#include "yuji/core/memory.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void yuji_panic(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");

  va_end(args);

#ifdef YUJI_DEBUG
  abort();
#else
  exit(EXIT_FAILURE);
#endif
}

void yuji_check_memory(void* ptr) {
  if (!ptr) {
    yuji_panic("error: memory is null");
  }
}

void* yuji_malloc(size_t size) {
  void* ptr = malloc(size);
  yuji_check_memory(ptr);
  return ptr;
}

void* yuji_realloc(void* ptr, size_t size) {
  void* new_ptr = realloc(ptr, size);
  yuji_check_memory(new_ptr);
  return new_ptr;
}

void yuji_free(void* ptr) {
  yuji_check_memory(ptr);
  free(ptr);
}
