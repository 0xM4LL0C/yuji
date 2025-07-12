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
