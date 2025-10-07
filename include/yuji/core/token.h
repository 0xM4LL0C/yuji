#pragma once

#include "yuji/core/position.h"

typedef enum {
  // basic
  TT_NUMBER,
  TT_STRING,
  TT_IDENTIFIER,

  // keywords
  TT_LET,
  TT_IF,
  TT_ELIF,
  TT_ELSE,
  TT_FN,
  TT_USE,
  TT_BOOL,
  TT_NULL,
  TT_WHILE,
  TT_RETURN,

  // operators
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
  TT_EQ, // ==
  TT_NEQ, // !=
  TT_LTE, // <=
  TT_GTE, // >=
  TT_AND, // &&
  TT_OR, // ||
} YujiTokenType;

const char* yuji_token_type_to_string(YujiTokenType type);

typedef struct {
  const char* value;
  YujiTokenType type;
  YujiPosition position;
} YujiToken;

YujiToken* yuji_token_init(const char* value, const YujiTokenType type,
                           const YujiPosition position);
void yuji_token_free(YujiToken* token);

const char* yuji_token_to_string(const YujiToken* token);
