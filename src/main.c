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

int main() {
  repl();
}
