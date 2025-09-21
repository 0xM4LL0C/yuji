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
} YujiInterpreter;

YujiScope* yuji_scope_init(YujiScope* parent);
void yuji_scope_free(YujiScope* scope);
void yuji_scope_push(YujiInterpreter* interpreter) ;
void yuji_scope_pop(YujiInterpreter* interpreter) ;
YujiValue* yuji_scope_get(YujiScope* scope, const char* key) ;
void yuji_scope_set(YujiScope* scope, const char* key, YujiValue* val) ;

YujiInterpreter* yuji_interpreter_init();
void yuji_interpreter_free(YujiInterpreter* interpreter);

YujiValue* yuji_interpreter_eval(YujiInterpreter* interpreter, YujiASTNode* node);
