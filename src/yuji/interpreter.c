#include "yuji/interpreter.h"
#include "yuji/ast.h"
#include "yuji/types/dyn_array.h"
#include "yuji/types/map.h"
#include "yuji/module.h"
#include "yuji/stdlib/io.h"
#include "yuji/utils.h"
#include "yuji/value.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Interpreter* interpreter_init() {
  Interpreter* interpreter = malloc(sizeof(Interpreter));

  check_memory_is_not_null(interpreter);

  interpreter->env = map_init();
  interpreter->loaded_modules = map_init();

  interpreter_init_stdlib(interpreter);
  return interpreter;
}

void interpreter_free(Interpreter* interpreter) {
  DYN_ARR_ITER(interpreter->env->pairs, MapPair, pair, {
    value_free(pair->value);
  })

  map_free(interpreter->env);

  DYN_ARR_ITER(interpreter->loaded_modules->pairs, MapPair, pair, {
    module_free(pair->value);
  })

  map_free(interpreter->loaded_modules);
  free(interpreter);
}

void interpreter_init_stdlib(Interpreter* interpreter) {
  YujiModule* yuji_module = module_init("yuji");
  YujiModule* io_module = yuji_module_io_init();
  module_add_submodule(yuji_module, io_module);
  map_insert(interpreter->loaded_modules, yuji_module->name, yuji_module);
}

void interpreter_load_module(Interpreter* interpreter,
                             const char* module_path) {
  char* path = strdup(module_path);
  char* module_name = strtok(path, "/");
  char* submodule_name = strtok(NULL, "/");

  if (module_name && submodule_name) {
    YujiModule* parent_module = map_get(interpreter->loaded_modules, module_name);

    if (!parent_module) {
      free(path);
      panic("Parent module not found: %s", module_name);
    }

    DYN_ARR_ITER(parent_module->submodules, YujiModule, submodule, {
      if (strcmp(submodule->name, submodule_name) == 0) {
        DYN_ARR_ITER(submodule->env->pairs, MapPair, pair, {
          YujiValue* func_value = value_cfunction_init((YujiCFunction)pair->value);
          map_insert(interpreter->env, pair->key, func_value);
        })
        free(path);
        return;
      }
    })

    free(path);
    panic("Submodule not found: %s", submodule_name);
  } else {
    YujiModule* module = map_get(interpreter->loaded_modules, module_path);

    if (!module) {
      free(path);
      panic("Module not found: %s", module_path);
    }

    DYN_ARR_ITER(module->env->pairs, MapPair, pair, {
      YujiValue* func_value = value_cfunction_init((YujiCFunction)pair->value);
      map_insert(interpreter->env, pair->key, func_value);
    })

    free(path);
  }
}

YujiValue* interpreter_eval(Interpreter* interpreter, ASTNode* node) {
  if (!node) {
    return value_null_init();
  }

  switch (node->type) {
    case AST_NUMBER: {
      return value_number_init(atoi(node->number.value));
    }

    case AST_BIN_OP: {
      YujiValue* left = interpreter_eval(interpreter, node->bin_op.left);
      YujiValue* right = interpreter_eval(interpreter, node->bin_op.right);

      YujiValue* result;

      if (strcmp(node->bin_op.op, "+") == 0) {
        result = value_number_init(left->value.number + right->value.number);
      } else if (strcmp(node->bin_op.op, "-") == 0) {
        result = value_number_init(left->value.number - right->value.number);
      } else if (strcmp(node->bin_op.op, "*") == 0) {
        result = value_number_init(left->value.number * right->value.number);
      } else if (strcmp(node->bin_op.op, "/") == 0) {
        result = value_number_init(left->value.number / right->value.number);
      } else if (strcmp(node->bin_op.op, "%") == 0) {
        result = value_number_init(left->value.number % right->value.number);
      } else if (strcmp(node->bin_op.op, "<") == 0) {
        result = value_bool_init(left->value.number < right->value.number);
      } else if (strcmp(node->bin_op.op, ">") == 0) {
        result = value_bool_init(left->value.number > right->value.number);
      } else {
        panic("invalid operator: %s", node->bin_op.op);
      }

      value_free(left);
      value_free(right);
      return result;
    }

    case AST_STRING: {
      return value_string_init(node->string.value);
    }

    case AST_LET: {
      if (map_get(interpreter->env, node->let.name->value)) {
        panic("variable already declared: %s", node->let.name->value);
      }

      YujiValue* value = interpreter_eval(interpreter, node->let.value);
      map_insert(interpreter->env, node->let.name->value, value);
      return value;
    }

    case AST_ASSIGN: {
      if (!map_get(interpreter->env, node->assign.name->value)) {
        panic("undefined name: %s", node->assign.name->value);
      }

      YujiValue* value = interpreter_eval(interpreter, node->assign.value);
      map_insert(interpreter->env, node->assign.name->value, value);
      return value;
    }

    case AST_BLOCK: {
      DYN_ARR_ITER(node->block.expressions, ASTNode, expr, {
        YujiValue* result = interpreter_eval(interpreter, expr);
        value_free(result);
      })

      return value_null_init();
    }

    case AST_IDENTIFIER: {
      YujiValue* value = map_get(interpreter->env, node->identifier.value);

      if (value) {
        return value;
      }

      panic("undefined name: %s", node->identifier.value);
    }

    case AST_IF: {
      YujiValue* condition = interpreter_eval(interpreter, node->if_.condition);

      if (value_to_bool(condition)) {
        value_free(condition);
        return interpreter_eval(interpreter, node->if_.body);
      }

      value_free(condition);
      return value_null_init();
    }

    case AST_ELIF: {
      YujiValue* condition = interpreter_eval(interpreter, node->elif.condition);

      if (value_to_bool(condition)) {
        value_free(condition);
        return interpreter_eval(interpreter, node->elif.body);
      }

      value_free(condition);

      return value_null_init();
    }

    case AST_ELSE: {
      return interpreter_eval(interpreter, node->else_.body);
    }

    case AST_USE: {
      interpreter_load_module(interpreter, node->use.value);
      return value_null_init();
    }

    case AST_FUNCTION: {
      if (map_get(interpreter->env, node->function.name->value)) {
        panic("name already defined: %s", node->function.name->value);
      }

      map_insert(interpreter->env, node->function.name->value,
                 value_function_init(node));
      return value_null_init();
    }

    case AST_CALL: {
      YujiValue* function = map_get(interpreter->env, node->call.name->value);

      if (!function) {
        panic("undefined function: %s", node->call.name->value);
      }

      if (function->type == VT_CFUNCTION) {
        DynArr* evaluated_args = dyn_array_init();

        DYN_ARR_ITER(node->call.args, ASTNode, arg, {
          YujiValue* arg_value = interpreter_eval(interpreter, arg);
          dyn_array_append(evaluated_args, arg_value);
        })

        YujiValue* result = function->value.cfunction(evaluated_args);

        dyn_array_free(evaluated_args);

        return result ? result : value_null_init();
      } else if (function->type == VT_FUNCTION) {
        ASTNode* func_node = function->value.function.node;

        if (func_node->function.params->size != node->call.args->size) {
          panic("function '%s' expects %zu arguments, got %zu",
                node->call.name->value,
                func_node->function.params->size,
                node->call.args->size);
        }

        Interpreter* local_interpreter = interpreter_init();
        local_interpreter->env = interpreter->env;

        DYN_ARR_ITER(node->call.args, ASTNode, arg, {
          ASTIdentifier* param = dyn_array_get(func_node->function.params, i);
          YujiValue* arg_value = interpreter_eval(local_interpreter, arg);
          map_insert(local_interpreter->env, param->value, arg_value);
        })

        YujiValue* result = interpreter_eval(local_interpreter,
                                             func_node->function.body);

        local_interpreter->env = NULL;
        interpreter_free(local_interpreter);

        return result ? result : value_null_init();
      } else {
        panic("'%s' is not a function", node->call.name->value);
      }
    }

    case AST_BOOL: {
      return value_bool_init(node->bool_.value);
    }

    case AST_NULL: {
      return value_null_init();
    }

  }

  panic("Invalid node type");
}
