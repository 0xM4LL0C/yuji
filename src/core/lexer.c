#include "yuji/core/lexer.h"
#include "yuji/core/memory.h"
#include "yuji/core/position.h"
#include "yuji/core/token.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include "yuji/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

YujiLexer* yuji_lexer_init(YujiDynArray* input, const char* file_name) {
  YujiLexer* lexer = yuji_malloc(sizeof(YujiLexer));

  lexer->input = input;
  lexer->position = yuji_position_init(0, 0, file_name);
  lexer->current_char = yuji_lexer_peek(lexer);

  return lexer;
}

void yuji_lexer_free(YujiLexer* lexer) {
  yuji_free(lexer);
}

char yuji_lexer_peek(YujiLexer* lexer) {
  if (lexer->position.line >= lexer->input->size) {
    return '\0';
  }

  char* line = yuji_dyn_array_get(lexer->input, lexer->position.line);

  if (!line || lexer->position.column >= strlen(line)) {
    return '\0';
  }

  return line[lexer->position.column];
}

char yuji_lexer_advance(YujiLexer* lexer) {
  if (lexer->position.line >= lexer->input->size) {
    lexer->current_char = '\0';
    return '\0';
  }

  char* line = yuji_dyn_array_get(lexer->input, lexer->position.line);

  if (!line || lexer->position.column >= strlen(line)) {
    lexer->position.line++;
    lexer->position.column = 0;

    if (lexer->position.line >= lexer->input->size) {
      lexer->current_char = '\0';
      return '\0';
    }

    line = yuji_dyn_array_get(lexer->input, lexer->position.line);

    if (!line || line[0] == '\0') {
      lexer->current_char = '\0';
      return '\0';
    }

    lexer->current_char = line[0];
  } else {
    lexer->position.column++;

    if (lexer->position.column < strlen(line)) {
      lexer->current_char = line[lexer->position.column];
    } else {
      lexer->position.line++;
      lexer->position.column = 0;

      if (lexer->position.line >= lexer->input->size) {
        lexer->current_char = '\0';
        return '\0';
      }

      line = yuji_dyn_array_get(lexer->input, lexer->position.line);
      lexer->current_char = (line && line[0] != '\0') ? line[0] : '\0';
    }
  }

  return lexer->current_char;
}

bool yuji_lexer_tokenize(YujiLexer* lexer, YujiDynArray* tokens) {
  yuji_check_memory(lexer);
  yuji_check_memory(tokens);

  while (lexer->current_char != '\0') {
    char c = lexer->current_char;

    if (isspace(c)) {
      yuji_lexer_skip_whitespace(lexer);
      continue;
    }

    YUJI_LOG("current char: %c", c);

    YujiTokenType type;
    char* value;

    if (isalpha(c)) {
      value = yuji_lexer_parse_identifier_or_keyword(lexer);

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
        type = TT_IDENTIFIER;
      }
    } else if (c == '"') {
      value = yuji_lexer_parse_string(lexer);
      type = TT_STRING;
    } else if (isdigit(c)) {
      value = yuji_lexer_parse_number(lexer);
      type = TT_NUMBER;
    } else {
#define _YUJI_LEXER_OPERATOR_CASE(VALUE, TYPE) case VALUE: type = TYPE; break;

      switch (c) {
          _YUJI_LEXER_OPERATOR_CASE('=', TT_ASSIGN)
          _YUJI_LEXER_OPERATOR_CASE('+', TT_PLUS)
          _YUJI_LEXER_OPERATOR_CASE('-', TT_MINUS)
          _YUJI_LEXER_OPERATOR_CASE('*', TT_MUL)
          _YUJI_LEXER_OPERATOR_CASE('/', TT_DIV)
          _YUJI_LEXER_OPERATOR_CASE('(', TT_LPAREN)
          _YUJI_LEXER_OPERATOR_CASE(')', TT_RPAREN)
          _YUJI_LEXER_OPERATOR_CASE('{', TT_LBRACE)
          _YUJI_LEXER_OPERATOR_CASE('}', TT_RBRACE)
          _YUJI_LEXER_OPERATOR_CASE('%', TT_MOD)
          _YUJI_LEXER_OPERATOR_CASE('>', TT_GT)
          _YUJI_LEXER_OPERATOR_CASE('<', TT_LT)
          _YUJI_LEXER_OPERATOR_CASE(',', TT_COMMA)

        default:
          yuji_panic("lexer error: unknown operator '%c' (%d) at %s", c, c,
                     yuji_position_to_string(lexer->position));
      }

#undef _YUJI_LEXER_OPERATOR_CASE

      value = yuji_malloc(2);
      value[0] = c;
      value[1] = '\0';

      yuji_lexer_advance(lexer);
    }

    yuji_dyn_array_push(tokens, yuji_token_init(value, type, lexer->position));
  }

  return true;
}

void yuji_lexer_skip_whitespace(YujiLexer* lexer) {
  while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
    if (lexer->current_char == '\n') {
      lexer->position.line++;
      lexer->position.column = 0;
    } else {
      lexer->position.column++;
    }

    lexer->current_char = yuji_lexer_peek(lexer);
  }
}

char* yuji_lexer_parse_number(YujiLexer* lexer) {
  YujiString* str = yuji_string_init();
  char c = lexer->current_char;

  while (c != '\0' && isdigit(c)) {
    yuji_string_append_char(str, c);
    c = yuji_lexer_advance(lexer);
  }

  char* result = strdup(str->data);
  yuji_string_free(str);

  return result;
}

char* yuji_lexer_parse_string(YujiLexer* lexer) {
  YujiString* str = yuji_string_init();
  char c = yuji_lexer_advance(lexer);

  while (c != '\0' && c != '"') {
    if (c == '\\') {
      c = yuji_lexer_advance(lexer);

      if (c == '\0') {
        yuji_panic("lexer error: unexpected end of string at %s", yuji_position_to_string(lexer->position));
      }

      switch (c) {
        case 'n':
          yuji_string_append_char(str, '\n');
          break;

        case 't':
          yuji_string_append_char(str, '\t');
          break;

        case '"':
          yuji_string_append_char(str, '"');
          break;

        case '\\':
          yuji_string_append_char(str, '\\');
          break;

        default:
          yuji_panic("lexer error: invalid escape sequence at %s", yuji_position_to_string(lexer->position));
      }
    } else {
      yuji_string_append_char(str, c);
    }

    c = yuji_lexer_advance(lexer);
  }

  if (c != '"') {
    yuji_panic("lexer error: unterminated string at %s", yuji_position_to_string(lexer->position));
  }

  yuji_lexer_advance(lexer);

  char* result = strdup(str->data);
  yuji_string_free(str);

  return result;
}

char* yuji_lexer_parse_identifier_or_keyword(YujiLexer* lexer) {
  YujiString* str = yuji_string_init();
  char c = lexer->current_char;

  while (c != '\0' && (isalpha(c) || isdigit(c) || c == '_')) {
    yuji_string_append_char(str, c);
    c = yuji_lexer_advance(lexer);
  }

  char* result = strdup(str->data);
  yuji_string_free(str);

  return result;
}
