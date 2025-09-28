#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include "yuji/core/value.h"
#include "yuji/core/interpreter.h"
#include "yuji/utils.h"
#include <stdlib.h>

static YujiValue* core_not(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("not function expects 1 argument, got %d", args->size);
  }

  YujiValue* value = yuji_dyn_array_get(args, 0);
  return yuji_value_bool_init(!yuji_value_to_bool(value));
}

static YujiValue* core_typeof(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("typeof function expects 1 argument, got %d", args->size);
  }

  YujiValue* value = yuji_dyn_array_get(args, 0);
  char* str = yuji_value_to_string(value);
  return yuji_value_string_init(yuji_string_init_from_cstr(str));
}

static YujiValue* core_assert(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size < 1) {
    yuji_panic("assert function expects 1 argument, got %d", args->size);
  } else if (args->size > 2) {
    yuji_panic("assert function maximum 2 arguments, got %d", args->size);
  }

  YujiValue* condition = yuji_dyn_array_get(args, 0);
  YujiValue* message = yuji_dyn_array_get(args, 1);

  char* msg = NULL;

  if (message) {
    msg = yuji_value_to_string(message);
  } else {
    msg = yuji_value_to_string(condition);
  }

  if (!yuji_value_to_bool(condition)) {
    yuji_panic("Assertion failed: %s", msg);
  }

  yuji_free(msg);

  return yuji_value_null_init();
}


static YujiValue* core_panic(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("panic function expects 1 argument, got %d", args->size);
  }

  YujiValue* message = yuji_dyn_array_get(args, 0);
  char* msg = yuji_value_to_string(message);

  yuji_panic("%s", msg);

  yuji_free(msg);

  return yuji_value_null_init();
}

static YujiValue* core_exit(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("exit function expects 1 argument, got %d", args->size);
  }

  YujiValue* code = yuji_dyn_array_get(args, 0);

  if (code->type != VT_INT) {
    yuji_panic("exit function expects an int argument, got %s", yuji_value_to_string(code));
  }

  int exit_code = (int)code->value.int_;

  exit(exit_code);

  return yuji_value_null_init();
}

YujiModule* yuji_load_core() {
  YujiModule* module = yuji_module_init("core");

  YUJI_MODULE_REGISTER_FUNC(module, "not", YUJI_FN_ARGC(1), core_not);
  YUJI_MODULE_REGISTER_FUNC(module, "typeof", YUJI_FN_ARGC(1), core_typeof);
  YUJI_MODULE_REGISTER_FUNC(module, "assert", YUJI_FN_INF_ARGUMENT, core_assert);
  YUJI_MODULE_REGISTER_FUNC(module, "panic", YUJI_FN_ARGC(1), core_panic);
  YUJI_MODULE_REGISTER_FUNC(module, "exit", YUJI_FN_ARGC(1), core_exit);

  return module;
}
