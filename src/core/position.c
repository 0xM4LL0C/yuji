#include "yuji/core/position.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

YujiPosition yuji_position_init(size_t line, size_t column, const char* file) {
  YujiPosition position = {
    .line = line,
    .column = column,
    .file = file,
  };

  return position;
}


char* yuji_position_to_string(const YujiPosition position) {
  int size = snprintf(NULL, 0, "%s:%zu:%zu", position.file, position.line + 1, position.column + 1);
  char* string = (char*)malloc((unsigned int)(size + 1));
  snprintf(string, (unsigned int)(size + 1), "%s:%zu:%zu", position.file, position.line + 1,
           position.column + 1);
  return string;
}
