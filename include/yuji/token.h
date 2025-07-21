#pragma once

#include "yuji/position.h"
#ifndef YUJI_MIN_VALUE_LENGTH
#define YUJI_MIN_VALUE_LENGTH 500
#endif

typedef enum {
  TT_NUMBER,
  TT_STRING,
  TT_IDENTIFIER,

  TT_LET,
  TT_IF,
  TT_ELIF,
  TT_ELSE,
  TT_FN,
  TT_USE,

  TT_ASSIGN, // =
  TT_PLUS, // +
  TT_MINUS, // -
  TT_MUL, // *
  TT_DIV, // /
  TT_LPAREN, // (
  TT_RPAREN, // )
  TT_LBRACE, // {
  TT_RBRACE, // }
  TT_MOD, // %
  TT_LT, // <
  TT_GT, // >
  TT_COMMA, // ,
  TT_QUOTE, // "
} TokenType;

typedef struct {
  char* value;
  TokenType type;
  Position* position;
} Token;

Token* token_init(char* value, TokenType type, const Position* position);
void token_free(Token* token);

char* tt_to_string(TokenType type);
