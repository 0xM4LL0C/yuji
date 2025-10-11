#include "yuji/core/interpreter.h"
#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"

static YujiValue* array_len(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("len function expects 1 argument");
  }

  YujiValue* array = yuji_dyn_array_get(args, 0);

  if (array->type != VT_ARRAY) {
    yuji_panic("len function expects an array");
  }

  return yuji_value_int_init((int64_t)array->value.array->size);
}

static YujiValue* array_push(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 2) {
    yuji_panic("push function expects 2 arguments");
  }

  YujiValue* array = yuji_dyn_array_get(args, 0);
  YujiValue* value = yuji_dyn_array_get(args, 1);

  if (array->type != VT_ARRAY) {
    yuji_panic("push function expects an array");
  }

  value->refcount++;
  yuji_dyn_array_push(array->value.array, value);

  return yuji_value_null_init();
}

static YujiValue* array_pop(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("pop function expects 1 argument");
  }

  YujiValue* array = yuji_dyn_array_get(args, 0);

  if (array->type != VT_ARRAY) {
    yuji_panic("pop function expects an array");
  }

  if (array->value.array->size == 0) {
    yuji_panic("pop function called on empty array");
  }

  YujiValue* value = yuji_dyn_array_pop(array->value.array);

  return value;
}

YUJI_DEFINE_MODULE(array, {
  YUJI_MODULE_REGISTER_FUNC(module, "len", YUJI_FN_ARGC(1), array_len);
  YUJI_MODULE_REGISTER_FUNC(module, "push", YUJI_FN_ARGC(2), array_push);
  YUJI_MODULE_REGISTER_FUNC(module, "pop", YUJI_FN_ARGC(1), array_pop);
})
