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

  if (!parser->current_token) {
    return false;
  }

  YujiToken* token = parser->current_token;
  return token->type == type;
}

bool yuji_parser_match_next(YujiParser *parser, YujiTokenType type) {
  yuji_check_memory(parser);

  YujiToken *token = yuji_dyn_array_get(parser->tokens, parser->index + 1);

  if (!token) {
    return false;
  }

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

  YujiASTNode* block = yuji_ast_block_init(exprs);
  yuji_dyn_array_free(exprs);
  return block;
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
    yuji_parser_advance(parser);

    YujiDynArray* param_names = yuji_dyn_array_init();

    if (!yuji_parser_match(parser, TT_RPAREN)) {
      do {
        yuji_parser_expect(parser, TT_IDENTIFIER);
        char* param = strdup(parser->current_token->value);
        yuji_dyn_array_push(param_names, param);
        yuji_parser_advance(parser);
      } while (yuji_parser_match(parser, TT_COMMA) && yuji_parser_advance(parser));
    }

    yuji_parser_expect(parser, TT_RPAREN);
    yuji_parser_advance(parser);
    YujiASTNode* block_node = yuji_parser_parse_block(parser);
    YujiASTNode* fn = yuji_ast_fn_init(name, param_names, block_node->value.block);
    yuji_ast_free(block_node);

    YUJI_DYN_ARRAY_ITER(param_names, char*, param, {
      yuji_free(param);
    });
    yuji_dyn_array_free(param_names);

    return fn;
  } else if (yuji_parser_match(parser, TT_IF)) {
    yuji_parser_advance(parser);

    YujiDynArray* branches = yuji_dyn_array_init();

    YujiASTNode* condition = yuji_parser_parse_expr(parser);
    YujiASTNode* body = yuji_parser_parse_block(parser);
    YujiASTIfBranch* if_branch = yuji_ast_if_branch_init(condition, yuji_ast_extract_block(body));
    yuji_dyn_array_push(branches, if_branch);

    YujiASTBlock* else_body = NULL;

    while (yuji_parser_match(parser, TT_ELIF)) {
      yuji_parser_advance(parser);
      YujiASTNode* elif_condition = yuji_parser_parse_expr(parser);
      YujiASTNode* elif_body = yuji_parser_parse_block(parser);
      YujiASTIfBranch* elif_branch = yuji_ast_if_branch_init(elif_condition,
                                     yuji_ast_extract_block(elif_body));
      yuji_dyn_array_push(branches, elif_branch);
    }

    if (yuji_parser_match(parser, TT_ELSE)) {
      yuji_parser_advance(parser);
      YujiASTNode* else_node = yuji_parser_parse_block(parser);
      else_body = yuji_ast_extract_block(else_node);
    }

    return yuji_ast_if_init(branches, else_body);
  } else if (yuji_parser_match(parser, TT_WHILE)) {
    yuji_parser_advance(parser);

    YujiASTNode* condition = yuji_parser_parse_expr(parser);

    yuji_parser_expect(parser, TT_LBRACE);
    YujiASTNode* body = yuji_parser_parse_block(parser);
    YujiASTNode* while_node = yuji_ast_while_init(condition, yuji_ast_extract_block(body));
    return while_node;
  } else if (yuji_parser_match(parser, TT_USE)) {
    yuji_parser_advance(parser);

    yuji_parser_expect(parser, TT_STRING);
    const char* name = parser->current_token->value;
    yuji_parser_advance(parser);

    return yuji_ast_use_init(name);
  } else if (yuji_parser_match(parser, TT_IDENTIFIER) && yuji_parser_match_next(parser, TT_ASSIGN)) {
    const char* name = parser->current_token->value;
    yuji_parser_advance(parser);
    yuji_parser_advance(parser);

    YujiASTNode* value = yuji_parser_parse_expr(parser);
    return yuji_ast_assign_init(name, value);
  } else if (yuji_parser_match(parser, TT_RETURN)) {
    yuji_parser_advance(parser);

    YujiASTNode* value = yuji_parser_parse_expr(parser);
    return yuji_ast_return_init(value);
  } else if (yuji_parser_match(parser, TT_BREAK)) {
    yuji_parser_advance(parser);

    return yuji_ast_break_init();
  } else if (yuji_parser_match(parser, TT_CONTINUE)) {
    yuji_parser_advance(parser);

    return yuji_ast_continue_init();
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
          yuji_parser_match(parser, TT_LT) ||
          yuji_parser_match(parser, TT_EQ) ||
          yuji_parser_match(parser, TT_NEQ) ||
          yuji_parser_match(parser, TT_GTE) ||
          yuji_parser_match(parser, TT_LTE) ||
          yuji_parser_match(parser, TT_AND) ||
          yuji_parser_match(parser, TT_OR))) {
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
      if (strchr(token->value, '.') != NULL) {
        double v = strtod(token->value, NULL);
        yuji_parser_advance(parser);
        return yuji_ast_float_init(v);
      } else {
        int64_t v = strtoll(token->value, NULL, 10);
        yuji_parser_advance(parser);
        return yuji_ast_int_init(v);
      }
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
      yuji_parser_advance(parser);
      return expr;
    }

    default:
      yuji_panic("parser error: unexpected token, got %s at %s",
                 yuji_token_type_to_string(parser->current_token->type),
                 yuji_position_to_string(parser->current_token->position));
  }
}
