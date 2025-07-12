#pragma once

#include "yuji/ast.h"
#include "yuji/dyn_array.h"
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
bool parser_expect(Parser* parser, TokenType type);
DynArr* parser_parse(Parser* parser);

ASTNode* parser_parse_expr(Parser* parser);
ASTNode* parser_parse_term(Parser* parser);
ASTNode* parser_parse_factor(Parser* parser);

void parser_error(const Parser* parser,
                  char* message) __attribute__((noreturn));
