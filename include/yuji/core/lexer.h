#pragma once

#include "yuji/core/position.h"
#include "yuji/core/types/dyn_array.h"

typedef struct {
  YujiDynArray* input;
  YujiPosition position;
  char current_char;
} YujiLexer;

YujiLexer* yuji_lexer_init(YujiDynArray* input, const char* file_name);
void yuji_lexer_free(YujiLexer* lexer);

char yuji_lexer_peek(YujiLexer* lexer);
char yuji_lexer_advance(YujiLexer* lexer);

bool yuji_lexer_tokenize(YujiLexer* lexer, YujiDynArray *tokens);

void yuji_lexer_skip_whitespace(YujiLexer* lexer);

char* yuji_lexer_parse_number(YujiLexer* lexer);
char* yuji_lexer_parse_string(YujiLexer* lexer);
char* yuji_lexer_parse_identifier_or_keyword(YujiLexer* lexer);

