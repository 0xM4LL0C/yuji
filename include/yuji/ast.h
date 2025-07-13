#pragma once

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

typedef enum {
  AST_NUMBER,
  AST_BIN_OP,
  AST_LET,
  AST_IDENTIFIER,
} ASTNodeType;

struct ASTNode {
  ASTNodeType type;
  union {
    ASTNumber number;
    ASTBinOp bin_op;
    ASTLet let;
    ASTIdentifier identifier;
  };
};

void ast_free(ASTNode* node);

ASTNode* ast_number_init(const char* value);
ASTNode* ast_binop_init(ASTNode* left, const char* op, ASTNode* right);
ASTNode* ast_identifier_init(const char* value);
ASTNode* ast_let_init(ASTIdentifier* name, ASTNode* value);
