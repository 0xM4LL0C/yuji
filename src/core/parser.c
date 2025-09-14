#include "yuji/core/parser.h"
#include "yuji/core/memory.h"
#include "yuji/core/position.h"
#include "yuji/core/token.h"
#include "yuji/core/types/dyn_array.h"

YujiParser* yuji_parser_init(YujiDynArray* tokens) {
  YujiParser* parser = yuji_malloc(sizeof(YujiParser));

  parser->tokens = tokens;
  parser->index = 0;
  parser->current_token = NULL;

  return parser;
}

void yuji_parser_free(YujiParser *parser) {
  yuji_free(parser);
}


YujiToken* yuji_parser_advance(YujiParser* parser) {
  yuji_check_memory(parser);

  if (parser->index >= parser->tokens->size) {
    parser->current_token = NULL;
    return NULL;
  }

  YujiToken* token = yuji_dyn_array_get(parser->tokens, parser->index);

  parser->current_token = token;
  parser->index++;

  return token;
}

bool yuji_parser_match(YujiParser* parser, YujiTokenType type) {
  yuji_check_memory(parser);

  YujiToken* token = yuji_dyn_array_get(parser->tokens, parser->index);

  if (token->type == type) {
    return true;
  }

  return false;
}

bool yuji_parser_match_next(YujiParser* parser, YujiTokenType type) {
  yuji_check_memory(parser);

  YujiToken* token = yuji_dyn_array_get(parser->tokens, parser->index + 1);

  if (token->type == type) {
    return true;
  }

  return false;
}