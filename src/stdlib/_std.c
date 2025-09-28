#include "yuji/stdlib/_std.h"
#include "yuji/core/module.h"
#include "yuji/core/types/map.h"

extern YujiModule* yuji_load_io();
extern YujiModule* yuji_load_core();

void yuji_std_load_all(YujiInterpreter* interpreter) {
  YujiModule* std = yuji_module_init("std");

  YujiModule* libio = yuji_load_io();
  yuji_module_add_submodule(std, libio);

  YujiModule* libcore = yuji_load_core();
  yuji_module_add_submodule(std, libcore);

  yuji_map_insert(interpreter->loaded_modules, std->name, std);
}
