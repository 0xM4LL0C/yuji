#include "yuji/core/parser.h"
#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/token.h"
#include "yuji/core/types/dyn_array.h"
#include <stdlib.h>
#include <string.h>

YujiParser* yuji_parser_init(YujiDynArray* tokens) {
  YujiParser* parser = yuji_malloc(sizeof(YujiParser));

  parser->tokens = tokens;
  parser->index = 0;
  parser->current_token = yuji_dyn_array_get(parser->tokens, parser->index);

  return parser;
}

void yuji_parser_free(YujiParser *parser) {
  yuji_free(parser);
}

YujiToken* yuji_parser_advance(YujiParser* parser) {
  yuji_check_memory(parser);

  parser->index++;

  if (parser->index >= parser->tokens->size) {
    parser->current_token = NULL;
    return NULL;
  }

  YujiToken* token = yuji_dyn_array_get(parser->tokens, parser->index);
  parser->current_token = token;
  return token;
}


bool yuji_parser_match(YujiParser* parser, YujiTokenType type) {
  yuji_check_memory(parser);
  yuji_check_memory(parser->current_token);
  YujiToken* token = parser->current_token;
  return token->type == type;
}

bool yuji_parser_match_next(YujiParser *parser, YujiTokenType type) {
  yuji_check_memory(parser);

  YujiToken *token = yuji_dyn_array_get(parser->tokens, parser->index + 1);
  yuji_check_memory(token);
  return token->type == type;
}

void yuji_parser_expect(YujiParser* parser, YujiTokenType type) {
  yuji_check_memory(parser);

  if (!parser->current_token) {
    yuji_panic("parser error: unexpected end of file");
  }

  if (!yuji_parser_match(parser, type)) {
    yuji_panic("parser error: expected %s, got %s at %s",
               yuji_token_type_to_string(type),
               yuji_token_type_to_string(parser->current_token->type),
               yuji_position_to_string(parser->current_token->position));
  }
}

void yuji_parser_except_next(YujiParser* parser, YujiTokenType type) {
  yuji_check_memory(parser);

  parser->index++;

  if (!yuji_parser_match(parser, type)) {
    parser->index--;
    yuji_panic("parser error: expected %s, got %s at %s",
               yuji_token_type_to_string(type),
               yuji_token_type_to_string(parser->current_token->type),
               yuji_position_to_string(parser->current_token->position));
  }

  parser->index--;
}

YujiDynArray* yuji_parser_parse(YujiParser* parser) {
  YujiDynArray* ast = yuji_dyn_array_init();

  while (parser->current_token) {
    YujiASTNode* stmt = yuji_parser_parse_stmt(parser);
    yuji_dyn_array_push(ast, stmt);
  }


  return ast;
}

YujiASTNode* yuji_parser_parse_block(YujiParser* parser) {
  yuji_check_memory(parser);
  YujiDynArray* exprs = yuji_dyn_array_init();

  if (yuji_parser_match(parser, TT_LBRACE)) {
    yuji_parser_advance(parser);

    while (parser->current_token && !yuji_parser_match(parser, TT_RBRACE)) {
      YujiASTNode* expr = yuji_parser_parse_stmt(parser);
      yuji_dyn_array_push(exprs, expr);
    }

    yuji_parser_expect(parser, TT_RBRACE);
    yuji_parser_advance(parser);
  } else {
    YujiASTNode* expr = yuji_parser_parse_stmt(parser);
    yuji_dyn_array_push(exprs, expr);
  }

  return yuji_ast_block_init(exprs);
}

YujiASTNode* yuji_parser_parse_stmt(YujiParser* parser) {
  yuji_check_memory(parser);

  if (yuji_parser_match(parser, TT_LET)) {
    yuji_parser_advance(parser);

    yuji_parser_expect(parser, TT_IDENTIFIER);
    const char* name = parser->current_token->value;

    yuji_parser_advance(parser);
    yuji_parser_expect(parser, TT_ASSIGN);
    yuji_parser_advance(parser);

    YujiASTNode* value = yuji_parser_parse_expr(parser);

    return yuji_ast_let_init(name, value);
  } else if (yuji_parser_match(parser, TT_FN)) {
    yuji_parser_advance(parser);

    yuji_parser_expect(parser, TT_IDENTIFIER);
    const char* name = parser->current_token->value;

    yuji_parser_advance(parser);
    yuji_parser_expect(parser, TT_LPAREN);

    YujiDynArray* params = yuji_dyn_array_init();

    if (!yuji_parser_match(parser, TT_RPAREN)) {
      do {
        yuji_parser_expect(parser, TT_IDENTIFIER);
        char* param = strdup(parser->current_token->value);
        yuji_dyn_array_push(params, param);
        yuji_parser_advance(parser);
      } while (yuji_parser_match(parser, TT_COMMA) && yuji_parser_advance(parser));
    }

    yuji_parser_expect(parser, TT_RPAREN);
    yuji_parser_expect(parser, TT_LBRACE);

    YujiASTNode* body = yuji_parser_parse_block(parser);
    YujiASTBlock* block = body->value.block;
    yuji_ast_free(body);
    return yuji_ast_fn_init(name, params, block);
  } else if (yuji_parser_match(parser, TT_IF)) {
    yuji_parser_advance(parser);

    YujiDynArray* branches = yuji_dyn_array_init();

    YujiASTNode* condition = yuji_parser_parse_expr(parser);
    yuji_parser_expect(parser, TT_LBRACE);
    YujiASTNode* body = yuji_parser_parse_block(parser);
    yuji_parser_expect(parser, TT_RBRACE);
    YujiASTBlock* block = body->value.block;
    yuji_ast_free(body);
    yuji_dyn_array_push(branches, yuji_ast_if_branch_init(condition, block));

    YujiASTBlock* else_body = NULL;

    while (yuji_parser_match(parser, TT_ELIF)) {
      yuji_parser_advance(parser);
      yuji_parser_expect(parser, TT_LBRACE);
      YujiASTNode* elif_condition = yuji_parser_parse_expr(parser);
      YujiASTNode* elif_body = yuji_parser_parse_block(parser);
      yuji_parser_expect(parser, TT_RBRACE);
      YujiASTBlock* elif_block = elif_body->value.block;
      yuji_free(elif_body);
      yuji_dyn_array_push(branches, yuji_ast_if_branch_init(elif_condition, elif_block));
    }

    if (yuji_parser_match(parser, TT_ELSE)) {
      yuji_parser_advance(parser);
      YujiASTNode* else_node = yuji_parser_parse_block(parser);
      else_body = else_node->value.block;
      yuji_free(else_node);
    }

    return yuji_ast_if_init(branches, else_body);
  } else if (yuji_parser_match(parser, TT_WHILE)) {
    yuji_parser_advance(parser);

    YujiASTNode* condition = yuji_parser_parse_expr(parser);

    yuji_parser_expect(parser, TT_LBRACE);
    YujiASTNode* body = yuji_parser_parse_block(parser);
    YujiASTBlock* block = body->value.block;
    yuji_free(body);
    return yuji_ast_while_init(condition, block);
  } else if (yuji_parser_match(parser, TT_USE)) {
    yuji_parser_advance(parser);

    yuji_parser_expect(parser, TT_STRING);
    const char* name = parser->current_token->value;
    yuji_parser_advance(parser);

    return yuji_ast_use_init(name);
  }

  if (yuji_parser_match(parser, TT_IDENTIFIER) && yuji_parser_match_next(parser, TT_ASSIGN)) {
    const char* name = parser->current_token->value;
    yuji_parser_advance(parser);
    yuji_parser_advance(parser);

    YujiASTNode* value = yuji_parser_parse_expr(parser);
    return yuji_ast_assign_init(name, value);
  }

  return yuji_parser_parse_expr(parser);
}

YujiASTNode* yuji_parser_parse_expr(YujiParser* parser) {
  yuji_check_memory(parser);
  YujiASTNode* node = yuji_parser_parse_term(parser);

  while (parser->current_token &&
         (yuji_parser_match(parser, TT_PLUS) ||
          yuji_parser_match(parser, TT_MINUS) ||
          yuji_parser_match(parser, TT_GT) ||
          yuji_parser_match(parser, TT_LT))) {
    char* op = (char*)parser->current_token->value;
    yuji_parser_advance(parser);
    YujiASTNode* right = yuji_parser_parse_term(parser);
    node = yuji_ast_bin_op_init(node, op, right);
  }

  return node;
}

YujiASTNode* yuji_parser_parse_term(YujiParser* parser) {
  yuji_check_memory(parser);
  YujiASTNode* node = yuji_parser_parse_factor(parser);

  while (parser->current_token &&
         (yuji_parser_match(parser, TT_MUL) ||
          yuji_parser_match(parser, TT_DIV) ||
          yuji_parser_match(parser, TT_MOD))) {
    char* op = (char*)parser->current_token->value;
    yuji_parser_advance(parser);
    YujiASTNode* right = yuji_parser_parse_factor(parser);
    node = yuji_ast_bin_op_init(node, op, right);
  }

  return node;
}

YujiASTNode* yuji_parser_parse_factor(YujiParser* parser) {
  yuji_check_memory(parser);
  yuji_check_memory(parser->current_token);

  YujiToken* token = parser->current_token;

  switch (token->type) {
    case TT_NUMBER: {
      int64_t value = strtoll(parser->current_token->value, NULL, 10);
      yuji_parser_advance(parser);
      return yuji_ast_number_init(value);
    }

    case TT_STRING: {
      YujiASTNode* node = yuji_ast_string_init(token->value);
      yuji_parser_advance(parser);
      return node;
    }

    case TT_IDENTIFIER: {
      const char* name = token->value;
      yuji_parser_advance(parser);

      if (yuji_parser_match(parser, TT_LPAREN)) {
        yuji_parser_advance(parser);

        YujiDynArray* args = yuji_dyn_array_init();

        if (!yuji_parser_match(parser, TT_RPAREN)) {
          YujiASTNode* arg = yuji_parser_parse_expr(parser);
          yuji_dyn_array_push(args, arg);

          while (yuji_parser_match(parser, TT_COMMA)) {
            yuji_parser_advance(parser);
            arg = yuji_parser_parse_expr(parser);
            yuji_dyn_array_push(args, arg);
          }
        }

        yuji_parser_expect(parser, TT_RPAREN);
        yuji_parser_advance(parser);
        return yuji_ast_call_init(name, args);
      }

      return yuji_ast_identifier_init(name);
    }

    case TT_BOOL: {
      bool value = strcmp(token->value, "true") == 0;
      yuji_parser_advance(parser);
      return yuji_ast_bool_init(value);
    }

    case TT_NULL: {
      yuji_parser_advance(parser);
      return yuji_ast_null_init();
    }

    case TT_LPAREN: {
      yuji_parser_advance(parser);
      YujiASTNode* expr = yuji_parser_parse_expr(parser);
      yuji_parser_expect(parser, TT_RPAREN);
      return expr;
    }

    default:
      yuji_panic("parser error: unexpected token, got %s at %s",
                 yuji_token_type_to_string(parser->current_token->type),
                 yuji_position_to_string(parser->current_token->position));
  }
}
