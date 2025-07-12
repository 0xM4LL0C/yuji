#include "yuji/dyn_array.h"
#include "yuji/lexer.h"
#include "yuji/parser.h"
#include "yuji/utils.h"
#include <stdio.h>
#include <string.h>

int eval(ASTNode* node) {
  if (node->type == AST_NUMBER)
    return atoi(node->number.value);

  int left = eval(node->bin_op.left);
  int right = eval(node->bin_op.right);

  if (strcmp(node->bin_op.op, "+") == 0)
    return left + right;

  if (strcmp(node->bin_op.op, "-") == 0)
    return left - right;

  if (strcmp(node->bin_op.op, "*") == 0)
    return left * right;

  if (strcmp(node->bin_op.op, "/") == 0)
    return left / right;

  panic("Unknown operator");
}

void repl() {
  char* input = NULL;
  size_t len = 0;

  while (1) {
    printf(">> ");
    getline(&input, &len, stdin);

    Lexer* lexer = lexer_init(input);
    DynArr* tokens = lexer_tokenize(lexer);

    Parser* parser = parser_init(tokens);
    DynArr* ast = parser_parse(parser);

    int result = eval(ast->data[0]);

    printf("%d\n", result);

    lexer_free(lexer);
    parser_free(parser);
  }
}

int main() {
  repl();
}
