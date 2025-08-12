#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "yuji/position.h"
#include "yuji/types/dyn_array.h"
#include "yuji/lexer.h"
#include "yuji/token.h"
#include "yuji/utils.h"
#include "yuji/keywords.h"

Lexer* lexer_init(DynArr* input, const char* file_name) {
  Lexer *lexer = malloc(sizeof(Lexer));
  check_memory_is_not_null(lexer);

  lexer->position = position_init(0, 0, file_name);
  lexer->input = input;
  lexer->tokens = dyn_array_init();
  lexer->current_char = lexer_peek(lexer);

  return lexer;
}

void lexer_free(Lexer *lexer) {
  if (!lexer) {
    return;
  }

  DYN_ARR_ITER(lexer->tokens, Token, token, {
    token_free(token);
  })

  dyn_array_free(lexer->tokens);
  position_free(lexer->position);
  free(lexer);
}

char lexer_peek(Lexer *lexer) {
  char* line = dyn_array_get(lexer->input, lexer->position->line);

  if (!line) {
    return '\0';
  }

  if (lexer->position->column < strlen(line)) {
    return line[lexer->position->column];
  }

  return '\0';
}

char lexer_advance(Lexer *lexer) {
  char* line = dyn_array_get(lexer->input, lexer->position->line);

  if (!line) {
    lexer->current_char = '\0';
    return '\0';
  }

  if (lexer->position->column < strlen(line)) {
    lexer->position->column++;
  } else {
    lexer->position->line++;
    lexer->position->column = 0;
  }

  lexer->current_char = lexer_peek(lexer);
  return lexer->current_char;
}

void lexer_skip_whitespace(Lexer *lexer) {
  while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
    if (lexer->current_char == '\n') {
      lexer->position->line++;
      lexer->position->column = 0;
    } else {
      lexer->position->column++;
    }

    lexer->current_char = lexer_peek(lexer);
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
    } else if (isalpha(c)) {
      lexer_parse_keyword_or_identifier(lexer, value);

      if (lexer_is_keyword(value)) {
        if (strcmp(value, "let") == 0) {
          type = TT_LET;
        } else if (strcmp(value, "if") == 0) {
          type = TT_IF;
        } else if (strcmp(value, "elif") == 0) {
          type = TT_ELIF;
        } else if (strcmp(value, "else") == 0) {
          type = TT_ELSE;
        } else if (strcmp(value, "fn") == 0) {
          type = TT_FN;
        } else if (strcmp(value, "use") == 0) {
          type = TT_USE;
        } else if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
          type = TT_BOOL;
        } else if (strcmp(value, "null") == 0) {
          type = TT_NULL;
        } else if (strcmp(value, "while") == 0) {
          type = TT_WHILE;
        } else {
          panic("Unexpected keyword: %s", value);
        }
      } else {
        type = TT_IDENTIFIER;
      }
    } else if (c == '"') {
      type = TT_STRING;
      lexer_parse_string(lexer, value);
    } else {
      switch (c) {
        case '+':
          type = TT_PLUS;
          break;

        case '-':
          type = TT_MINUS;
          break;

        case '*':
          type = TT_MUL;
          break;

        case '/':
          type = TT_DIV;
          break;

        case '=':
          type = TT_ASSIGN;
          break;

        case '(':
          type = TT_LPAREN;
          break;

        case ')':
          type = TT_RPAREN;
          break;

        case '{':
          type = TT_LBRACE;
          break;

        case '}':
          type = TT_RBRACE;
          break;

        case '<':
          type = TT_LT;
          break;

        case '>':
          type = TT_GT;
          break;

        case '%':
          type = TT_MOD;
          break;

        case ',':
          type = TT_COMMA;
          break;

        default:
          lexer_error(lexer, "Unexpected character");
      }

      value[0] = c;
      value[1] = '\0';

      c = lexer_advance(lexer);
    }

    Token *token = token_init(value, type, lexer->position);
    dyn_array_append(lexer->tokens, token);
    free(value);
  }

  return lexer->tokens;
}

bool lexer_is_keyword(const char* keyword) {
  for (size_t i = 0; KEYWORDS[i] != NULL; i++) {
    if (strcmp(KEYWORDS[i], keyword) == 0) {
      return true;
    }
  }

  return false;
}

void lexer_parse_number(Lexer *lexer, char* value) {
  size_t index = 0;
  char c = lexer->current_char;

  while (c != '\0' && isdigit(c)) {
    value[index++] = c;
    c = lexer_advance(lexer);
  }

  value[index] = '\0';
}

void lexer_parse_keyword_or_identifier(Lexer *lexer, char* value) {
  size_t index = 0;
  char c = lexer->current_char;

  while (c != '\0' && isalpha(c)) {
    value[index++] = c;
    c = lexer_advance(lexer);
  }

  value[index] = '\0';
}

void lexer_error(const Lexer* lexer, char* message) {
  const char* input = dyn_array_get(lexer->input, lexer->position->line);
  size_t column = lexer->position->column;
  fprintf(stderr, ANSI_RED "\nLexer error at line %zu column %zu: %s\n"
          ANSI_RESET, lexer->position->line, lexer->position->column, message);
  fprintf(stderr, "%s", input);

  for (size_t i = 0; i < column; i++) {
    fputc(input[i] == '\t' ? '\t' : '~', stderr);
  }

  fprintf(stderr, ANSI_RED "^\n" ANSI_RESET);
  exit(1);
}

void lexer_parse_string(Lexer* lexer, char* value) {
  size_t index = 0;
  char c = lexer_advance(lexer);

  while (c != '\0' && c != '"') {
    if (c == '\\') {
      c = lexer_advance(lexer);

      if (c == '\0') {
        lexer_error(lexer, "Unexpected end of string");
      }

      switch (c) {
        case 'n':
          value[index++] = '\n';
          break;

        case 't':
          value[index++] = '\t';
          break;

        case '"':
          value[index++] = '"';
          break;

        case '\\':
          value[index++] = '\\';
          break;

        default:
          lexer_error(lexer, "Invalid escape sequence");
      }
    } else {
      value[index++] = c;
    }

    c = lexer_advance(lexer);
  }

  if (c != '"') {
    lexer_error(lexer, "Unterminated string");
  }

  value[index] = '\0';
  lexer_advance(lexer);
}
