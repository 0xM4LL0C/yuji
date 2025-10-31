#include "yuji/core/interpreter.h"
#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include <stdlib.h>

static YujiValue* os_system(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("system function expects 1 argument, got %ld", args->size);
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_STRING) {
    yuji_panic("system function expects a string argument");
  }

  int result = system(arg->value.string->data);
  return yuji_value_int_init(result);
}

static YujiValue* os_setenv(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 2) {
    yuji_panic("setenv function expects 2 arguments, got %ld", args->size);
  }

  YujiValue* key = yuji_dyn_array_get(args, 0);
  YujiValue* value = yuji_dyn_array_get(args, 1);

  if (key->type != VT_STRING || value->type != VT_STRING) {
    yuji_panic("setenv function expects string arguments");
  }

  int result = setenv(key->value.string->data, value->value.string->data, 1);
  return yuji_value_int_init(result);
}

static YujiValue* os_getenv(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("getenv function expects 1 argument, got %ld", args->size);
  }

  YujiValue* key = yuji_dyn_array_get(args, 0);

  if (key->type != VT_STRING) {
    yuji_panic("getenv function expects a string argument");
  }

  char* result = getenv(key->value.string->data);
  return yuji_value_string_init(yuji_string_init_from_cstr(result));
}


YUJI_DEFINE_MODULE(os, {
  YUJI_MODULE_REGISTER_FUNC(module, "system", YUJI_FN_ARGC(1), os_system);
  YUJI_MODULE_REGISTER_FUNC(module, "setenv", YUJI_FN_ARGC(2), os_setenv);
  YUJI_MODULE_REGISTER_FUNC(module, "getenv", YUJI_FN_ARGC(1), os_getenv);
})
