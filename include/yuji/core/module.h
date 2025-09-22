#pragma once

#include "yuji/core/interpreter.h"
#include "yuji/core/types/map.h"

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

#define YUJI_MODULE_REGISTER_FUNC(module, name, func) \
  yuji_module_register(module, name, yuji_value_cfunction_init(func))
