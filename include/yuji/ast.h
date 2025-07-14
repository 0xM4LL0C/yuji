#pragma once

#include "yuji/dyn_array.h"
typedef struct ASTNode ASTNode;

typedef struct {
  char* value;
} ASTNumber;

typedef struct {
  ASTNode* left;
  char* op;
  ASTNode* right;
} ASTBinOp;


typedef struct {
  char* value;
} ASTIdentifier;

typedef struct {
  ASTIdentifier* name;
  ASTNode* value;
} ASTLet;

typedef struct {
  ASTIdentifier* name;
  ASTNode* value;
} ASTAssign;

typedef struct {
  DynArr* expressions;
} ASTBlock;

typedef struct {
  ASTNode* condition;
  ASTNode* body;
} ASTIf;

typedef struct {
  ASTNode* condition;
  ASTNode* body;
} ASTElif;

typedef struct {
  ASTNode* condition;
  ASTNode* body;
} ASTElse;

typedef enum {
  AST_NUMBER,
  AST_BIN_OP,
  AST_LET,
  AST_IDENTIFIER,
  AST_ASSIGN,
  AST_BLOCK,
  AST_IF,
  AST_ELIF,
  AST_ELSE,
} ASTNodeType;

struct ASTNode {
  ASTNodeType type;
  union {
    ASTNumber number;
    ASTBinOp bin_op;
    ASTLet let;
    ASTIdentifier identifier;
    ASTAssign assign;
    ASTBlock block;
    ASTIf if_;
    ASTElif elif;
    ASTElse else_;
  };
};

void ast_free(ASTNode* node);

ASTNode* ast_number_init(const char* value);
ASTNode* ast_binop_init(ASTNode* left, const char* op, ASTNode* right);
ASTNode* ast_identifier_init(const char* value);
ASTNode* ast_let_init(ASTIdentifier* name, ASTNode* value);
ASTNode* ast_assign_init(ASTIdentifier* name, ASTNode* value);
ASTNode* ast_block_init(DynArr* expressions);
ASTNode* ast_if_init(ASTNode* condition, ASTNode* body);
ASTNode* ast_elif_init(ASTNode* condition, ASTNode* body);
ASTNode* ast_else_init(ASTNode* body);
