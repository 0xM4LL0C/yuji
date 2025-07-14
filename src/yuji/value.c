#include "yuji/value.h"
#include "yuji/ast.h"
#include "yuji/utils.h"
#include <stdlib.h>
#include <string.h>

void value_free(YujiValue* value) {
  switch (value->type) {
    case VT_NUMBER:
    case VT_FUNCTION:
      break;

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
