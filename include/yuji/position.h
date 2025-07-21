#pragma once

#include <stddef.h>

typedef struct {
  size_t line;
  size_t column;
  const char* file_name;
} Position;

Position* position_init(size_t line, size_t column, const char* file_name);
void position_free(Position*);

Position* position_copy(const Position* position);
