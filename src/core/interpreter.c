#include "yuji/core/interpreter.h"
#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/map.h"
#include "yuji/core/types/stack.h"
#include "yuji/core/types/string.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include "yuji/stdlib/_std.h"
#include <stdio.h>
#include <string.h>


YujiScope* yuji_scope_init(YujiScope* parent) {
  YujiScope* scope = yuji_malloc(sizeof(YujiScope));

  scope->env = yuji_map_init();
  scope->parent = parent;

  return scope;
}

void yuji_scope_free(YujiScope* scope) {
  YUJI_DYN_ARRAY_ITER(scope->env->pairs, YujiMapPair, pair, {
    yuji_value_free(pair->value);
  })

  yuji_map_free(scope->env);
  yuji_free(scope);
}

void yuji_scope_push(YujiInterpreter* interpreter) {
  interpreter->current_scope = yuji_scope_init(interpreter->current_scope);
}

void yuji_scope_pop(YujiInterpreter* interpreter) {
  YujiScope* old = interpreter->current_scope;

  if (old) {
    interpreter->current_scope = old->parent;
    yuji_scope_free(old);
  }
}

YujiValue* yuji_scope_get(YujiScope* scope, const char* key) {
  for (YujiScope* s = scope; s; s = s->parent) {
    YujiValue* val = yuji_map_get(s->env, key);

    if (val) {
      val->refcount++;
      return val;
    }
  }

  return NULL;
}

void yuji_scope_set(YujiScope* scope, const char* key, YujiValue* val) {
  YujiValue* old_val = yuji_map_get(scope->env, key);

  if (old_val) {
    yuji_value_free(old_val);
  }

  val->refcount++;
  yuji_map_set(scope->env, key, val);
}

void yuji_scope_update(YujiScope* scope, const char* key, YujiValue* val) {
  for (YujiScope* s = scope; s; s = s->parent) {
    YujiValue* existing = yuji_map_get(s->env, key);

    if (existing) {
      yuji_value_free(existing);
      val->refcount++;
      yuji_map_set(s->env, key, val);
      return;
    }
  }

  yuji_scope_set(scope, key, val);
}

void yuji_scope_merge(YujiScope* dest, YujiScope* src) {
  YUJI_DYN_ARRAY_ITER(src->env->pairs, YujiMapPair, pair, {
    yuji_scope_set(dest, pair->key, pair->value);
  })
}

YujiCallFrame* yuji_call_frame_init(YujiScope* scope, const char* name, YujiDynArray* args) {
  YujiCallFrame* frame = yuji_malloc(sizeof(YujiCallFrame));

  frame->scope = scope;
  frame->function_name = strdup(name);
  frame->args = args;
  frame->has_return = false;
  frame->return_value = NULL;

  return frame;
}

void yuji_call_frame_free(YujiCallFrame* frame) {
  yuji_free(frame->function_name);

  if (frame->args) {
    YUJI_DYN_ARRAY_ITER(frame->args, YujiValue, arg, {
      yuji_value_free(arg);
    })
    yuji_dyn_array_free(frame->args);
  }

  if (frame->return_value) {
    yuji_value_free(frame->return_value);
  }

  yuji_free(frame);
}

YujiLoopFrame* yuji_loop_frame_init() {
  YujiLoopFrame* frame = yuji_malloc(sizeof(YujiLoopFrame));

  frame->has_break = false;
  frame->has_continue = false;

  return frame;
}

void yuji_loop_frame_free(YujiLoopFrame* frame) {
  yuji_free(frame);
}

YujiInterpreter* yuji_interpreter_init() {
  YujiInterpreter* interpreter = yuji_malloc(sizeof(YujiInterpreter));

  interpreter->current_scope = yuji_scope_init(NULL);
  interpreter->loaded_modules = yuji_map_init();
  interpreter->call_stack = yuji_stack_init();
  interpreter->loop_stack = yuji_stack_init();
  interpreter->max_stack_size = 10000;

  yuji_std_load_all(interpreter);

  return interpreter;
}

void yuji_interpreter_free(YujiInterpreter* interpreter) {
  while (interpreter->current_scope) {
    yuji_scope_pop(interpreter);
  }

  YUJI_DYN_ARRAY_ITER(interpreter->loaded_modules->pairs, YujiMapPair, pair, {
    yuji_module_free(pair->value);
  })
  yuji_map_free(interpreter->loaded_modules);
  yuji_stack_free(interpreter->call_stack);
  yuji_stack_free(interpreter->loop_stack);
  yuji_free(interpreter);
}

void yuji_interpreter_load_module(YujiInterpreter* interpreter, const char* module_name) {
  yuji_check_memory(interpreter);

  YujiString* module_name_s = yuji_string_init_from_cstr(module_name);
  YujiDynArray* parts = yuji_string_split(module_name_s, '/');

  if (parts->size == 0) {
    yuji_panic("empty module name");
  }

  YujiString* first = parts->data[0];
  YujiModule* module = yuji_map_get(interpreter->loaded_modules, first->data);

  if (!module) {
    yuji_panic("module '%s' not found", first->data);
  }

  for (size_t i = 1; i < parts->size; i++) {
    YujiString* part = parts->data[i];
    YujiModule* sub = yuji_module_find_submodule(module, part->data);

    if (!sub) {
      yuji_panic("module '%s' not found", module_name);
    }

    module = sub;
  }

  yuji_scope_merge(interpreter->current_scope, module->scope);

  YUJI_DYN_ARRAY_ITER(parts, YujiString, part, {
    yuji_string_free(part);
  })
  yuji_dyn_array_free(parts);
  yuji_string_free(module_name_s);
}

YujiValue* yuji_interpreter_eval_block(YujiInterpreter* interpreter, YujiASTBlock* block) {
  yuji_check_memory(interpreter);
  yuji_check_memory(block);

  yuji_scope_push(interpreter);
  YujiValue* result = yuji_value_null_init();

  YUJI_DYN_ARRAY_ITER(block->exprs, YujiASTNode, expr, {
    YujiValue* expr_result = yuji_interpreter_eval(interpreter, expr);
    yuji_value_free(result);

    result = expr_result;

    if (interpreter->call_stack->data->size > 0) {
      YujiCallFrame* frame = yuji_stack_peek(interpreter->call_stack);

      if (frame->has_return) {
        break;
      }
    }

    if (interpreter->loop_stack->data->size > 0) {
      YujiLoopFrame* lframe = yuji_stack_peek(interpreter->loop_stack);

      if (lframe->has_break || lframe->has_continue) {
        break;
      }
    }
  })
  yuji_scope_pop(interpreter);
  return result;
}

YujiValue* yuji_interpreter_eval(YujiInterpreter* interpreter, YujiASTNode* node) {
  yuji_check_memory(interpreter);
  yuji_check_memory(node);

  YUJI_LOG("evaluating node of type: %s (%p)", yuji_ast_node_type_to_string(node->type), node);

  switch (node->type) {
    case YUJI_AST_INT: {
      return yuji_value_int_init(node->value.int_->value);
    }

    case YUJI_AST_FLOAT: {
      return yuji_value_float_init(node->value.float_->value);
    }

    case YUJI_AST_STRING: {
      return yuji_value_string_init(node->value.string->value);
    }

    case YUJI_AST_BIN_OP: {
      YujiASTBinOp* binop = node->value.bin_op;
      YujiValue* left = yuji_interpreter_eval(interpreter, binop->left);
      YujiValue* right = yuji_interpreter_eval(interpreter, binop->right);

      bool is_float = (left->type == VT_FLOAT || right->type == VT_FLOAT);
      double l = (left->type == VT_FLOAT) ? left->value.float_ : (double)left->value.int_;
      double r = (right->type == VT_FLOAT) ? right->value.float_ : (double)right->value.int_;

      YujiValue* result = NULL;

      if (YUJI_STRCMP(binop->operator, "+")) {
        result = is_float ? yuji_value_float_init(l + r) : yuji_value_int_init((int64_t)(l + r));
      } else if (YUJI_STRCMP(binop->operator, "-")) {
        result = is_float ? yuji_value_float_init(l - r) : yuji_value_int_init((int64_t)(l - r));
      } else if (YUJI_STRCMP(binop->operator, "*")) {
        result = is_float ? yuji_value_float_init(l * r) : yuji_value_int_init((int64_t)(l * r));
      } else if (YUJI_STRCMP(binop->operator, "/")) {
        if (r == 0.0) {
          yuji_panic("division by zero");
        }

        result = is_float ? yuji_value_float_init(l / r) : yuji_value_int_init((int64_t)(l / r));
      } else if (!is_float && YUJI_STRCMP(binop->operator, "%")) {
        result = yuji_value_int_init((int64_t)l % (int64_t)r);
      } else if (YUJI_STRCMP(binop->operator, "<")) {
        result = yuji_value_bool_init(l < r);
      } else if (YUJI_STRCMP(binop->operator, ">")) {
        result = yuji_value_bool_init(l > r);
      } else if (YUJI_STRCMP(binop->operator, "==")) {
        result = yuji_value_bool_init(l == r);
      } else if (YUJI_STRCMP(binop->operator, "!=")) {
        result = yuji_value_bool_init(l != r);
      } else if (YUJI_STRCMP(binop->operator, "<=")) {
        result = yuji_value_bool_init(l <= r);
      } else if (YUJI_STRCMP(binop->operator, ">=")) {
        result = yuji_value_bool_init(l >= r);
      } else if (YUJI_STRCMP(binop->operator, "&&")) {
        result = yuji_value_bool_init(yuji_value_to_bool(left) && yuji_value_to_bool(right));
      } else if (YUJI_STRCMP(binop->operator, "||")) {
        result = yuji_value_bool_init(yuji_value_to_bool(left) || yuji_value_to_bool(right));
      }

      yuji_value_free(left);
      yuji_value_free(right);
      return result;
    }

    case YUJI_AST_BLOCK: {
      return yuji_interpreter_eval_block(interpreter, node->value.block);
    }

    case YUJI_AST_BOOL: {
      return yuji_value_bool_init(node->value.boolean->value);
    }

    case YUJI_AST_NULL: {
      return yuji_value_null_init();
    }

    case YUJI_AST_LET: {
      char* name = node->value.let->name;

      if (yuji_scope_get(interpreter->current_scope, name)) {
        yuji_panic("variable %s already exists", name);
      }

      YujiValue* value = yuji_interpreter_eval(interpreter, node->value.let->value);
      yuji_scope_set(interpreter->current_scope, name, value);
      yuji_value_free(value);
      return yuji_value_null_init();
    }

    case YUJI_AST_IDENTIFIER: {
      char* name = node->value.identifier->value;
      YujiValue* value = yuji_scope_get(interpreter->current_scope, name);

      if (!value) {
        yuji_panic("name %s not found", name);
      }

      return value;
    }

    case YUJI_AST_ASSIGN: {
      char* name = node->value.assign->name;
      YujiValue* existing = yuji_scope_get(interpreter->current_scope, name);

      if (!existing) {
        yuji_panic("name %s not found", name);
      }

      yuji_value_free(existing);

      YujiValue* value = yuji_interpreter_eval(interpreter, node->value.assign->value);
      yuji_scope_update(interpreter->current_scope, name, value);
      yuji_value_free(value);
      return yuji_value_null_init();
    }

    case YUJI_AST_FN: {
      char* name = node->value.fn->name;

      if (!name) {
        YujiValue* value = yuji_value_function_init(node->value.fn);
        value->refcount++;
        yuji_value_free(value);
        return value;
      }

      if (yuji_scope_get(interpreter->current_scope, name)) {
        yuji_panic("function %s already exists", name);
      }

      YujiValue* value = yuji_value_function_init(node->value.fn);
      yuji_scope_set(interpreter->current_scope, name, value);
      yuji_value_free(value);
      return yuji_value_null_init();
    }

    case YUJI_AST_CALL: {
      YujiASTCall* call = node->value.call;
      YujiValue* fn = yuji_scope_get(interpreter->current_scope, call->name);

      if (!fn) {
        yuji_panic("function %s not found", call->name);
      }

      YujiValue* result = NULL;

      if (fn->type == VT_CFUNCTION) {
        if (fn->value.cfunction->argc != YUJI_FN_INF_ARGUMENT
            && fn->value.cfunction->argc != call->args->size) {
          yuji_panic("function '%s' expects %ld, got %ld", call->name, fn->value.cfunction->argc,
                     call->args->size);
        }

        YujiDynArray* args = yuji_dyn_array_init();
        YUJI_DYN_ARRAY_ITER(call->args, YujiASTNode, arg, {
          YujiValue* arg_val = yuji_interpreter_eval(interpreter, arg);
          yuji_dyn_array_push(args, arg_val);
        })

        yuji_scope_push(interpreter);
        YujiCallFrame* frame = yuji_call_frame_init(interpreter->current_scope, call->name, args);
        yuji_stack_push(interpreter->call_stack, frame);

        result = fn->value.cfunction->func(interpreter->current_scope, args);

        yuji_call_frame_free(yuji_stack_pop(interpreter->call_stack));
        yuji_scope_pop(interpreter);
      } else if (fn->type == VT_FUNCTION) {
        YujiASTFunction* fn_node = fn->value.function.node;

        if (fn_node->params->size != call->args->size) {
          yuji_panic("function %s expects %zu args, got %zu",
                     call->name,
                     fn_node->params->size,
                     call->args->size);
        }

        yuji_scope_push(interpreter);
        YujiScope* fn_scope = interpreter->current_scope;

        for (size_t i = 0; i < fn_node->params->size; i++) {
          char* param_name = fn_node->params->data[i];
          YujiASTNode* arg_expr = call->args->data[i];
          YujiValue* arg_val = yuji_interpreter_eval(interpreter, arg_expr);
          yuji_scope_set(fn_scope, param_name, arg_val);
          yuji_value_free(arg_val);
        }

        if (interpreter->call_stack->data->size > interpreter->max_stack_size) {
          yuji_panic("stack overflow");
        }

        YujiCallFrame* frame = yuji_call_frame_init(fn_scope, call->name, NULL);
        yuji_stack_push(interpreter->call_stack, frame);

        result = yuji_interpreter_eval(interpreter, fn_node->body);

        YujiValue* ret_val;

        if (frame->has_return) {
          ret_val = frame->return_value ? frame->return_value : yuji_value_null_init();
          frame->return_value = NULL;
          yuji_value_free(result);
        } else {
          ret_val = result;
        }

        yuji_call_frame_free(yuji_stack_pop(interpreter->call_stack));
        yuji_scope_pop(interpreter);
        yuji_value_free(fn);

        return ret_val;
      } else {
        yuji_panic("'%s' is not a function (got '%s')", call->name, yuji_value_to_string(fn));
      }

      yuji_value_free(fn);
      return result;
    }

    case YUJI_AST_WHILE: {
      yuji_scope_push(interpreter);
      YujiLoopFrame* loop_frame = yuji_loop_frame_init();
      yuji_stack_push(interpreter->loop_stack, loop_frame);

      YujiValue* result = yuji_value_null_init();

      while (true) {
        YujiValue* condition_val = yuji_interpreter_eval(interpreter, node->value.while_stmt->condition);
        bool cond = yuji_value_to_bool(condition_val);
        yuji_value_free(condition_val);

        if (!cond || loop_frame->has_break) {
          break;
        }

        if (loop_frame->has_continue) {
          loop_frame->has_continue = false;
        }

        yuji_value_free(result);
        result = yuji_interpreter_eval_block(interpreter, node->value.while_stmt->body);
      }

      yuji_loop_frame_free(yuji_stack_pop(interpreter->loop_stack));
      yuji_scope_pop(interpreter);
      return result;
    }

    case YUJI_AST_IF: {
      YUJI_DYN_ARRAY_ITER(node->value.if_stmt->branches, YujiASTIfBranch, branch, {
        YujiValue* condition_val = yuji_interpreter_eval(interpreter, branch->condition);
        bool condition_result = yuji_value_to_bool(condition_val);
        yuji_value_free(condition_val);

        if (condition_result) {
          return yuji_interpreter_eval_block(interpreter, branch->body);
        }
      })

      if (node->value.if_stmt->else_body) {
        return yuji_interpreter_eval_block(interpreter, node->value.if_stmt->else_body);
      }

      return yuji_value_null_init();
    }

    case YUJI_AST_USE: {
      yuji_interpreter_load_module(interpreter, node->value.use->value);
      return yuji_value_null_init();
    }

    case YUJI_AST_RETURN: {
      if (interpreter->call_stack->data->size == 0) {
        yuji_panic("Cannot return from top-level code");
      }

      YujiValue* value = node->value.return_stmt->value
                         ? yuji_interpreter_eval(interpreter, node->value.return_stmt->value)
                         : yuji_value_null_init();
      YujiCallFrame* frame = yuji_stack_peek(interpreter->call_stack);
      frame->has_return = true;
      value->refcount++;
      frame->return_value = value;
      return value;
    }

    case YUJI_AST_BREAK: {
      if (interpreter->loop_stack->data->size == 0) {
        yuji_panic("Cannot break outside of a loop");
      }

      YujiLoopFrame* frame = yuji_stack_peek(interpreter->loop_stack);
      frame->has_break = true;
      return yuji_value_null_init();
    }

    case YUJI_AST_CONTINUE: {
      if (interpreter->loop_stack->data->size == 0) {
        yuji_panic("Cannot continue outside of a loop");
      }

      YujiLoopFrame* frame = yuji_stack_peek(interpreter->loop_stack);
      frame->has_continue = true;
      return yuji_value_null_init();
    }

    case YUJI_AST_ARRAY: {
      YujiDynArray* evaluated_elements = yuji_dyn_array_init();

      YUJI_DYN_ARRAY_ITER(node->value.array->elements, YujiASTNode, element, {
        YujiValue* evaluated = yuji_interpreter_eval(interpreter, element);
        yuji_dyn_array_push(evaluated_elements, evaluated);
      })

      return yuji_value_array_init(evaluated_elements);
    }

    case YUJI_AST_INDEX_ACCESS: {
      YujiValue* obj_val = yuji_interpreter_eval(interpreter, node->value.index_access->object);
      YujiValue* index_val = yuji_interpreter_eval(interpreter, node->value.index_access->index);

      if (obj_val->type != VT_ARRAY) {
        yuji_panic("Cannot index non-array type");
      }

      if (index_val->type != VT_INT) {
        yuji_panic("Array index must be an integer");
      }

      int64_t index = index_val->value.int_;
      yuji_value_free(index_val);

      if (index < 0 || (size_t)index >= obj_val->value.array->size) {
        yuji_value_free(obj_val);
        yuji_panic("Array index out of bounds: %lld", (long long)index);
      }

      YujiValue* element = yuji_dyn_array_get(obj_val->value.array, (size_t)index);
      element->refcount++;
      yuji_value_free(obj_val);
      return element;
    }

    case YUJI_AST_INDEX_ASSIGN: {
      YujiValue* obj_val = yuji_interpreter_eval(interpreter, node->value.index_assign->object);
      YujiValue* index_val = yuji_interpreter_eval(interpreter, node->value.index_assign->index);
      YujiValue* new_value = yuji_interpreter_eval(interpreter, node->value.index_assign->value);

      if (obj_val->type != VT_ARRAY) {
        yuji_panic("Cannot index non-array type");
      }

      if (index_val->type != VT_INT) {
        yuji_panic("Array index must be an integer");
      }

      int64_t index = index_val->value.int_;
      yuji_value_free(index_val);

      if (index < 0 || (size_t)index >= obj_val->value.array->size) {
        yuji_value_free(obj_val);
        yuji_value_free(new_value);
        yuji_panic("Array index out of bounds: %lld", (long long)index);
      }

      YujiValue* old_element = yuji_dyn_array_get(obj_val->value.array, (size_t)index);
      yuji_value_free(old_element);

      new_value->refcount++;
      yuji_dyn_array_set(obj_val->value.array, (size_t)index, new_value);

      yuji_value_free(obj_val);
      yuji_value_free(new_value);

      return yuji_value_null_init();
    }
  }

  yuji_panic("Unknown node type: %d", node->type);
}
