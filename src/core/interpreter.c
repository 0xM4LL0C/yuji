#include "yuji/core/interpreter.h"
#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/map.h"
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

void yuji_scope_merge(YujiScope* dest, YujiScope* src) {
  YUJI_DYN_ARRAY_ITER(src->env->pairs, YujiMapPair, pair, {
    yuji_scope_set(dest, pair->key, pair->value);
  })
}

YujiInterpreter* yuji_interpreter_init() {
  YujiInterpreter* interpreter = yuji_malloc(sizeof(YujiInterpreter));

  interpreter->current_scope = yuji_scope_init(NULL);
  interpreter->loaded_modules = yuji_map_init();

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

YujiValue* yuji_interpreter_eval(YujiInterpreter* interpreter, YujiASTNode* node) {
  yuji_check_memory(interpreter);
  yuji_check_memory(node);

  switch (node->type) {
    case YUJI_AST_NUMBER: {
      return yuji_value_number_init(node->value.number->value);
    }

    case YUJI_AST_STRING: {
      return yuji_value_string_init(node->value.string->value);
    }

    case YUJI_AST_BIN_OP: {
      YujiASTBinOp* binop = node->value.bin_op;
      YujiValue* left = yuji_interpreter_eval(interpreter, binop->left);
      YujiValue* right = yuji_interpreter_eval(interpreter, binop->right);

      YujiValue* result = NULL;

      if (YUJI_STRCMP(binop->operator, "+")) {
        result = yuji_value_number_init(left->value.number + right->value.number);
      } else if (YUJI_STRCMP(binop->operator, "-")) {
        result = yuji_value_number_init(left->value.number - right->value.number);
      } else if (YUJI_STRCMP(binop->operator, "*")) {
        result = yuji_value_number_init(left->value.number * right->value.number);
      } else if (YUJI_STRCMP(binop->operator, "/")) {
        result = yuji_value_number_init(left->value.number / right->value.number);
      } else if (YUJI_STRCMP(binop->operator, "%")) {
        result = yuji_value_number_init(left->value.number % right->value.number);
      } else if (YUJI_STRCMP(binop->operator, "<")) {
        result = yuji_value_bool_init(left->value.number < right->value.number);
      } else if (YUJI_STRCMP(binop->operator, ">")) {
        result = yuji_value_bool_init(left->value.number > right->value.number);
      } else {
        yuji_panic("Unknown operator: %s", binop->operator);
      }

      yuji_value_free(left);
      yuji_value_free(right);
      return result;
    }

    case YUJI_AST_BLOCK: {
      yuji_scope_push(interpreter);
      YujiValue* result = yuji_value_null_init();

      YUJI_DYN_ARRAY_ITER(node->value.block->exprs, YujiASTNode, expr, {
        YujiValue* expr_result = yuji_interpreter_eval(interpreter, expr);
        yuji_value_free(result);

        result = expr_result;
      })

      yuji_scope_pop(interpreter);
      return result;
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
      yuji_scope_set(interpreter->current_scope, name, value);
      yuji_value_free(value);
      return yuji_value_null_init();
    }

    case YUJI_AST_FN: {
      char* name = node->value.fn->name;

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
          yuji_panic("function '%s' expects %d, got %d", call->name, fn->value.cfunction->argc,
                     call->args->size);
        }

        YujiDynArray* args = yuji_dyn_array_init();
        YUJI_DYN_ARRAY_ITER(call->args, YujiASTNode, arg, {
          YujiValue* arg_val = yuji_interpreter_eval(interpreter, arg);
          yuji_dyn_array_push(args, arg_val);
        })

        yuji_scope_push(interpreter);
        result = fn->value.cfunction->func(interpreter->current_scope, args);
        yuji_scope_pop(interpreter);

        YUJI_DYN_ARRAY_ITER(args, YujiValue, arg, {
          yuji_value_free(arg);
        })
        yuji_dyn_array_free(args);
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

        YujiValue* fn_result = yuji_interpreter_eval(interpreter, fn_node->body);
        yuji_scope_pop(interpreter);
        yuji_value_free(result);
        result = fn_result;
      } else {
        yuji_panic("'%s' is not a function", call->name);
      }

      yuji_value_free(fn);
      return result;
    }

    case YUJI_AST_WHILE: {
      yuji_scope_push(interpreter);
      YujiValue* result = yuji_value_null_init();

      while (true) {
        YujiValue* condition_val = yuji_interpreter_eval(interpreter, node->value.while_stmt->condition);
        bool should_continue = yuji_value_to_bool(condition_val);
        yuji_value_free(condition_val);

        if (!should_continue) {
          break;
        }

        YUJI_DYN_ARRAY_ITER(node->value.while_stmt->body->exprs, YujiASTNode, body, {
          YujiValue* body_result = yuji_interpreter_eval(interpreter, body);

          yuji_value_free(result);

          result = body_result;
        })
      }

      yuji_scope_pop(interpreter);
      return result;
    }

    case YUJI_AST_IF: {
      YUJI_DYN_ARRAY_ITER(node->value.if_stmt->branches, YujiASTIfBranch, branch, {
        YujiValue* condition_val = yuji_interpreter_eval(interpreter, branch->condition);
        bool condition_result = yuji_value_to_bool(condition_val);

        yuji_value_free(condition_val);

        if (condition_result) {
          YujiASTNode* block = yuji_ast_block_init(branch->body->exprs);
          YujiValue* result = yuji_interpreter_eval(interpreter, block);
          yuji_ast_free(block);
          return result;
        }
      })

      if (node->value.if_stmt->else_body) {
        YujiValue* result = yuji_value_null_init();
        YUJI_DYN_ARRAY_ITER(node->value.if_stmt->else_body->exprs, YujiASTNode, expr, {
          YujiValue* expr_result = yuji_interpreter_eval(interpreter, expr);

          yuji_value_free(result);

          result = expr_result;
        })
        return result;
      }

      return yuji_value_null_init();
    }

    case YUJI_AST_USE: {
      yuji_interpreter_load_module(interpreter, node->value.use->value);
      return yuji_value_null_init();
    }
  }

  yuji_panic("Unknown node type");
}
