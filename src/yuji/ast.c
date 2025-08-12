#include "yuji/ast.h"
#include "yuji/types/dyn_array.h"
#include "yuji/utils.h"
#include <stdlib.h>
#include <string.h>

void ast_free(ASTNode* node) {
  if (!node) {
    return;
  }

  switch (node->type) {
    case AST_BOOL:
    case AST_NULL:
      break;

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
      free(node->let.name->value);
      free(node->let.name);
      ast_free(node->let.value);
      break;

    case AST_ASSIGN:
      free(node->assign.name->value);
      free(node->assign.name);
      ast_free(node->assign.value);
      break;

    case AST_BLOCK:
      DYN_ARR_ITER(node->block.expressions, ASTNode, element, {
        ast_free(element);
      })
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
      free(node->function.name->value);
      free(node->function.name);

      DYN_ARR_ITER(node->function.params, ASTIdentifier, param, {
        free(param->value);
        free(param);
      })
      dyn_array_free(node->function.params);
      ast_free(node->function.body);
      break;

    case AST_CALL:
      free(node->call.name->value);
      free(node->call.name);

      DYN_ARR_ITER(node->call.args, ASTNode, element, {
        ast_free(element);
      })
      dyn_array_free(node->call.args);
      break;

    case AST_STRING:
      free(node->string.value);
      break;

    case AST_USE:
      free(node->use.value);
      break;

    case AST_WHILE:
      ast_free(node->while_.condition);
      ast_free(node->while_.body);
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

ASTNode* ast_use_init(const char* value) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_USE;
  node->use.value = strdup(value);
  return node;
}

ASTNode* ast_bool_init(const char* value) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_BOOL;


  if (strcmp(value, "true") == 0) {
    node->bool_.value = true;
  } else if (strcmp(value, "false") == 0) {
    node->bool_.value = false;
  } else {
    panic("invalue bool: %s", value);
  }

  return node;
}

ASTNode* ast_null_init() {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_NULL;

  return node;
}

ASTNode* ast_while_init(ASTNode* condition, ASTNode* body) {
  ASTNode* node = malloc(sizeof(ASTNode));
  check_memory_is_not_null(node);

  node->type = AST_WHILE;
  node->while_.condition = condition;
  node->while_.body = body;

  return node;
}
