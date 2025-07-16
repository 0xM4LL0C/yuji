#pragma once

#include "yuji/dyn_array.h"
#include "yuji/map.h"
#include "yuji/value.h"

typedef struct YujiModule {
  const char* name;
  DynArr* submodules;
  Map* env;
} YujiModule;

typedef YujiValue* (*YujiModuleFunction)(DynArr* args);

YujiModule* module_init(const char* name);
void module_free(YujiModule* module);

void module_add_submodule(YujiModule* module, YujiModule* submodule);
void module_register_function(YujiModule* module, const char* name,
                              YujiModuleFunction func);
