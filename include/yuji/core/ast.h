#pragma once

#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include <stdint.h>

typedef struct YujiASTNode YujiASTNode;

YujiASTNode* yuji_ast_node_copy(YujiASTNode* node);

typedef struct {
  int64_t value;
} YujiASTInt;

typedef struct {
  double value;
} YujiASTFloat;

typedef struct {
  YujiString* value;
} YujiASTString;

typedef struct {
  YujiASTNode* left;
  YujiASTNode* right;
  char* operator;
} YujiASTBinOp;

typedef struct {
  char* value;
} YujiASTIdentifier;

typedef struct {
  char* name;
  YujiASTNode* value;
} YujiASTAssign;

typedef struct {
  char* name;
  YujiASTNode* value;
} YujiASTLet;

typedef struct {
  YujiDynArray* exprs;
} YujiASTBlock;

typedef struct {
  char* name;
  YujiDynArray* params;
  YujiASTNode* body;
} YujiASTFunction;

typedef struct {
  char* name;
  YujiDynArray* args;
} YujiASTCall;

typedef struct {
  char* value;
} YujiASTUse;

typedef struct {
  bool value;
} YujiASTBool;

typedef struct {
  YujiASTNode* condition;
  YujiASTBlock* body;
} YujiASTWhile;

typedef struct {} YujiASTNull;

typedef struct {
  YujiASTNode* condition;
  YujiASTBlock* body;
} YujiASTIfBranch;

typedef struct {
  YujiDynArray* branches;
  YujiASTBlock* else_body;
} YujiASTIf;

typedef struct {
  YujiASTNode* value;
} YujiASTReturn;

typedef struct {} YujiASTBreak;

typedef enum {
  YUJI_AST_INT,
  YUJI_AST_FLOAT,
  YUJI_AST_STRING,
  YUJI_AST_BIN_OP,
  YUJI_AST_IDENTIFIER,
  YUJI_AST_ASSIGN,
  YUJI_AST_LET,
  YUJI_AST_BLOCK,
  YUJI_AST_FN,
  YUJI_AST_CALL,
  YUJI_AST_USE,
  YUJI_AST_BOOL,
  YUJI_AST_WHILE,
  YUJI_AST_IF,
  YUJI_AST_NULL,
  YUJI_AST_RETURN,
  YUJI_AST_BREAK
} YujiASTNodeType;

struct YujiASTNode {
  YujiASTNodeType type;

  union {
    YujiASTInt* int_;
    YujiASTFloat* float_;
    YujiASTString* string;
    YujiASTBinOp* bin_op;
    YujiASTIdentifier* identifier;
    YujiASTAssign* assign;
    YujiASTLet* let;
    YujiASTBlock* block;
    YujiASTFunction* fn;
    YujiASTCall* call;
    YujiASTUse* use;
    YujiASTBool* boolean;
    YujiASTWhile* while_stmt;
    YujiASTIf* if_stmt;
    YujiASTNull* null;
    YujiASTReturn* return_stmt;
    YujiASTBreak* break_stmt;
  } value;
};

#define YUJI_AST_INIT(NAME, TYPE, BODY, ...) \
  YujiASTNode *yuji_ast_##NAME##_init(__VA_ARGS__) { \
    YujiASTNode* node = yuji_malloc(sizeof(YujiASTNode)); \
    node->type = TYPE;\
    BODY;\
    return node;\
  }

void yuji_ast_free(YujiASTNode* node);

char* yuji_ast_node_type_to_string(YujiASTNodeType type);

YujiASTNode* yuji_ast_int_init(int64_t value);
YujiASTNode* yuji_ast_float_init(double value);
YujiASTNode* yuji_ast_string_init(const char* value);
YujiASTNode* yuji_ast_bin_op_init(YujiASTNode* left, const char* operator, YujiASTNode* right);
YujiASTNode* yuji_ast_identifier_init(const char* name);
YujiASTNode* yuji_ast_assign_init(const char* name, YujiASTNode* value);
YujiASTNode* yuji_ast_let_init(const char* name, YujiASTNode* value);
YujiASTNode* yuji_ast_block_init(YujiDynArray* exprs);
YujiASTNode* yuji_ast_fn_init(const char* name, YujiDynArray* params, YujiASTBlock* body);
YujiASTNode* yuji_ast_call_init(const char* name, YujiDynArray* args);
YujiASTNode* yuji_ast_use_init(const char* value);
YujiASTNode* yuji_ast_bool_init(bool value);
YujiASTNode* yuji_ast_while_init(YujiASTNode* condition, YujiASTBlock* body);
YujiASTIfBranch* yuji_ast_if_branch_init(YujiASTNode* condition, YujiASTBlock* body);
YujiASTNode* yuji_ast_if_init(YujiDynArray* branches, YujiASTBlock* else_body);
YujiASTBlock* yuji_ast_extract_block(YujiASTNode* block_node);
YujiASTNode* yuji_ast_null_init();
YujiASTNode* yuji_ast_return_init(YujiASTNode* value);
YujiASTNode* yuji_ast_break_init();
