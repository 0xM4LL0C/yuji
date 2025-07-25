#include "yuji/value.h"
#include "yuji/ast.h"
#include "yuji/utils.h"
#include <stdlib.h>
#include <string.h>

void value_free(YujiValue* value) {

  switch (value->type) {
    case VT_NUMBER:
    case VT_FUNCTION:
    case VT_CFUNCTION:
    case VT_BOOL:
      break;

    case VT_NULL:
      return;

    case VT_STRING:
      free(value->value.string);
      break;
  }

  free(value);
}

YujiValue* value_number_init(int number) {
  YujiValue* value = malloc(sizeof(YujiValue));
  check_memory_is_not_null(value);
  value->type = VT_NUMBER;
  value->value.number = number;
  return value;
}

YujiValue* value_function_init(ASTNode* node) {
  YujiValue* value = malloc(sizeof(YujiValue));
  check_memory_is_not_null(value);
  value->type = VT_FUNCTION;
  value->value.function.node = node;
  return value;
}

YujiValue* value_string_init(const char* string) {
  YujiValue* value = malloc(sizeof(YujiValue));
  check_memory_is_not_null(value);
  value->type = VT_STRING;
  value->value.string = strdup(string);
  return value;
}

YujiValue* value_null_init() {
  static YujiValue null_value = { .type = VT_NULL };
  return &null_value;
}

YujiValue* value_cfunction_init(YujiCFunction func) {
  YujiValue* value = malloc(sizeof(YujiValue));
  check_memory_is_not_null(value);
  value->type = VT_CFUNCTION;
  value->value.cfunction = func;
  return value;
}

YujiValue* value_bool_init(bool bool_) {
  YujiValue* value = malloc(sizeof(YujiValue));
  check_memory_is_not_null(value);

  value->type = VT_BOOL;
  value->value.bool_ = bool_;

  return value;
}

bool value_to_bool(YujiValue* value) {
  switch (value->type) {
    case VT_BOOL:
      return value->value.bool_;

    case VT_CFUNCTION:
    case VT_FUNCTION:
      return true;

    case VT_NUMBER:
      return value->value.number != 0;

    case VT_STRING:
      return strlen(value->value.string) > 0;

    case VT_NULL:
      return false;
  }

  return false;
}
