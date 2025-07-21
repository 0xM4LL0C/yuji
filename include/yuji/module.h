#pragma once

#include "yuji/types/dyn_array.h"
#include "yuji/types/map.h"
#include "yuji/value.h"

typedef struct YujiModule {
  const char* name;
  DynArr* submodules;
  Map* env;
} YujiModule;


YujiModule* module_init(const char* name);
void module_free(YujiModule* module);

void module_add_submodule(YujiModule* module, YujiModule* submodule);
void module_register_function(YujiModule* module, const char* name,
                              YujiCFunction func);
