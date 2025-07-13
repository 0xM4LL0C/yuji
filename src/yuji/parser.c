#include "yuji/parser.h"
#include "yuji/token.h"
#include "yuji/utils.h"
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

  return parser->current_token;
}

bool parser_match(Parser* parser, TokenType type) {
  return !parser->current_token || parser->current_token->type != type;
}

void parser_expect(Parser* parser, TokenType type) {
  if (parser_match(parser, type)) {
    parser_error(parser, "Unexpected token");
  }
}

void parser_error(const Parser* parser, char* message) {
  panic("Parser error at token %zu: %s", parser->pos, message);
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

  if (parser->current_token->type == TT_IDENTIFIER) {
    ASTNode* node = ast_identifier_init(parser->current_token->value);
    parser_advance(parser);
    return node;
  }

  parser_error(parser, "Expected number or identifier");
  return NULL;
}


ASTNode* parser_parse_term(Parser* parser) {
  ASTNode* node = parser_parse_factor(parser);

  while (parser->current_token &&
         (parser->current_token->type == TT_MUL
          || parser->current_token->type == TT_DIV) &&
         (strcmp(parser->current_token->value, "*") == 0 ||
          strcmp(parser->current_token->value, "/") == 0)) {
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
          parser->current_token->type == TT_MINUS) &&
         (strcmp(parser->current_token->value, "+") == 0 ||
          strcmp(parser->current_token->value, "-") == 0)) {
    const char* op = parser->current_token->value;
    parser_advance(parser);
    ASTNode* right = parser_parse_term(parser);
    node = ast_binop_init(node, op, right);
  }

  return node;
}

DynArr* parser_parse(Parser* parser) {
  while (parser->current_token) {
    ASTNode* node = NULL;

    if (parser->current_token->type == TT_LET &&
        strcmp(parser->current_token->value, "let") == 0) {
      node = parser_parse_let(parser);
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
