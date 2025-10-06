#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include <time.h>

static YujiValue* time_time(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 0) {
    yuji_panic("now function takes no arguments");

  }

  return yuji_value_int_init(time(NULL));
}

static YujiValue* time_sleep(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("sleep function takes one argument");

  }

  YujiValue* duration = yuji_dyn_array_get(args, 0);

  if (duration->type != VT_INT) {
    yuji_panic("sleep function takes an integer argument");
  }

  time_t seconds = duration->value.int_;
  struct timespec ts = {
    .tv_sec = seconds,
    .tv_nsec = 0
  };

  nanosleep(&ts, NULL);


  return yuji_value_null_init();
}

static YujiValue* time_sleepms(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("sleepms function takes one argument");

  }

  YujiValue* duration = yuji_dyn_array_get(args, 0);

  if (duration->type != VT_INT) {
    yuji_panic("sleepms function takes an integer argument");
  }

  time_t milliseconds = duration->value.int_;
  struct timespec ts = {
    .tv_sec = milliseconds / 1000,
    .tv_nsec = (milliseconds % 1000) * 1000000
  };

  nanosleep(&ts, NULL);


  return yuji_value_null_init();
}

YUJI_DEFINE_MODULE(time, {
  YUJI_MODULE_REGISTER_FUNC(module, "time", YUJI_FN_NO_ARGUMENT, time_time);
  YUJI_MODULE_REGISTER_FUNC(module, "sleep", YUJI_FN_ARGC(1), time_sleep);
  YUJI_MODULE_REGISTER_FUNC(module, "sleepms", YUJI_FN_ARGC(1), time_sleepms);
})
