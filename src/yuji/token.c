#include "yuji/token.h"
#include "yuji/utils.h"
#include <stdlib.h>
#include <string.h>

Token* token_init(char* value, TokenType type) {
  Token *token = malloc(sizeof(Token));
  check_memory_is_not_null(token);
  token->value = strdup(value);
  token->type = type;
  return token;
}

void token_free(Token *token) {
  if (token) {
    free(token->value);
    free(token);
  }
}

char* tt_to_string(TokenType type) {
  switch (type) {

    case TT_NUMBER:
      return "TT_NUMBER";

    case TT_IDENTIFIER:
      return "TT_IDENTIFIER";

    case TT_LET:
      return "TT_LET";

    case TT_ASSIGN:
      return "TT_ASSIGN";

    case TT_PLUS:
      return "TT_PLUS";

    case TT_MINUS:
      return "TT_MINUS";

    case TT_MUL:
      return "TT_MUL";

    case TT_DIV:
      return "TT_DIV";

    case TT_LPAREN:
      return "TT_LPAREN";

    case TT_RPAREN:
      return "TT_RPAREN";
  }
}
