#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "yuji/dyn_array.h"
#include "yuji/lexer.h"
#include "yuji/token.h"
#include "yuji/utils.h"

Lexer* lexer_init(const char* input) {
  Lexer *lexer = malloc(sizeof(Lexer));
  check_memory_is_not_null(lexer);
  lexer->pos = 0;
  lexer->input = input;
  lexer->current_char =
    lexer->input[lexer->pos] ? lexer->input[lexer->pos] : '\0';
  lexer->tokens = dyn_array_init();
  return lexer;
}

void lexer_free(Lexer *lexer) {
  if (!lexer) {
    return;
  }

  for (size_t i = 0; i < lexer->tokens->size; i++) {
    Token *token = (Token*)dyn_array_get(lexer->tokens, i);
    token_free(token);
  }

  dyn_array_free(lexer->tokens);
  free(lexer);
}

char lexer_advance(Lexer *lexer) {
  lexer->pos++;
  lexer->current_char =
    lexer->pos < strlen(lexer->input) ? lexer->input[lexer->pos] : '\0';
  return lexer->current_char;
}

void lexer_skip_whitespace(Lexer *lexer) {
  char c = lexer->current_char;

  while (c != '\0' && isspace(c)) {
    c = lexer_advance(lexer);
  }
}

DynArr* lexer_tokenize(Lexer *lexer) {
  while (lexer->current_char != '\0') {
    char c = lexer->current_char;

    if (isspace(c)) {
      lexer_skip_whitespace(lexer);
      continue;
    }

    LOG("Current character: %c", c);
    char* value = malloc(YUJI_MIN_VALUE_LENGTH);
    TokenType type;
    check_memory_is_not_null(value);

    if (isdigit(c)) {
      lexer_parse_number(lexer, value);
      type = TT_NUMBER;
    } else {
      switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
          value[0] = c;
          value[1] = '\0';
          type = TT_OPERATOR;
          break;

        default: {
          lexer_error(lexer, "Unexpected character");
        }
      }

      c = lexer_advance(lexer);
    }

    Token *token = token_init(value, type);
    dyn_array_append(lexer->tokens, token);
  }

  return lexer->tokens;
}

char* lexer_parse_number(Lexer *lexer, char* value) {
  size_t index = 0;
  char c = lexer->current_char;

  while (c != '\0' && isdigit(c)) {
    value[index++] = c;
    c = lexer_advance(lexer);
  }

  return value;
}

void lexer_error(const Lexer* lexer, char* message) {
  const char* input = lexer->input;
  size_t pos = lexer->pos;
  fprintf(stderr, ANSI_RED "\nLexer error: %s\n" ANSI_RESET, message);
  fprintf(stderr, "%s\n", input);

  for (size_t i = 0; i < pos; i++) {
    fputc(input[i] == '\t' ? '\t' : '~', stderr);
  }

  fprintf(stderr, ANSI_RED "^\n" ANSI_RESET);
  exit(1);
}
