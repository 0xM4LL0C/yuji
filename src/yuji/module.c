#include "yuji/module.h"
#include "yuji/types/dyn_array.h"
#include "yuji/types/map.h"
#include "yuji/utils.h"
#include <stdlib.h>
#include <string.h>


YujiModule* module_init(const char* name) {
  YujiModule* module = malloc(sizeof(YujiModule));
  check_memory_is_not_null(module);

  module->name = strdup(name);
  module->submodules = dyn_array_init();
  module->env = map_init();

  return module;
}

void module_free(YujiModule* module) {
  free((void*)module->name);

  DYN_ARR_ITER(module->submodules, YujiModule, submodule, {
    module_free(submodule);
  })

  dyn_array_free(module->submodules);
  map_free(module->env);
  free(module);
}

void module_add_submodule(YujiModule* module, YujiModule* submodule) {
  dyn_array_append(module->submodules, submodule);
}

void module_register_function(YujiModule* module, const char* name,
                              YujiCFunction func) {
  map_insert(module->env, name, func);
}
