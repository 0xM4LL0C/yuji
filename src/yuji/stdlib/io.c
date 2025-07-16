#include "yuji/stdlib/io.h"
#include "yuji/dyn_array.h"
#include "yuji/map.h"
#include "yuji/utils.h"
#include "yuji/value.h"
#include <stdio.h>
#include <stdlib.h>

YujiValue* io_print(DynArr* args) {
  for (size_t i = 0; i < args->size; i++) {
    YujiValue* value = dyn_array_get(args, i);

    switch (value->type) {
      case VT_STRING:
        printf("%s", value->value.string);
        break;

      case VT_NUMBER:
        printf("%d", value->value.number);
        break;

      default:
        printf("<unknown>");
        break;
    }
  }

  printf("\n");
  YujiValue* result = malloc(sizeof(YujiValue));
  result->type = VT_NULL;
  return result;
}


YujiModule* yuji_module_io_init() {
  YujiModule* module = malloc(sizeof(YujiModule));
  check_memory_is_not_null(module);
  module->name = "io";
  module->env = map_init();
  map_insert(module->env, "print", io_print);
  return module;
}
