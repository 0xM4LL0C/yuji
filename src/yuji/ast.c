#include "yuji/ast.h"
#include "yuji/utils.h"
#include <stdlib.h>
#include <string.h>

void ast_free(ASTNode* node) {
  if (!node) {
    return;
  }

  switch (node->type) {
    case AST_NUMBER:
      free(node->number.value);
      break;

    case AST_BIN_OP:
      ast_free(node->bin_op.left);
      ast_free(node->bin_op.right);
      free(node->bin_op.op);
      break;

    case AST_IDENTIFIER:
      free(node->identifier.value);
      break;

    case AST_LET:
      free(node->let.name);
      free(node->let.value);
      break;
  }

  free(node);
}

ASTNode* ast_number_init(const char* value) {
  ASTNode* n = malloc(sizeof(ASTNode));
  check_memory_is_not_null(n);
  n->type = AST_NUMBER;
  n->number.value = strdup(value);
  return n;
}

ASTNode* ast_binop_init(ASTNode* left, const char* op, ASTNode* right) {
  ASTNode* n = malloc(sizeof(ASTNode));
  check_memory_is_not_null(n);
  n->type = AST_BIN_OP;
  n->bin_op.left = left;
  n->bin_op.op = strdup(op);
  n->bin_op.right = right;
  return n;
}

ASTNode* ast_identifier_init(const char* value) {
  ASTNode* n = malloc(sizeof(ASTNode));
  check_memory_is_not_null(n);
  n->type = AST_IDENTIFIER;
  n->identifier.value = strdup(value);
  return n;
}

ASTNode* ast_let_init(ASTIdentifier* name, ASTNode* value) {
  ASTNode* n = malloc(sizeof(ASTNode));
  check_memory_is_not_null(n);
  n->type = AST_LET;
  n->let.name = name;
  n->let.value = value;
  return n;
}
