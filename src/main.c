#include "yuji/ast.h"
#include "yuji/dyn_array.h"
#include "yuji/interpreter.h"
#include "yuji/lexer.h"
#include "yuji/parser.h"
#include "yuji/utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int eval(ASTNode* node, Interpreter* interpreter) {
  if (node->type == AST_NUMBER) {
    return atoi(node->number.value);
  }

  if (node->type == AST_BIN_OP) {
    int left = eval(node->bin_op.left, interpreter);
    int right = eval(node->bin_op.right, interpreter);

    if (strcmp(node->bin_op.op, "+") == 0) {
      return left + right;
    }

    if (strcmp(node->bin_op.op, "-") == 0) {
      return left - right;
    }

    if (strcmp(node->bin_op.op, "*") == 0) {
      return left * right;
    }

    if (strcmp(node->bin_op.op, "/") == 0) {
      return left / right;
    }
  }

  if (node->type == AST_LET) {
    int result = eval(node->let.value, interpreter);
    map_insert(interpreter->variables, node->let.name->value,
               (void*)(intptr_t)result);
    return result;
  }

  if (node->type == AST_IDENTIFIER) {
    void* val = map_get(interpreter->variables, node->identifier.value);

    if (!val) {
      panic("Undefined variable: %s", node->identifier.value);
    }

    return (int)(intptr_t)val;
  }

  if (node->type == AST_ASSIGN) {
    int result = eval(node->assign.value, interpreter);
    void* existing_val = map_get(interpreter->variables, node->assign.name->value);

    if (!existing_val) {
      panic("Cannot assign to undefined variable: %s", node->assign.name->value);
    }

    map_insert(interpreter->variables, node->assign.name->value,
               (void*)(intptr_t)result);
    return result;
  }

  if (node->type == AST_IF) {
    int condition = eval(node->if_.condition, interpreter);

    if (condition) {
      return eval(node->if_.body, interpreter);
    }
  }

  if (node->type == AST_BLOCK) {
    int result = 0;

    for (size_t i = 0; i < node->block.expressions->size; ++i) {
      result = eval(dyn_array_get(node->block.expressions, i), interpreter);
    }

    return result;
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
      int result = eval(node, interpreter);
      printf("%d\n", result);
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

  char* input = NULL;
  size_t len = 0;

  Interpreter* interpreter = interpreter_init();

  while (getline(&input, &len, fp) != -1) {
    Lexer* lexer = lexer_init(input);
    DynArr* tokens = lexer_tokenize(lexer);

    Parser* parser = parser_init(tokens);
    DynArr* ast = parser_parse(parser);

    for (size_t i = 0; i < ast->size; i++) {
      ASTNode* node = dyn_array_get(ast, i);
      int result = eval(node, interpreter);
      printf("%d\n", result);
    }

    lexer_free(lexer);
    parser_free(parser);
  }

  interpreter_free(interpreter);
  fclose(fp);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    repl();
  } else {
    run_file(argv[1]);
  }
}
