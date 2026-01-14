#pragma once

#include "yuji/core/interpreter.h"
#include "yuji/core/types/map.h"
#include <stdint.h>

typedef struct {
  const char* name;
  YujiScope* scope;
  YujiMap* submodules;
} YujiModule;

YujiModule* yuji_module_init(const char* name);
void yuji_module_free(YujiModule* module);

void yuji_module_add_submodule(YujiModule* module, YujiModule* submodule);
void yuji_module_register(YujiModule* module, const char* name, YujiValue *value);

YujiModule* yuji_module_find_submodule(YujiModule* module, const char* name);

#define YUJI_FN_ARGC(N) N
#define YUJI_FN_NO_ARGUMENT YUJI_FN_ARGC(0)
#define YUJI_FN_INF_ARGUMENT YUJI_FN_ARGC(SIZE_MAX)

#define YUJI_MODULE_REGISTER(TYPE, MODULE, NAME, ...) \
  do { \
    YujiValue* tmp = yuji_value_##TYPE##_init(__VA_ARGS__); \
    yuji_module_register(MODULE, NAME, tmp); \
    yuji_value_free(tmp); \
  } while (0)


// deprecated: use YUJI_MODULE_REGISTER instant
#define YUJI_MODULE_REGISTER_FUNC(MODULE, NAME, ARGC, FUNC) \
  YUJI_MODULE_REGISTER(cfunction, MODULE, NAME, ARGC, FUNC)

#define YUJI_DEFINE_MODULE(NAME, BODY) \
  YujiModule* yuji_load_##NAME() {\
    YujiModule* module = yuji_module_init(#NAME);\
    BODY \
    return module; \
  }
