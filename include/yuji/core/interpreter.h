#pragma once

#include "yuji/core/ast.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/map.h"
#include "yuji/core/types/stack.h"
#include "yuji/core/value.h"


typedef struct YujiScope {
  YujiMap* env;
  struct YujiScope* parent;
} YujiScope;

typedef struct {
  YujiScope* scope;
  char* function_name;
  YujiDynArray* args;
  bool has_return;
  YujiValue* return_value;
} YujiCallFrame;

typedef struct {
  bool has_break;
  bool has_continue;
} YujiLoopFrame;

typedef struct {
  YujiScope* current_scope;
  YujiMap* loaded_modules;
  YujiStack* call_stack;
  YujiStack* loop_stack;
  size_t max_stack_size;
} YujiInterpreter;

// SCOPE
YujiScope* yuji_scope_init(YujiScope* parent);
void yuji_scope_free(YujiScope* scope);
void yuji_scope_push(YujiInterpreter* interpreter);
void yuji_scope_pop(YujiInterpreter* interpreter);
YujiValue* yuji_scope_get(YujiScope* scope, const char* key);
void yuji_scope_set(YujiScope* scope, const char* key, YujiValue* val);
void yuji_scope_update(YujiScope* scope, const char* key, YujiValue* val);
void yuji_scope_merge(YujiScope* dest, YujiScope* src);

// CALL FRAME
YujiCallFrame* yuji_call_frame_init(YujiScope* scope, const char* name, YujiDynArray* args);
void yuji_call_frame_free(YujiCallFrame* frame);

// LOOP FRAME
YujiLoopFrame* yuji_loop_frame_init();
void yuji_loop_frame_free(YujiLoopFrame* frame);

// INTERPRETER
YujiInterpreter* yuji_interpreter_init();
void yuji_interpreter_free(YujiInterpreter* interpreter);

YujiValue* yuji_interpreter_eval_module(YujiInterpreter* interpreter, YujiASTModule* module);
YujiValue* yuji_interpreter_eval_block(YujiInterpreter* interpreter, YujiASTBlock* block);
YujiValue* yuji_interpreter_eval(YujiInterpreter* interpreter, YujiASTNode* node);
