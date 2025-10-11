#include "yuji/stdlib/_std.h"
#include "yuji/core/module.h"
#include "yuji/core/types/map.h"

extern YujiModule* yuji_load_io();
extern YujiModule* yuji_load_core();
extern YujiModule* yuji_load_os();
extern YujiModule* yuji_load_time();
extern YujiModule* yuji_load_math();
extern YujiModule* yuji_load_array();

void yuji_std_load_all(YujiInterpreter* interpreter) {
  YujiModule* std = yuji_module_init("std");

  YujiModule* libio = yuji_load_io();
  yuji_module_add_submodule(std, libio);

  YujiModule* libcore = yuji_load_core();
  yuji_module_add_submodule(std, libcore);

  YujiModule* libos = yuji_load_os();
  yuji_module_add_submodule(std, libos);

  YujiModule* libtime = yuji_load_time();
  yuji_module_add_submodule(std, libtime);

  YujiModule* libmath = yuji_load_math();
  yuji_module_add_submodule(std, libmath);

  YujiModule* libarray = yuji_load_array();
  yuji_module_add_submodule(std, libarray);

  yuji_map_insert(interpreter->loaded_modules, std->name, std);
}
