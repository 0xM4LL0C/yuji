#include "yuji/parser.h"
#include "yuji/ast.h"
#include "yuji/token.h"
#include "yuji/utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parser* parser_init(const DynArr* tokens) {
  Parser* parser = malloc(sizeof(Parser));
  check_memory_is_not_null(parser);
  parser->tokens = tokens;
  parser->ast = dyn_array_init();
  parser->pos = 0;
  parser->current_token = parser->tokens->size ? dyn_array_get(parser->tokens,
                          0) : NULL;
  return parser;
}

void parser_free(Parser* parser) {
  if (!parser) {
    return;
  }

  for (size_t i = 0; i < parser->ast->size; ++i)
    ast_free(dyn_array_get(parser->ast, i));

  dyn_array_free(parser->ast);
  free(parser);
}

Token* parser_advance(Parser* parser) {
  if (parser->pos < parser->tokens->size - 1) {
    parser->current_token = dyn_array_get(parser->tokens, ++parser->pos);
  } else {
    parser->current_token = NULL;
  }

  LOG("current token type: %s",
      parser->current_token ? tt_to_string(parser->current_token->type) : "NULL");
  return parser->current_token;
}

bool parser_match(Parser* parser, TokenType type) {
  return !parser->current_token || parser->current_token->type == type;
}

bool parser_match_next(Parser* parser, TokenType type) {
  if (!parser->current_token || parser->pos >= parser->tokens->size - 1) {
    return false;
  }

  Token* next_token = dyn_array_get(parser->tokens, parser->pos + 1);
  return next_token && next_token->type == type;
}

void parser_expect(Parser* parser, TokenType type) {
  if (!parser_match(parser, type)) {
    LOG("Excepted '%s', got '%s'", tt_to_string(parser->current_token->type),
        tt_to_string(type));
    parser_error(parser, "Unexpected token");
  }
}

void parser_expect_next(Parser* parser, TokenType type) {
  if (!parser_match_next(parser, type)) {
    LOG("Excepted '%s', got '%s'", tt_to_string(parser->current_token->type),
        tt_to_string(type));
    parser_error(parser, "Unexpected token");
  }
}

void parser_error(const Parser* parser, char* message) {
  panic("Parser error at position %zu: %s", parser->pos, message);
}

ASTNode* parser_parse_factor(Parser* parser) {
  if (!parser->current_token) {
    parser_error(parser, "Unexpected end of input");
  }

  if (parser->current_token->type == TT_NUMBER) {
    ASTNode* node = ast_number_init(parser->current_token->value);
    parser_advance(parser);
    return node;
  }

  if (parser->current_token->type == TT_STRING) {
    ASTNode* node = ast_string_init(parser->current_token->value);
    parser_advance(parser);
    return node;
  }

  if (parser->current_token->type == TT_IDENTIFIER) {
    if (parser_match_next(parser, TT_LPAREN)) {
      return parser_parse_call(parser);
    }

    ASTNode* node = ast_identifier_init(parser->current_token->value);
    parser_advance(parser);
    return node;
  }

  if (parser->current_token->type == TT_LPAREN) {
    parser_advance(parser);
    ASTNode* node = parser_parse_expr(parser);
    parser_expect(parser, TT_RPAREN);
    parser_advance(parser);
    return node;
  }

  if (parser->current_token->type == TT_IF) {
    return parser_parse_if(parser);
  }

  if (parser->current_token->type == TT_ELIF) {
    return parser_parse_elif(parser);
  }

  if (parser->current_token->type == TT_ELSE) {
    return parser_parse_else(parser);
  }

  if (parser->current_token->type == TT_FN) {
    return parser_parse_fn(parser);
  }

  if (parser->current_token->type == TT_USE) {
    return parser_parse_use(parser);
  }


  LOG("current token type: %s", tt_to_string(parser->current_token->type));
  parser_error(parser, "Expected number, string, or identifier");
}


ASTNode* parser_parse_term(Parser* parser) {
  ASTNode* node = parser_parse_factor(parser);

  while (parser->current_token &&
         (parser->current_token->type == TT_MUL
          || parser->current_token->type == TT_DIV
          || parser->current_token->type == TT_MOD)) {
    const char* op = parser->current_token->value;
    parser_advance(parser);
    ASTNode* right = parser_parse_factor(parser);
    node = ast_binop_init(node, op, right);
  }

  return node;
}

ASTNode* parser_parse_expr(Parser* parser) {
  ASTNode* node = parser_parse_term(parser);

  while (parser->current_token &&
         (parser->current_token->type == TT_PLUS ||
          parser->current_token->type == TT_MINUS)) {
    const char* op = parser->current_token->value;
    parser_advance(parser);
    ASTNode* right = parser_parse_term(parser);
    node = ast_binop_init(node, op, right);
  }

  if (parser->current_token &&
      (parser->current_token->type == TT_LT ||
       parser->current_token->type == TT_GT)) {
    const char* op = parser->current_token->value;
    parser_advance(parser);
    ASTNode* right = parser_parse_expr(parser);
    node = ast_binop_init(node, op, right);
  }

  return node;
}

DynArr* parser_parse(Parser* parser) {
  while (parser->current_token) {
    ASTNode* node = NULL;

    if (parser->current_token->type == TT_LET) {
      node = parser_parse_let(parser);
    } else if (parser->current_token->type == TT_IDENTIFIER) {
      if (parser_match_next(parser, TT_ASSIGN)) {
        node = parser_parse_assign(parser);
      } else {
        node = parser_parse_expr(parser);
      }
    } else if (parser->current_token->type == TT_IF) {
      node = parser_parse_if(parser);
    } else if (parser->current_token->type == TT_ELIF) {
      node = parser_parse_elif(parser);
    } else if (parser->current_token->type == TT_ELSE) {
      node = parser_parse_else(parser);
    } else if (parser->current_token->type == TT_FN) {
      node = parser_parse_fn(parser);
    } else {
      node = parser_parse_expr(parser);
    }

    dyn_array_append(parser->ast, node);
  }

  return parser->ast;
}


ASTNode* parser_parse_let(Parser* parser) {
  parser_expect(parser, TT_LET);

  if (strcmp(parser->current_token->value, "let") != 0) {
    parser_error(parser, "Expected 'let' keyword");
  }

  parser_advance(parser);

  parser_expect(parser, TT_IDENTIFIER);
  ASTIdentifier* id = malloc(sizeof(ASTIdentifier));
  check_memory_is_not_null(id);
  id->value = strdup(parser->current_token->value);
  parser_advance(parser);

  parser_expect(parser, TT_ASSIGN);

  if (strcmp(parser->current_token->value, "=") != 0) {
    parser_error(parser, "Expected '=' after identifier in let statement");
  }

  parser_advance(parser);

  ASTNode* value = parser_parse_expr(parser);

  return ast_let_init(id, value);
}

ASTNode* parser_parse_assign(Parser* parser) {
  parser_expect(parser, TT_IDENTIFIER);
  ASTIdentifier* id = malloc(sizeof(ASTIdentifier));
  check_memory_is_not_null(id);
  id->value = strdup(parser->current_token->value);
  parser_advance(parser);

  parser_expect(parser, TT_ASSIGN);

  if (strcmp(parser->current_token->value, "=") != 0) {
    parser_error(parser, "Expected '=' in assignment");
  }

  parser_advance(parser);

  ASTNode* value = parser_parse_expr(parser);
  return ast_assign_init(id, value);
}

ASTNode* parser_parse_block(Parser* parser) {
  parser_expect(parser, TT_LBRACE);
  parser_advance(parser);

  DynArr* expressions = dyn_array_init();

  while (parser->current_token && parser->current_token->type != TT_RBRACE) {
    ASTNode* expr = NULL;

    if (parser->current_token->type == TT_LET) {
      expr = parser_parse_let(parser);
    } else if (parser->current_token->type == TT_IDENTIFIER &&
               parser_match_next(parser, TT_ASSIGN)) {
      expr = parser_parse_assign(parser);
    } else if (parser->current_token->type == TT_IF) {
      expr = parser_parse_if(parser);
    } else if (parser->current_token->type == TT_ELIF) {
      expr = parser_parse_elif(parser);
    } else if (parser->current_token->type == TT_ELSE) {
      expr = parser_parse_else(parser);
    } else {
      expr = parser_parse_expr(parser);
    }

    dyn_array_append(expressions, expr);
  }

  parser_expect(parser, TT_RBRACE);
  parser_advance(parser);

  return ast_block_init(expressions);
}


ASTNode* parser_parse_if(Parser* parser) {
  parser_expect(parser, TT_IF);
  parser_advance(parser);

  ASTNode* condition = parser_parse_expr(parser);

  ASTNode* body = parser_parse_block(parser);

  return ast_if_init(condition, body);
}

ASTNode* parser_parse_elif(Parser* parser) {
  parser_expect(parser, TT_ELIF);
  parser_advance(parser);

  ASTNode* condition = parser_parse_expr(parser);

  ASTNode* body = parser_parse_block(parser);

  return ast_elif_init(condition, body);
}

ASTNode* parser_parse_else(Parser* parser) {
  parser_expect(parser, TT_ELSE);
  parser_advance(parser);

  ASTNode* body = parser_parse_block(parser);

  return ast_else_init(body);
}

ASTNode* parser_parse_fn(Parser* parser) {
  parser_expect(parser, TT_FN);
  parser_advance(parser);

  parser_expect(parser, TT_IDENTIFIER);
  ASTIdentifier* name = malloc(sizeof(ASTIdentifier));
  check_memory_is_not_null(name);
  name->value = strdup(parser->current_token->value);
  parser_advance(parser);

  parser_expect(parser, TT_LPAREN);
  parser_advance(parser);

  DynArr* params = dyn_array_init();

  while (parser->current_token && parser->current_token->type != TT_RPAREN) {
    parser_expect(parser, TT_IDENTIFIER);
    ASTIdentifier* param = malloc(sizeof(ASTIdentifier));
    check_memory_is_not_null(param);
    param->value = strdup(parser->current_token->value);
    dyn_array_append(params, param);
    parser_advance(parser);

    if (parser->current_token && parser->current_token->type == TT_COMMA) {
      parser_advance(parser);
    }
  }

  parser_expect(parser, TT_RPAREN);
  parser_advance(parser);

  ASTNode* body = parser_parse_block(parser);

  return ast_function_init(name, params, body);
}

ASTNode* parser_parse_call(Parser* parser) {
  parser_expect(parser, TT_IDENTIFIER);
  ASTIdentifier* name = malloc(sizeof(ASTIdentifier));
  check_memory_is_not_null(name);
  name->value = strdup(parser->current_token->value);
  parser_advance(parser);

  parser_expect(parser, TT_LPAREN);
  parser_advance(parser);

  DynArr* args = dyn_array_init();

  while (parser->current_token && parser->current_token->type != TT_RPAREN) {
    ASTNode* arg = parser_parse_expr(parser);
    dyn_array_append(args, arg);

    if (parser->current_token && parser->current_token->type == TT_COMMA) {
      parser_advance(parser);
    }
  }

  parser_expect(parser, TT_RPAREN);
  parser_advance(parser);

  return ast_call_init(name, args);
}

ASTNode* parser_parse_use(Parser* parser) {
  parser_expect(parser, TT_USE);
  parser_advance(parser);
  parser_expect(parser, TT_STRING);
  ASTNode* node = ast_use_init(parser->current_token->value);
  parser_advance(parser);
  return node;
}
