#pragma once

#include "yuji/core/types/dyn_array.h"
#include <stddef.h>

typedef struct {
  char* data;
  size_t size;
  size_t capacity;
} YujiString;

YujiString* yuji_string_init();
YujiString* yuji_string_init_from_cstr(const char* cstr);
void yuji_string_free(YujiString* str);

void yuji_string_append_char(YujiString* sstr, char char_);
void yuji_string_append_cstr(YujiString* str, const char* cstr);
void yuji_string_append(YujiString* str, const char* buf, size_t len);

YujiDynArray* yuji_string_split(YujiString* str, char delim);
