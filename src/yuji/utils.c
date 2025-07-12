#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "yuji/utils.h"

void panic(const char* format, ...) {
  va_list args;
  va_start(args, format);
  fprintf(stderr, ANSI_RED "PANIC: ");
  vfprintf(stderr, format, args);
  fprintf(stderr, ANSI_RESET "\n");
  va_end(args);
  exit(1);
}

void check_memory_is_not_null(void* ptr) {
  if (!ptr) {
    panic("Memory allocation failed");
  }
}
