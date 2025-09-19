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
#define _YUJI_POSITION_TO_STRING_SNPRINTF(OUT, SIZE) \
  snprintf(OUT, SIZE, "%s:%zu:%zu", position.file, position.line + 1, position.column + 1)

  unsigned int size = (unsigned int)_YUJI_POSITION_TO_STRING_SNPRINTF(NULL, 0);
  char* string = (char*)malloc((unsigned int)(size + 1));

  _YUJI_POSITION_TO_STRING_SNPRINTF(string, size + 1);

#undef _YUJI_POSITION_TO_STRING_SNPRINTF

  return string;
}
