#include "yuji/module.h"
#include "yuji/value.h"
#ifdef WIN32
#warning "Windows is not officially supported"
#endif
#ifndef __unix__
#warning "Non Unix OS is not officially supported"
#endif

#include "yuji/ast.h"
#include "yuji/types/dyn_array.h"
#include "yuji/interpreter.h"
#include "yuji/lexer.h"
#include "yuji/parser.h"
#include "yuji/utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

YujiValue* eval(ASTNode* node, Interpreter* interpreter) {
  if (node->type == AST_NUMBER) {
    return value_number_init(atoi(node->number.value));
  }

  if (node->type == AST_BIN_OP) {
    YujiValue* left = eval(node->bin_op.left, interpreter);
    YujiValue* right = eval(node->bin_op.right, interpreter);

    if (strcmp(node->bin_op.op, "+") == 0) {
      return value_number_init(left->value.number + right->value.number);
    }

    if (strcmp(node->bin_op.op, "-") == 0) {
      return value_number_init(left->value.number - right->value.number);
    }

    if (strcmp(node->bin_op.op, "*") == 0) {
      return value_number_init(left->value.number * right->value.number);
    }

    if (strcmp(node->bin_op.op, "/") == 0) {
      return value_number_init(left->value.number / right->value.number);
    }

    if (strcmp(node->bin_op.op, "%") == 0) {
      return value_number_init(left->value.number % right->value.number);
    }

    if (strcmp(node->bin_op.op, "<") == 0) {
      return value_number_init(left->value.number < right->value.number);
    }

    if (strcmp(node->bin_op.op, ">") == 0) {
      return value_number_init(left->value.number > right->value.number);
    }
  }

  if (node->type == AST_LET) {
    YujiValue* result = eval(node->let.value, interpreter);
    map_insert(interpreter->env, node->let.name->value, result);
    return result;
  }

  if (node->type == AST_IDENTIFIER) {
    YujiValue* val = map_get(interpreter->env, node->identifier.value);

    if (!val) {
      panic("Undefined variable: %s", node->identifier.value);
    }

    return val;
  }

  if (node->type == AST_ASSIGN) {
    YujiValue* result = eval(node->assign.value, interpreter);
    YujiValue* existing_val = map_get(interpreter->env,
                                      node->assign.name->value);

    if (!existing_val) {
      panic("Cannot assign to undefined variable: %s", node->assign.name->value);
    }

    map_insert(interpreter->env, node->assign.name->value, result);
    return result;
  }

  if (node->type == AST_IF) {
    YujiValue* condition = eval(node->if_.condition, interpreter);

    if (condition) {
      return eval(node->if_.body, interpreter);
    }

    return value_null_init();
  }

  if (node->type == AST_ELIF) {
    YujiValue* condition = eval(node->elif.condition, interpreter);

    if (condition) {
      return eval(node->elif.body, interpreter);
    }

    return value_null_init();
  }

  if (node->type == AST_ELSE) {
    return eval(node->else_.body, interpreter);
  }

  if (node->type == AST_BLOCK) {
    YujiValue* result = value_null_init();

    for (size_t i = 0; i < node->block.expressions->size; ++i) {
      result = eval(dyn_array_get(node->block.expressions, i), interpreter);
    }

    return result;
  }

  if (node->type == AST_FUNCTION) {
    YujiValue* func_value = value_function_init(node);
    map_insert(interpreter->env, node->function.name->value, func_value);
    return func_value;
  }

  if (node->type == AST_CALL) {
    YujiValue* func_value = map_get(interpreter->env, node->call.name->value);

    if (!func_value) {
      panic("Undefined function: %s", node->call.name->value);
    }

    YujiModuleFunction module_func = map_get(interpreter->env,
                                     node->call.name->value);

    if (module_func && func_value->type != VT_FUNCTION) {
      DynArr* args = dyn_array_init();

      for (size_t i = 0; i < node->call.args->size; i++) {
        YujiValue* arg_value = eval(dyn_array_get(node->call.args, i), interpreter);
        dyn_array_append(args, arg_value);
      }

      YujiValue* result = module_func(args);
      dyn_array_free(args);
      return result;
    }

    if (func_value->type != VT_FUNCTION) {
      panic("Invalid function: %s", node->call.name->value);
    }

    ASTNode* func_node = func_value->value.function.node;

    if (func_node->function.params->size != node->call.args->size) {
      panic("Incorrect number of arguments for function %s", node->call.name->value);
    }

    Interpreter* local_interpreter = interpreter_init();
    local_interpreter->env = interpreter->env;

    for (size_t i = 0; i < func_node->function.params->size; ++i) {
      ASTIdentifier* param = dyn_array_get(func_node->function.params, i);
      YujiValue* arg_value = eval(dyn_array_get(node->call.args, i), interpreter);
      map_insert(local_interpreter->env, param->value, arg_value);
    }

    YujiValue* result = eval(func_node->function.body, local_interpreter);
    local_interpreter->env = NULL;
    interpreter_free(local_interpreter);
    return result;
  }

  if (node->type == AST_STRING) {
    return value_string_init(node->string.value);
  }

  if (node->type == AST_USE) {
    YujiModule* module = map_get(interpreter->loaded_modules, node->use.value);

    if (!module) {
      panic("Module not found: %s", node->use.value);
    }

    LOG("Loading module: %s, env size: %zu", node->use.value,
        module->env->pairs->size);

    for (size_t i = 0; i < module->env->pairs->size; i++) {
      MapPair* pair = dyn_array_get(module->env->pairs, i);

      if (pair->value) {
        map_insert(interpreter->env, pair->key,
                   pair->value);
        LOG("Registered function: %s from module %s", pair->key, node->use.value);
      }
    }

    return value_null_init();
  }

  LOG("unhandled token type: %d", node->type);
  panic("Unknown operator");
}

void repl() {
  char* input = NULL;
  size_t len = 0;
  Interpreter* interpreter = interpreter_init();

  while (1) {
    printf(">> ");

    if (getline(&input, &len, stdin) == -1)
      break;

    Lexer* lexer = lexer_init(input);
    DynArr* tokens = lexer_tokenize(lexer);

    Parser* parser = parser_init(tokens);
    DynArr* ast = parser_parse(parser);

    for (size_t i = 0; i < ast->size; i++) {
      ASTNode* node = dyn_array_get(ast, i);
      eval(node, interpreter);
    }

    lexer_free(lexer);
    parser_free(parser);
  }

  interpreter_free(interpreter);
}

void run_file(const char* file) {
  FILE* fp = fopen(file, "r");

  if (!fp) {
    panic("Failed to open file: %s", file);
  }

  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char* input = malloc(size + 1);

  if (!input) {
    panic("Memory allocation failed");
  }

  size_t read_size = fread(input, 1, size, fp);
  input[read_size] = '\0';

  fclose(fp);

  Interpreter* interpreter = interpreter_init();

  Lexer* lexer = lexer_init(input);
  DynArr* tokens = lexer_tokenize(lexer);

  Parser* parser = parser_init(tokens);
  DynArr* ast = parser_parse(parser);

  for (size_t i = 0; i < ast->size; i++) {
    ASTNode* node = dyn_array_get(ast, i);
    eval(node, interpreter);
  }

  lexer_free(lexer);
  parser_free(parser);
  free(input);
  interpreter_free(interpreter);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    repl();
  } else {
    run_file(argv[1]);
  }
}
