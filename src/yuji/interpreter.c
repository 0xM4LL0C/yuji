#include "yuji/interpreter.h"
#include "yuji/map.h"
#include "yuji/module.h"
#include "yuji/stdlib/io.h"
#include "yuji/utils.h"

#include <stdlib.h>

Interpreter* interpreter_init() {
  Interpreter* interpreter = malloc(sizeof(Interpreter));

  check_memory_is_not_null(interpreter);

  interpreter->env = map_init();
  interpreter->loaded_modules = map_init();

  interpreter_init_stdlib(interpreter);
  return interpreter;
}

void interpreter_free(Interpreter* interpreter) {
  map_free(interpreter->env);
  map_free(interpreter->loaded_modules);

  free(interpreter);
}

void interpreter_init_stdlib(Interpreter* interpreter) {
  YujiModule* io_module = yuji_module_io_init();
  map_insert(interpreter->loaded_modules, io_module->name, io_module);
}

void interpreter_load_module(Interpreter* interpreter,
                             const char* module_name) {
  YujiModule* module = map_get(interpreter->loaded_modules, module_name);
  map_insert(interpreter->loaded_modules, module->name, module);

  if (!module) {
    panic("Module not found: %s", module_name);
  }
}
