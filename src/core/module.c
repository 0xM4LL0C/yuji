#include "yuji/core/interpreter.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"
#include <yuji/core/module.h>

YujiModule* yuji_module_init(const char* name) {
  YujiModule* module = yuji_malloc(sizeof(YujiModule));

  module->name = name;
  module->submodules = yuji_dyn_array_init();
  module->scope = yuji_scope_init(NULL);

  return module;
}

void yuji_module_free(YujiModule* module) {
  YUJI_DYN_ARRAY_ITER(module->submodules, YujiModule, submodule, {
    yuji_module_free(submodule);
  })

  yuji_dyn_array_free(module->submodules);
  yuji_scope_free(module->scope);
  yuji_free(module);
}

void yuji_module_add_submodule(YujiModule* module, YujiModule* submodule) {
  yuji_dyn_array_push(module->submodules, submodule);
}

void yuji_module_register(YujiModule* module, const char* name, YujiValue* value) {
  if (yuji_scope_get(module->scope, name)) {
    yuji_panic("names %s already exists in module '%s'", name, module->name);
  }

  yuji_scope_set(module->scope, name, value);
}
