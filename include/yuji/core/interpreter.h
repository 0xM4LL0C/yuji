#pragma once

#include "yuji/core/ast.h"
#include "yuji/core/types/map.h"
#include "yuji/core/value.h"

typedef struct YujiScope {
  YujiMap* env;
  struct YujiScope* parent;
} YujiScope;

typedef struct {
  YujiScope* current_scope;
  YujiMap* loaded_modules;
} YujiInterpreter;

YujiScope* yuji_scope_init(YujiScope* parent);
void yuji_scope_free(YujiScope* scope);
void yuji_scope_push(YujiInterpreter* interpreter);
void yuji_scope_pop(YujiInterpreter* interpreter);
YujiValue* yuji_scope_get(YujiScope* scope, const char* key);
void yuji_scope_set(YujiScope* scope, const char* key, YujiValue* val);
void yuji_scope_merge(YujiScope* dest, YujiScope* src);

YujiInterpreter* yuji_interpreter_init();
void yuji_interpreter_free(YujiInterpreter* interpreter);

void yuji_interpreter_load_module(YujiInterpreter* interpreter, const char* module_name);
YujiValue* yuji_interpreter_eval(YujiInterpreter* interpreter, YujiASTNode* node);
