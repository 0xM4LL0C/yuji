#pragma once

#include "yuji/ast.h"
#include "yuji/types/dyn_array.h"
#include "yuji/token.h"
#include <stdlib.h>

typedef struct {
  const DynArr* tokens;
  DynArr* ast;
  Token* current_token;
  size_t pos;
} Parser;

Parser* parser_init(const DynArr* tokens);
void parser_free(Parser* parser);

Token* parser_advance(Parser* parser);
bool parser_match(Parser* parser, TokenType type);
bool parser_match_next(Parser* parser, TokenType type);
void parser_expect(Parser* parser, TokenType type);
void parser_expect_next(Parser* parser, TokenType type);
DynArr* parser_parse(Parser* parser);

ASTNode* parser_parse_expr(Parser* parser);
ASTNode* parser_parse_term(Parser* parser);
ASTNode* parser_parse_factor(Parser* parser);

ASTNode* parser_parse_let(Parser* parser);
ASTNode* parser_parse_assign(Parser* parser);
ASTNode* parser_parse_block(Parser* parser);
ASTNode* parser_parse_if(Parser* parser);
ASTNode* parser_parse_fn(Parser* parser);
ASTNode* parser_parse_call(Parser* parser);
ASTNode* parser_parse_use(Parser* parser);
ASTNode* parser_parse_bool(Parser* parser);
ASTNode* parser_parse_null(Parser* parser);

void parser_error(const Parser* parser,
                  char* message) __attribute__((noreturn));
