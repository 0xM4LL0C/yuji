#include "yuji/value.h"
#include "yuji/utils.h"
#include <stdlib.h>

void value_free(YujiValue* value) {
  switch (value->type) {
    case VT_NUMBER:
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
