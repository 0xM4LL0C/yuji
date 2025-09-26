// src/stdlib/libio.c
#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

static YujiValue* io_print(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size == 0) {
    yuji_panic("print function requires at least one argument");
  }

  YUJI_DYN_ARRAY_ITER(args, YujiValue, arg, {
    char* str = yuji_value_to_string(arg);
    printf("%s", str);
    yuji_free(str);
  })

  return yuji_value_null_init();
}

static YujiValue* io_println(YujiScope* scope, YujiDynArray* args) {
  YujiValue* result = io_print(scope, args);
  printf("\n");
  return result;
}

static YujiValue* io_input(YujiScope* scope, YujiDynArray* args) {
  if (args->size == 1) {
    YujiValue* tmp = io_print(scope, args);
    yuji_value_free(tmp);
  }

  YujiString* result = yuji_string_init();
  char buffer[1024];

  if (fgets(buffer, sizeof(buffer), stdin)) {
    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    yuji_string_append_cstr(result, buffer);
  }

  return yuji_value_string_init(result);
}

YujiModule* yuji_load_io() {
  YujiModule* module = yuji_module_init("io");

  YUJI_MODULE_REGISTER_FUNC(module, "print", YUJI_FN_INF_ARGUMENT, io_print);
  YUJI_MODULE_REGISTER_FUNC(module, "println", YUJI_FN_INF_ARGUMENT, io_println);
  YUJI_MODULE_REGISTER_FUNC(module, "input", YUJI_FN_ARGC(1), io_input);

  return module;
}
