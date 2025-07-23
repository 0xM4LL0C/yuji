#include "yuji/interpreter.h"
#include "yuji/ast.h"
#include "yuji/types/dyn_array.h"
#include "yuji/types/map.h"
#include "yuji/module.h"
#include "yuji/stdlib/io.h"
#include "yuji/utils.h"
#include "yuji/value.h"

#include <stdint.h>
#include <stdio.h>
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
  for (size_t i = 0; i < interpreter->env->pairs->size; i++) {
    MapPair* pair = dyn_array_get(interpreter->env->pairs, i);
    value_free(pair->value);
  }

  map_free(interpreter->env);

  for (size_t i = 0; i < interpreter->loaded_modules->pairs->size; i++) {
    MapPair* pair = dyn_array_get(interpreter->loaded_modules->pairs, i);
    module_free(pair->value);
  }

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

    for (size_t i = 0; i < parent_module->submodules->size; i++) {
      YujiModule* submodule = dyn_array_get(parent_module->submodules, i);

      if (strcmp(submodule->name, submodule_name) == 0) {
        for (size_t j = 0; j < submodule->env->pairs->size; j++) {
          MapPair* pair = dyn_array_get(submodule->env->pairs, j);
          YujiValue* func_value = value_cfunction_init((YujiCFunction)pair->value);
          map_insert(interpreter->env, pair->key, func_value);
        }

        free(path);
        return;
      }
    }

    free(path);
    panic("Submodule not found: %s", submodule_name);
  } else {
    YujiModule* module = map_get(interpreter->loaded_modules, module_path);

    if (!module) {
      free(path);
      panic("Module not found: %s", module_path);
    }

    for (size_t i = 0; i < module->env->pairs->size; i++) {
      MapPair* pair = dyn_array_get(module->env->pairs, i);
      YujiValue* func_value = value_cfunction_init((YujiCFunction)pair->value);
      map_insert(interpreter->env, pair->key, func_value);
    }

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
      for (size_t i = 0; i < node->block.expressions->size; i++) {
        YujiValue* result = interpreter_eval(interpreter,
                                             (ASTNode*)dyn_array_get(node->block.expressions,
                                               i));
        value_free(result);
      }

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

      if (condition) {
        value_free(condition);
        return interpreter_eval(interpreter, node->if_.body);
      }

      value_free(condition);
      return value_null_init();
    }

    case AST_ELIF: {
      YujiValue* condition = interpreter_eval(interpreter, node->elif.condition);

      if (condition) {
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

        for (size_t i = 0; i < node->call.args->size; i++) {
          YujiValue* arg_value = interpreter_eval(interpreter,
                                                  dyn_array_get(node->call.args, i));
          dyn_array_append(evaluated_args, arg_value);
        }

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

        for (size_t i = 0; i < node->call.args->size; i++) {
          ASTIdentifier* param = dyn_array_get(func_node->function.params, i);
          YujiValue* arg_value = interpreter_eval(local_interpreter,
                                                  dyn_array_get(node->call.args, i));
          map_insert(local_interpreter->env, param->value, arg_value);
        }

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
