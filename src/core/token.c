#include "yuji/core/token.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/string.h"
#include <string.h>

const char* yuji_token_type_to_string(const YujiTokenType type) {
#define _YUJI_TOKEN_TYPE_CASE(type) case type: return #type

  switch (type) {
      _YUJI_TOKEN_TYPE_CASE(TT_NUMBER);
      _YUJI_TOKEN_TYPE_CASE(TT_STRING);
      _YUJI_TOKEN_TYPE_CASE(TT_IDENTIFIER);
      _YUJI_TOKEN_TYPE_CASE(TT_LET);
      _YUJI_TOKEN_TYPE_CASE(TT_IF);
      _YUJI_TOKEN_TYPE_CASE(TT_ELIF);
      _YUJI_TOKEN_TYPE_CASE(TT_ELSE);
      _YUJI_TOKEN_TYPE_CASE(TT_FN);
      _YUJI_TOKEN_TYPE_CASE(TT_USE);
      _YUJI_TOKEN_TYPE_CASE(TT_BOOL);
      _YUJI_TOKEN_TYPE_CASE(TT_NULL);
      _YUJI_TOKEN_TYPE_CASE(TT_WHILE);
      _YUJI_TOKEN_TYPE_CASE(TT_ASSIGN);
      _YUJI_TOKEN_TYPE_CASE(TT_PLUS);
      _YUJI_TOKEN_TYPE_CASE(TT_MINUS);
      _YUJI_TOKEN_TYPE_CASE(TT_MUL);
      _YUJI_TOKEN_TYPE_CASE(TT_DIV);
      _YUJI_TOKEN_TYPE_CASE(TT_LPAREN);
      _YUJI_TOKEN_TYPE_CASE(TT_RPAREN);
      _YUJI_TOKEN_TYPE_CASE(TT_LBRACE);
      _YUJI_TOKEN_TYPE_CASE(TT_RBRACE);
      _YUJI_TOKEN_TYPE_CASE(TT_MOD);
      _YUJI_TOKEN_TYPE_CASE(TT_LT);
      _YUJI_TOKEN_TYPE_CASE(TT_GT);
      _YUJI_TOKEN_TYPE_CASE(TT_COMMA);
      _YUJI_TOKEN_TYPE_CASE(TT_QUOTE);
      _YUJI_TOKEN_TYPE_CASE(TT_EQ);
      _YUJI_TOKEN_TYPE_CASE(TT_NEQ);
      _YUJI_TOKEN_TYPE_CASE(TT_LTE);
      _YUJI_TOKEN_TYPE_CASE(TT_GTE);
      _YUJI_TOKEN_TYPE_CASE(TT_AND);
      _YUJI_TOKEN_TYPE_CASE(TT_OR);

    default:
      yuji_panic("Unknown token type: %d", type);
  }

#undef _YUJI_TOKEN_TYPE_CASE
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
  yuji_check_memory((void*)token);

  YujiString* str = yuji_string_init();

  yuji_string_append_cstr(str, "YujiToken {");
  yuji_string_append_cstr(str, ".value=\"");
  yuji_string_append_cstr(str, token->value);
  yuji_string_append_cstr(str, "\", .type=");
  yuji_string_append_cstr(str, yuji_token_type_to_string(token->type));
  yuji_string_append_cstr(str, ", .position=\"");
  char* pos_str = yuji_position_to_string(token->position);
  yuji_string_append_cstr(str, pos_str);
  yuji_free(pos_str);
  yuji_string_append_cstr(str, "\"}");

  const char* result = strdup(str->data);
  yuji_string_free(str);

  return result;
}
