#include "yuji/value.h"
#include "yuji/ast.h"
#include "yuji/utils.h"
#include <stdlib.h>

void value_free(YujiValue* value) {
  switch (value->type) {
    case VT_NUMBER:
    case VT_FUNCTION:
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
