#include "yuji/stdlib/io.h"
#include "yuji/types/dyn_array.h"
#include "yuji/types/map.h"
#include "yuji/module.h"
#include "yuji/utils.h"
#include "yuji/value.h"
#include <stdio.h>
#include <string.h>

YujiValue* io_print(DynArr* args) {
  DYN_ARR_ITER(args, YujiValue, value, {
    switch (value->type) {
      case VT_STRING:
        printf("%s", value->value.string);
        break;

      case VT_NUMBER:
        printf("%d", value->value.number);
        break;

      case VT_NULL:
        printf("null");
        break;

      case VT_BOOL:
        printf("%s", value->value.bool_ ? "true" : "false");
        break;

      default:
        printf("<unknown>");
        break;
    }
  })

  return value_null_init();
}

YujiValue* io_println(DynArr* args) {
  io_print(args);
  printf("\n");
  return value_null_init();
}

YujiValue* io_input(DynArr* args) {
  if (args->size == 0) {
    panic("input() requires at least one argument");
  } else if (args->size > 1) {
    panic("input() requires at most one argument");
  }

  io_print(args);

  char buffer[1024];
  fgets(buffer, sizeof(buffer), stdin);
  buffer[strcspn(buffer, "\n")] = '\0';

  return value_string_init(buffer);
}


YujiModule* yuji_module_io_init() {
  YujiModule* module = module_init("io");

  map_insert(module->env, "print", io_print);
  map_insert(module->env, "println", io_println);
  map_insert(module->env, "input", io_input);

  return module;
}
