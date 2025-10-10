#include "yuji/core/value.h"
#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include <stdio.h>
#include <string.h>

void yuji_value_free(YujiValue* value) {
  yuji_check_memory(value);

  if (--value->refcount != 0) {
    return;
  }

  switch (value->type) {
    case VT_INT:
    case VT_FLOAT:
    case VT_FUNCTION:
    case VT_BOOL:
    case VT_NULL:
      break;

    case VT_ARRAY:
      YUJI_DYN_ARRAY_ITER(value->value.array, YujiValue, element, {
        yuji_value_free(element);
      })
      yuji_dyn_array_free(value->value.array);
      break;

    case VT_STRING:
      yuji_string_free(value->value.string);
      break;

    case VT_CFUNCTION:
      yuji_free(value->value.cfunction);
      break;
  }

  yuji_free(value);
}

bool yuji_value_to_bool(YujiValue* value) {
  switch (value->type) {
    case VT_INT:
      return value->value.int_ != 0;

    case VT_FLOAT:
      return value->value.float_ != 0.0;

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
    case VT_INT: {
      size_t len = (size_t)snprintf(NULL, 0, "%lld", (long long)value->value.int_);
      result = yuji_malloc(len + 1);
      snprintf(result, len + 1, "%lld", (long long)value->value.int_);
      break;
    }

    case VT_FLOAT: {
      char buf[64];
      snprintf(buf, sizeof(buf), "%.15f", value->value.float_);
      char* end = buf + strlen(buf) - 1;

      while (end > buf && *end == '0') {
        end--;
      }

      if (*end == '.') {
        end--;
      }

      *(end + 1) = '\0';
      size_t len = strlen(buf);
      result = yuji_malloc(len + 1);
      memcpy(result, buf, len + 1);
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

    case VT_ARRAY: {
      YujiDynArray* array = value->value.array;
      YujiString* str = yuji_string_init();

      yuji_string_append_cstr(str, "[");

      size_t i = 0;
      YUJI_DYN_ARRAY_ITER(array, YujiValue, element, {
        char* element_str = yuji_value_to_string(element);
        yuji_string_append_cstr(str, element_str);
        yuji_free(element_str);

        if (i < array->size - 1) {
          yuji_string_append_cstr(str, ", ");
        }
        i++;
      })

      yuji_string_append_cstr(str, "]");

      result = yuji_malloc(str->size + 1);
      strcpy(result, str->data);
      yuji_string_free(str);
      break;
    }
  }

  return result;
}


char* yuji_value_type_to_string(YujiValueType type) {
  switch (type) {
    case VT_INT:
      return "int";

    case VT_FLOAT:
      return "float";

    case VT_STRING:
      return "string";

    case VT_BOOL:
      return "bool";

    case VT_NULL:
      return "null";

    case VT_FUNCTION:
    case VT_CFUNCTION:
      return "function";

    case VT_ARRAY:
      return "array";
  }

  yuji_panic("Unknown value type: %d", type);
}

bool yuji_value_type_is(YujiValueType type, YujiValueType expected) {
  return type == expected;
}

YUJI_VALUE_INIT(int, VT_INT, {
  value->value.int_ = number;
}, int64_t number)

YUJI_VALUE_INIT(float, VT_FLOAT, {
  value->value.float_ = number;
}, double number)

YUJI_VALUE_INIT(function, VT_FUNCTION, {
  value->value.function.node = node;
}, YujiASTFunction* node)

YUJI_VALUE_INIT(string, VT_STRING, {
  value->value.string = yuji_string_init_from_cstr(string->data);
}, YujiString* string)

YUJI_VALUE_INIT(null, VT_NULL, {})

YUJI_VALUE_INIT(cfunction, VT_CFUNCTION, {
  YujiCFunction* cfunction = yuji_malloc(sizeof(YujiCFunction));

  cfunction->argc = argc;
  cfunction->func = func;
  value->value.cfunction = cfunction;

}, size_t argc, YujiValue * (*func)(struct YujiScope* scope, YujiDynArray* args))

YUJI_VALUE_INIT(bool, VT_BOOL, {
  value->value.bool_ = bool_;
}, bool bool_)

YUJI_VALUE_INIT(array, VT_ARRAY, {
  value->value.array = array;
}, YujiDynArray* array)
