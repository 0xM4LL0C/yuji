#pragma once

#include "yuji/types/dyn_array.h"
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
  ASTNode* body;
} ASTElse;

typedef struct {
  ASTIdentifier* name;
  DynArr* params;
  ASTNode* body;
} ASTFunction;

typedef struct {
  ASTIdentifier* name;
  DynArr* args;
} ASTCall;

typedef struct {
  char* value;
} ASTString;

typedef struct {
  char* value;
} ASTUse;

typedef struct {
  bool value;
} ASTBool;

typedef struct ASTNull ASTNull;

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
  AST_FUNCTION,
  AST_CALL,
  AST_STRING,
  AST_USE,
  AST_BOOL,
  AST_NULL,
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
    ASTFunction function;
    ASTCall call;
    ASTString string;
    ASTUse use;
    ASTBool bool_;
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
ASTNode* ast_function_init(ASTIdentifier* name, DynArr* params, ASTNode* body);
ASTNode* ast_call_init(ASTIdentifier* name, DynArr* args);
ASTNode* ast_string_init(const char* value);
ASTNode* ast_use_init(const char* value);
ASTNode* ast_bool_init(const char* value);
ASTNode* ast_null_init();
