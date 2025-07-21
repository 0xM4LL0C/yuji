#pragma once

#include "yuji/ast.h"
#include "yuji/types/map.h"
#include "yuji/value.h"
typedef struct {
  Map* env;
  Map* loaded_modules;
} Interpreter;

Interpreter* interpreter_init();
void interpreter_free(Interpreter* interpreter);

void interpreter_init_stdlib(Interpreter* interpreter);
void interpreter_load_module(Interpreter* interpreter, const char* module_name);
YujiValue* interpreter_eval(Interpreter* interpreter, ASTNode* node);
