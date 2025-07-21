#include "yuji/position.h"
#include "yuji/utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Position* position_init(size_t line, size_t column, const char* file_name) {
  Position* position = malloc(sizeof(Position));
  check_memory_is_not_null(position);

  position->line = line;
  position->column = column;
  position->file_name = strdup(file_name);

  return position;
}

void position_free(Position *position) {
  free((void*)position->file_name);
  free(position);
}

Position* position_copy(const Position* position) {
  return position_init(position->line, position->column, position->file_name);
}
