#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"
#include <yuji/core/types/string.h>

YujiString* yuji_string_init() {
  YujiString* str = yuji_malloc(sizeof(YujiString));

  str->capacity = 16;
  str->size = 0;
  str->data = yuji_malloc(str->capacity);
  str->data[0] = '\0';

  return str;
}

YujiString* yuji_string_init_from_cstr(const char* cstr) {
  YujiString* str = yuji_string_init();

  while (*cstr) {
    yuji_string_append_char(str, *cstr++);
  }

  return str;
}

void yuji_string_free(YujiString* str) {
  yuji_free(str->data);
  yuji_free(str);
}

void yuji_string_append_char(YujiString* str, char char_) {
  if (str->size + 2 > str->capacity) {
    str->capacity *= 2;
    str->data = yuji_realloc(str->data, str->capacity);
  }

  str->data[str->size++] = char_;
  str->data[str->size] = '\0';
}

void yuji_string_append_cstr(YujiString* str, const char* cstr) {
  while (*cstr) {
    yuji_string_append_char(str, *cstr++);
  }
}

YujiDynArray* yuji_string_split(YujiString* str, char delim) {
  YujiDynArray* result = yuji_dyn_array_init();
  YujiString* buf = yuji_string_init();

  for (size_t i = 0; i < str->size; ++i) {
    if (str->data[i] == delim) {
      yuji_dyn_array_push(result, buf);
      buf = yuji_string_init();
    } else {
      yuji_string_append_char(buf, str->data[i]);
    }
  }

  yuji_dyn_array_push(result, buf);
  return result;
}
