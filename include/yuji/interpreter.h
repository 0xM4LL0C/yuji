#pragma once

#include "yuji/types/map.h"
typedef struct {
  Map* env;
  Map* loaded_modules;
} Interpreter;

Interpreter* interpreter_init();
void interpreter_free(Interpreter* interpreter);

void interpreter_init_stdlib(Interpreter* interpreter);
void interpreter_load_module(Interpreter* interpreter, const char* module_name);
