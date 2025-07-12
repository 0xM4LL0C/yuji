#pragma once

#include <stddef.h>

#include "dyn_array.h"

typedef struct {
  DynArr* tokens;
  const char* input;
  char current_char;
  size_t pos;
} Lexer;

Lexer* lexer_init(const char* input);
void lexer_free(Lexer* lexer);

char lexer_advance(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);

DynArr* lexer_tokenize(Lexer* lexer);

char* lexer_parse_number(Lexer* lexer, char* value);
void lexer_error(const Lexer* lexer, char* message) __attribute__((noreturn));
