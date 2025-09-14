#pragma once

#include <stddef.h>

typedef struct {
  size_t line;
  size_t column;
  const char* file;
} YujiPosition;

YujiPosition yuji_position_init(size_t line, size_t column, const char* file);
char* yuji_position_to_string(const YujiPosition position);
