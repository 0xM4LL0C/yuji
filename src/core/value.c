#include "yuji/core/value.h"
#include "yuji/core/ast.h"
#include "yuji/core/memory.h"

void yuji_value_free(YujiValue* value) {
  yuji_check_memory(value);

  switch (value->type) {
    case VT_NUMBER:
    case VT_FUNCTION:
    case VT_CFUNCTION:
    case VT_BOOL:
    case VT_NULL:
      break;

    case VT_STRING:
      yuji_string_free(value->value.string);
      break;
  }

  yuji_free(value);
}

bool yuji_value_to_bool(YujiValue* value) {
  switch (value->type) {
    case VT_NUMBER:
      return value->value.number != 0;

    case VT_BOOL:
      return value->value.bool_;

    case VT_STRING:
      return value->value.string->size > 0;

    default:
      return false;
  }
}

YUJI_VALUE_INIT(number, VT_NUMBER, {
  value->value.number = number;
}, int64_t number)

YUJI_VALUE_INIT(function, VT_FUNCTION, {
  value->value.function.node = node;
}, YujiASTFunction* node)

YUJI_VALUE_INIT(string, VT_STRING, {
  value->value.string = string;
}, YujiString* string)

YujiValue* yuji_value_null_init() {
  static YujiValue *global_null = NULL;

  if (!global_null) {
    global_null = yuji_malloc(sizeof(YujiValue));
    global_null->type = VT_NULL;
  }

  return global_null;
}

YUJI_VALUE_INIT(cfunction, VT_CFUNCTION, {
  value->value.cfunction = cfunction;
}, YujiCFunction cfunction)

YUJI_VALUE_INIT(bool, VT_BOOL, {
  value->value.bool_ = bool_;
}, bool bool_)
