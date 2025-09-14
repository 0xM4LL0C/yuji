#include "yuji/core/token.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/string.h"
#include <string.h>

const char* yuji_token_type_to_string(const YujiTokenType type) {
  switch (type) {
    case TT_NUMBER:
      return "NUMBER";

    case TT_STRING:
      return "STRING";

    case TT_IDENTIFIER:
      return "IDENTIFIER";

    case TT_LET:
      return "LET";

    case TT_IF:
      return "IF";

    case TT_ELIF:
      return "ELIF";

    case TT_ELSE:
      return "ELSE";

    case TT_FN:
      return "FN";

    case TT_USE:
      return "USE";

    case TT_BOOL:
      return "BOOL";

    case TT_NULL:
      return "NULL";

    case TT_WHILE:
      return "WHILE";

    case TT_ASSIGN:
      return "ASSIGN";

    case TT_PLUS:
      return "PLUS";

    case TT_MINUS:
      return "MINUS";

    case TT_MUL:
      return "MUL";

    case TT_DIV:
      return "DIV";

    case TT_LPAREN:
      return "LPAREN";

    case TT_RPAREN:
      return "RPAREN";

    case TT_LBRACE:
      return "LBRACE";

    case TT_RBRACE:
      return "RBRACE";

    case TT_MOD:
      return "MOD";

    case TT_LT:
      return "LT";

    case TT_GT:
      return "GT";

    case TT_COMMA:
      return "COMMA";

    case TT_QUOTE:
      return "QUOTE";

    default:
      yuji_panic("Unknown token type: %d", type);
  }
}

YujiToken* yuji_token_init(const char* value, const YujiTokenType type, YujiPosition position) {
  YujiToken* token = yuji_malloc(sizeof(YujiToken));

  token->value = value;
  token->type = type;
  token->position = position;

  return token;
}

void yuji_token_free(YujiToken* token) {
  yuji_free((void*)token->value);
  yuji_free(token);
}

const char* yuji_token_to_string(const YujiToken* token) {
  yuji_check_memory(token);

  YujiString* str = yuji_string_init();

  yuji_string_append_cstr(str, "YujiToken {");
  yuji_string_append_cstr(str, ".value=");
  yuji_string_append_cstr(str, token->value);
  yuji_string_append_cstr(str, ", .type=");
  yuji_string_append_cstr(str, yuji_token_type_to_string(token->type));
  yuji_string_append_cstr(str, ", .position='");
  char* pos_str = yuji_position_to_string(token->position);
  yuji_string_append_cstr(str, pos_str);
  yuji_free(pos_str);
  yuji_string_append_cstr(str, "'}");

  const char* result = strdup(str->data);
  yuji_string_free(str);

  return result;
}
