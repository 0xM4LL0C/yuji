#include "yuji/core/memory.h"
#include "yuji/core/runner.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

extern YujiRunner* g_yuji_runner;

void yuji_panic(const char* fmt, ...) {
  fprintf(stderr, "===== PANIC =====\n");

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);

  if (g_yuji_runner) {
    yuji_print_call_stack(g_yuji_runner);
  }

#ifdef YUJI_DEBUG
  abort();
#else
  exit(EXIT_FAILURE);
#endif
}

void yuji_check_memory(void* ptr) {
  if (!ptr) {
    yuji_panic("error: memory is null: %p", ptr);
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
