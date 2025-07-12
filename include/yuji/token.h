#pragma once

#ifndef YUJI_MIN_VALUE_LENGTH
#define YUJI_MIN_VALUE_LENGTH 500
#endif

typedef enum {
  TT_NUMBER,
  TT_OPERATOR,
} TokenType;

typedef struct {
  char* value;
  TokenType type;
} Token;

Token* token_init(char* value, TokenType type);
void token_free(Token* token);
