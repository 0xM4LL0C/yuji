#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include <stdio.h>
#include <stdio.h>

static YujiValue* io_print(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size == 0) {
    yuji_panic("print function requires at least one argument");
  }

  YUJI_DYN_ARRAY_ITER(args, YujiValue, arg, {
    printf("%s", yuji_value_to_string(arg));
  })

  return yuji_value_null_init();
}

static YujiValue* io_println(YujiScope* scope, YujiDynArray* args) {
  io_print(scope, args);
  printf("\n");
  return yuji_value_null_init();
}

static YujiValue* io_input(YujiScope* scope, YujiDynArray* args) {
  if (args->size == 1) {
    io_print(scope, args);
  }

  YujiString* result = yuji_string_init();
  char buffer[1024];

  while (fgets(buffer, sizeof(buffer), stdin)) {
    yuji_string_append_cstr(result, buffer);
  }

  return yuji_value_string_init(result);
}

YujiModule* yuji_load_io() {
  YujiModule* module = yuji_module_init("io");

  YUJI_MODULE_REGISTER_FUNC(module, "print", io_print);
  YUJI_MODULE_REGISTER_FUNC(module, "println", io_println);
  YUJI_MODULE_REGISTER_FUNC(module, "input", io_input);

  return module;
}
