#include "yuji/core/parser.h"
#include "yuji/core/memory.h"
#include "yuji/core/position.h"
#include "yuji/core/token.h"
#include "yuji/core/types/dyn_array.h"

YujiParser* yuji_parser_init(YujiDynArray* tokens, const char* filename) {
    YujiParser* parser = yuji_malloc(sizeof(YujiParser));

    parser->tokens = tokens;
    parser->position = yuji_position_init(0, 0, filename);
    parser->current_token = NULL;

    return parser;
}

void yuji_parser_free(YujiParser *parser) {
    yuji_free(parser);
}

