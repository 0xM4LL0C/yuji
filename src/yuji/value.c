#include "yuji/value.h"
#include "yuji/ast.h"
#include "yuji/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void value_free(YujiValue* value) {
  if (!value) {
    return;
  }

  LOG("value_free: type=%d, ptr=%p", value->type, value);

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
  _INIT_VALUE(VT_NUMBER);
  value->value.number = number;
  return value;
}

YujiValue* value_function_init(ASTNode* node) {
  _INIT_VALUE(VT_FUNCTION);
  value->value.function.node = node;
  return value;
}

YujiValue* value_string_init(const char* string) {
  _INIT_VALUE(VT_STRING);
  value->value.string = strdup(string);
  return value;
}

YujiValue* value_null_init() {
  static YujiValue null_value = { .type = VT_NULL };
  return &null_value;
}

YujiValue* value_cfunction_init(YujiCFunction func) {
  _INIT_VALUE(VT_CFUNCTION);
  value->value.cfunction = func;
  return value;
}

YujiValue* value_bool_init(bool bool_) {
  _INIT_VALUE(VT_BOOL);
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

YujiValue* value_copy(YujiValue* value) {
  switch (value->type) {
    case VT_BOOL:
      return value_bool_init(value->value.bool_);

    case VT_CFUNCTION:
      return value_cfunction_init(value->value.cfunction);

    case VT_FUNCTION:
      return value_function_init(value->value.function.node);

    case VT_NUMBER:
      return value_number_init(value->value.number);

    case VT_STRING:
      return value_string_init(value->value.string);

    case VT_NULL:
      return value_null_init();
  }

  LOG("Unsupported value type for copying: %d", value->type);
  panic("Unsupported value type for copying");
}
