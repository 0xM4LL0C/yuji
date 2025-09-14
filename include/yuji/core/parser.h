#pragma once

#include "yuji/core/position.h"
#include "yuji/core/token.h"
#include "yuji/core/types/dyn_array.h"
#include <stdbool.h>

typedef struct {
  YujiDynArray* tokens;
  size_t index;
  YujiToken* current_token;
} YujiParser;

YujiParser* yuji_parser_init(YujiDynArray* tokens);
void yuji_parser_free(YujiParser* parser);

YujiToken* yuji_parser_advance(YujiParser* parser);

bool yuji_parser_match(YujiParser* parser, YujiTokenType type);
bool yuji_parser_match_next(YujiParser* parser, YujiTokenType type);

bool yuji_parser_expect(YujiParser* parser, YujiTokenType type);
bool yuji_parser_expect_next(YujiParser* parser, YujiTokenType type);