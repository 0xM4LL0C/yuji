#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include <math.h>
#include <stdlib.h>

static YujiValue* math_sin(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("sin function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("sin function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(sin(value));
}

static YujiValue* math_cos(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("cos function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("cos function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(cos(value));
}

static YujiValue* math_tan(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("tan function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("tan function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(tan(value));
}

static YujiValue* math_atan(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("atan function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("atan function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(atan(value));
}

static YujiValue* math_pow(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 2) {
    yuji_panic("pow function expects exactly two arguments");
  }

  YujiValue* base = yuji_dyn_array_get(args, 0);
  YujiValue* exponent = yuji_dyn_array_get(args, 1);

  if (base->type != VT_INT && base->type != VT_FLOAT) {
    yuji_panic("pow function expects a int or float as base");
  }

  if (exponent->type != VT_INT && exponent->type != VT_FLOAT) {
    yuji_panic("pow function expects a int or float as exponent");
  }

  double base_value = base->type == VT_INT ? (double)base->value.int_ : base->value.float_;
  double exponent_value = exponent->type == VT_INT ? (double)exponent->value.int_ :
                          exponent->value.float_;

  return yuji_value_float_init(pow(base_value, exponent_value));
}

static YujiValue* math_sqrt(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("sqrt function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("sqrt function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(sqrt(value));
}

static YujiValue* math_abs(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("abs function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("abs function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(fabs(value));
}

static YujiValue* math_floor(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("floor function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("floor function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(floor(value));
}

static YujiValue* math_ceil(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("ceil function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("ceil function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(ceil(value));
}

static YujiValue* math_round(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("round function expects exactly one argument");
  }

  YujiValue* arg = yuji_dyn_array_get(args, 0);

  if (arg->type != VT_INT && arg->type != VT_FLOAT) {
    yuji_panic("round function expects a int or float");
  }

  double value = arg->type == VT_INT ? (double)arg->value.int_ : arg->value.float_;

  return yuji_value_float_init(round(value));
}

static YujiValue* math_random(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 2) {
    yuji_panic("random function expects exactly two arguments");
  }

  YujiValue* min_arg = yuji_dyn_array_get(args, 0);
  YujiValue* max_arg = yuji_dyn_array_get(args, 1);

  if (min_arg->type != VT_INT || max_arg->type != VT_INT) {
    yuji_panic("random function expects integers");
  }

  int64_t min = min_arg->value.int_;
  int64_t max = max_arg->value.int_;

  if (min >= max) {
    yuji_panic("min must be less than max");
  }

  int64_t range = max - min;
  return yuji_value_int_init(min + (rand() % range));
}

YUJI_DEFINE_MODULE(math, {
  YUJI_MODULE_REGISTER_FUNC(module, "sin", YUJI_FN_ARGC(1), math_sin);
  YUJI_MODULE_REGISTER_FUNC(module, "cos", YUJI_FN_ARGC(1), math_cos);
  YUJI_MODULE_REGISTER_FUNC(module, "tan", YUJI_FN_ARGC(1), math_tan);
  YUJI_MODULE_REGISTER_FUNC(module, "atan", YUJI_FN_ARGC(1), math_atan);
  YUJI_MODULE_REGISTER_FUNC(module, "pow", YUJI_FN_ARGC(2), math_pow);
  YUJI_MODULE_REGISTER_FUNC(module, "sqrt", YUJI_FN_ARGC(1), math_sqrt);
  YUJI_MODULE_REGISTER_FUNC(module, "abs", YUJI_FN_ARGC(1), math_abs);
  YUJI_MODULE_REGISTER_FUNC(module, "floor", YUJI_FN_ARGC(1), math_floor);
  YUJI_MODULE_REGISTER_FUNC(module, "ceil", YUJI_FN_ARGC(1), math_ceil);
  YUJI_MODULE_REGISTER_FUNC(module, "round", YUJI_FN_ARGC(1), math_round);
  YUJI_MODULE_REGISTER_FUNC(module, "random", YUJI_FN_ARGC(2), math_random);
})
