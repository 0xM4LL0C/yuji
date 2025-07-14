#include "yuji/ast.h"
#include "yuji/dyn_array.h"
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

    case AST_ASSIGN:
    case AST_LET:
      free(node->let.name);
      free(node->let.value);
      break;

    case AST_BLOCK:
      for (size_t i = 0; i < node->block.expressions->size; ++i) {
        ast_free(dyn_array_get(node->block.expressions, i));
      }

      dyn_array_free(node->block.expressions);
      break;

    case AST_IF:
      ast_free(node->if_.condition);
      ast_free(node->if_.body);
      break;

    case AST_ELIF:
      ast_free(node->elif.condition);
      ast_free(node->elif.body);
      break;

    case AST_ELSE:
      ast_free(node->else_.body);
      break;

    case AST_FUNCTION:
      free(node->function.name);

      for (size_t i = 0; i < node->function.params->size; ++i) {
        ast_free(dyn_array_get(node->function.params, i));
      }

      dyn_array_free(node->function.params);
      ast_free(node->function.body);
      break;

    case AST_CALL:
      free(node->call.name);

      for (size_t i = 0; i < node->call.args->size; ++i) {
        ast_free(dyn_array_get(node->call.args, i));
      }

      dyn_array_free(node->call.args);
      break;

    case AST_STRING:
      free(node->string.value);
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

ASTNode* ast_assign_init(ASTIdentifier* name, ASTNode* value) {
  ASTNode* n = malloc(sizeof(ASTNode));
  check_memory_is_not_null(n);
  n->type = AST_ASSIGN;
  n->assign.name = name;
  n->assign.value = value;
  return n;
}

ASTNode* ast_block_init(DynArr* expressions) {
  ASTNode* n = malloc(sizeof(ASTNode));
  check_memory_is_not_null(n);
  n->type = AST_BLOCK;
  n->block.expressions = expressions;
  return n;
}

ASTNode* ast_if_init(ASTNode* condition, ASTNode* body) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_IF;
  node->if_.condition = condition;
  node->if_.body = body;

  return node;
}

ASTNode* ast_elif_init(ASTNode* condition, ASTNode* body) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_ELIF;
  node->elif.condition = condition;
  node->elif.body = body;

  return node;
}

ASTNode* ast_else_init(ASTNode* body) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_ELSE;
  node->else_.body = body;

  return node;
}

ASTNode* ast_function_init(ASTIdentifier* name, DynArr* params, ASTNode* body) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_FUNCTION;
  node->function.name = name;
  node->function.params = params;
  node->function.body = body;

  return node;
}

ASTNode* ast_call_init(ASTIdentifier* name, DynArr* args) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);
  node->type = AST_CALL;
  node->call.name = name;
  node->call.args = args;
  return node;
}

ASTNode* ast_string_init(const char* value) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_STRING;
  node->string.value = strdup(value);

  return node;
}
