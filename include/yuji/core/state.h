#pragma once

#include "yuji/core/ast.h"
#include "yuji/core/interpreter.h"

typedef struct {
  YujiInterpreter* interpreter;
} YujiState;

YujiState* yuji_state_init();
void yuji_state_free(YujiState* state);

YujiASTNode* yuji_get_ast(const char* string, const char* source_name);
YujiASTNode* yuji_get_ast_from_file(const char* filename);

void yuji_eval_string(YujiState* state, const char* string);
void yuji_eval_file(YujiState* state, const char* filename);

void yuji_print_call_stack(YujiState* state);
