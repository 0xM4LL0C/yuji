#include "yuji/core/value.h"
#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/string.h"
#include <stdio.h>
#include <string.h>

void yuji_value_free(YujiValue* value) {
  yuji_check_memory(value);

  if (--value->refcount != 0) {
    return;
  }

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

char* yuji_value_to_string(YujiValue* value) {
  yuji_check_memory(value);

  char* result = NULL;

  switch (value->type) {
    case VT_NUMBER: {
      size_t len = (size_t)snprintf(NULL, 0, "%lld", (long long)value->value.number);
      result = yuji_malloc(len + 1);
      snprintf(result, len + 1, "%lld", (long long)value->value.number);
      break;
    }

    case VT_STRING:
      result = yuji_malloc(value->value.string->size + 1);
      strcpy(result, value->value.string->data);
      break;

    case VT_BOOL:
      result = yuji_malloc((value->value.bool_ ? strlen("true") : strlen("false")) + 1);
      strcpy(result, value->value.bool_ ? "true" : "false");
      break;

    case VT_NULL:
      result = yuji_malloc(strlen("null") + 1);
      strcpy(result, "null");
      break;

    case VT_FUNCTION:
      result = yuji_malloc(32);
      snprintf(result, 32, "<function:%p>", (void*)value->value.function.node);
      break;

    case VT_CFUNCTION:
      result = yuji_malloc(32);
      snprintf(result, 32, "<cfunction:%p>", (void*)value->value.cfunction);
      break;
  }

  return result;
}

YUJI_VALUE_INIT(number, VT_NUMBER, {
  value->value.number = number;
}, int64_t number)

YUJI_VALUE_INIT(function, VT_FUNCTION, {
  value->value.function.node = node;
}, YujiASTFunction* node)

YUJI_VALUE_INIT(string, VT_STRING, {
  value->value.string = yuji_string_init_from_cstr(strdup(string->data));
}, YujiString* string)

YUJI_VALUE_INIT(null, VT_NULL, {})

YUJI_VALUE_INIT(cfunction, VT_CFUNCTION, {
  value->value.cfunction = cfunction;
}, YujiCFunction cfunction)

YUJI_VALUE_INIT(bool, VT_BOOL, {
  value->value.bool_ = bool_;
}, bool bool_)
