#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include <string.h>
#include <stdlib.h>


void yuji_ast_free(YujiASTNode* node) {
  yuji_check_memory(node);

  switch (node->type) {
    case YUJI_AST_INT:
      yuji_free(node->value.int_);
      break;

    case YUJI_AST_FLOAT:
      yuji_free(node->value.float_);
      break;

    case YUJI_AST_STRING:
      yuji_string_free(node->value.string->value);
      yuji_free(node->value.string);
      break;

    case YUJI_AST_BIN_OP:
      yuji_ast_free(node->value.bin_op->left);
      yuji_ast_free(node->value.bin_op->right);
      yuji_free(node->value.bin_op->operator);
      yuji_free(node->value.bin_op);
      break;

    case YUJI_AST_IDENTIFIER:
      yuji_free(node->value.identifier->value);
      yuji_free(node->value.identifier);
      break;

    case YUJI_AST_ASSIGN:
      yuji_free(node->value.assign->name);
      yuji_ast_free(node->value.assign->value);
      yuji_free(node->value.assign);
      break;

    case YUJI_AST_LET:
      yuji_free(node->value.let->name);
      yuji_ast_free(node->value.let->value);
      yuji_free(node->value.let);
      break;

    case YUJI_AST_BLOCK:
      if (node->value.block) {
        YUJI_DYN_ARRAY_ITER(node->value.block->exprs, YujiASTNode, expr, {
          yuji_ast_free(expr);
        })
        yuji_dyn_array_free(node->value.block->exprs);
        yuji_free(node->value.block);
      }

      break;

    case YUJI_AST_FN:
      yuji_free(node->value.fn->name);
      YUJI_DYN_ARRAY_ITER(node->value.fn->params, char*, param, {
        yuji_free(param);
      })
      yuji_dyn_array_free(node->value.fn->params);
      yuji_ast_free(node->value.fn->body);
      yuji_free(node->value.fn);
      break;

    case YUJI_AST_CALL:
      yuji_free(node->value.call->name);
      YUJI_DYN_ARRAY_ITER(node->value.call->args, YujiASTNode, arg, {
        yuji_ast_free(arg);
      })
      yuji_dyn_array_free(node->value.call->args);
      yuji_free(node->value.call);
      break;

    case YUJI_AST_USE:
      yuji_free(node->value.use->value);
      yuji_free(node->value.use);
      break;

    case YUJI_AST_BOOL:
      yuji_free(node->value.boolean);
      break;

    case YUJI_AST_WHILE:
      yuji_ast_free(node->value.while_stmt->condition);

      YUJI_DYN_ARRAY_ITER(node->value.while_stmt->body->exprs, YujiASTNode, expr, {
        yuji_ast_free(expr);
      })

      yuji_dyn_array_free(node->value.while_stmt->body->exprs);
      yuji_free(node->value.while_stmt->body);
      yuji_free(node->value.while_stmt);
      break;

    case YUJI_AST_IF:
      YUJI_DYN_ARRAY_ITER(node->value.if_stmt->branches, YujiASTIfBranch, branch, {
        yuji_ast_free(branch->condition);

        YUJI_DYN_ARRAY_ITER(branch->body->exprs, YujiASTNode, expr, {
          yuji_ast_free(expr);
        })
        yuji_dyn_array_free(branch->body->exprs);
        yuji_free(branch->body);
        yuji_free(branch);
      })
      yuji_dyn_array_free(node->value.if_stmt->branches);

      if (node->value.if_stmt->else_body) {
        YUJI_DYN_ARRAY_ITER(node->value.if_stmt->else_body->exprs, YujiASTNode, expr, {
          yuji_ast_free(expr);
        })
        yuji_dyn_array_free(node->value.if_stmt->else_body->exprs);
        yuji_free(node->value.if_stmt->else_body);
      }

      yuji_free(node->value.if_stmt);
      break;

    case YUJI_AST_NULL:
      break;

    case YUJI_AST_RETURN:
      yuji_ast_free(node->value.return_stmt->value);
      yuji_free(node->value.return_stmt);
      break;
  }

  yuji_free(node);
}

char* yuji_ast_node_type_to_string(YujiASTNodeType type) {
#define _YUJI_AST_NODE_TYPE_CASE(type) case type: return #type

  switch (type) {
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_INT);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_FLOAT);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_STRING);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_BIN_OP);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_IDENTIFIER);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_ASSIGN);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_IF);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_NULL);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_LET);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_BLOCK);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_FN);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_CALL);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_USE);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_BOOL);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_WHILE);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_RETURN);

    default:
      yuji_panic("Unknown node type: %d", type);
  }

#undef _YUJI_AST_NODE_TYPE_CASE
}

YUJI_AST_INIT(int, YUJI_AST_INT, {
  node->value.int_ = yuji_malloc(sizeof(YujiASTInt));
  node->value.int_->value = value;
}, int64_t value)

YUJI_AST_INIT(float, YUJI_AST_FLOAT, {
  node->value.float_ = yuji_malloc(sizeof(YujiASTFloat));
  node->value.float_->value = value;
}, double value)

YUJI_AST_INIT(string, YUJI_AST_STRING, {
  node->value.string = yuji_malloc(sizeof(YujiASTString));
  node->value.string->value = yuji_string_init_from_cstr(value);
}, const char* value)

YUJI_AST_INIT(bin_op, YUJI_AST_BIN_OP, {
  node->value.bin_op = yuji_malloc(sizeof(YujiASTBinOp));
  node->value.bin_op->left = left;
  node->value.bin_op->right = right;
  node->value.bin_op->operator = strdup(operator);
}, YujiASTNode* left, const char* operator, YujiASTNode* right)

YUJI_AST_INIT(identifier, YUJI_AST_IDENTIFIER, {
  node->value.identifier = yuji_malloc(sizeof(YujiASTIdentifier));
  node->value.identifier->value = strdup(name);
}, const char* name)

YUJI_AST_INIT(assign, YUJI_AST_ASSIGN, {
  node->value.assign = yuji_malloc(sizeof(YujiASTAssign));
  node->value.assign->name = strdup(name);
  node->value.assign->value = value;
}, const char* name, YujiASTNode* value)

YUJI_AST_INIT(let, YUJI_AST_LET, {
  node->value.let = yuji_malloc(sizeof(YujiASTLet));
  node->value.let->name = strdup(name);
  node->value.let->value = value;
}, const char* name, YujiASTNode* value)

YUJI_AST_INIT(block, YUJI_AST_BLOCK, {
  node->value.block = yuji_malloc(sizeof(YujiASTBlock));
  node->value.block->exprs = yuji_dyn_array_init();
  YUJI_DYN_ARRAY_ITER(exprs, YujiASTNode, expr, {
    yuji_dyn_array_push(node->value.block->exprs, expr);
  });
}, YujiDynArray* exprs)

YUJI_AST_INIT(fn, YUJI_AST_FN, {
  node->value.fn = yuji_malloc(sizeof(YujiASTFunction));
  node->value.fn->name = strdup(name);
  node->value.fn->params = yuji_dyn_array_init();

  YUJI_DYN_ARRAY_ITER(params, void*, param, {
    char* param_str = (char*)param;
    char* param_copy = strdup(param_str);
    yuji_dyn_array_push(node->value.fn->params, param_copy);
  });

  YujiDynArray* fn_exprs = yuji_dyn_array_init();

  YUJI_DYN_ARRAY_ITER(body->exprs, YujiASTNode, expr, {
    YujiASTNode* expr_copy = yuji_ast_node_copy(expr);
    yuji_dyn_array_push(fn_exprs, expr_copy);
  });

  node->value.fn->body = yuji_ast_block_init(fn_exprs);
  yuji_dyn_array_free(fn_exprs);
}, const char* name, YujiDynArray* params, YujiASTBlock* body)

YUJI_AST_INIT(call, YUJI_AST_CALL, {
  node->value.call = yuji_malloc(sizeof(YujiASTCall));
  node->value.call->name = strdup(name);
  node->value.call->args = args;
}, const char* name, YujiDynArray* args)

YUJI_AST_INIT(use, YUJI_AST_USE, {
  node->value.use = yuji_malloc(sizeof(YujiASTUse));
  node->value.use->value = strdup(value);
}, const char* value)

YUJI_AST_INIT(bool, YUJI_AST_BOOL, {
  node->value.boolean = yuji_malloc(sizeof(YujiASTBool));
  node->value.boolean->value = value;
}, bool value)

YujiASTNode* yuji_ast_node_copy(YujiASTNode* node) {
  yuji_check_memory(node);

  YujiASTNode* copy = yuji_malloc(sizeof(YujiASTNode));
  copy->type = node->type;

  switch (node->type) {
    case YUJI_AST_INT:
      copy->value.int_ = yuji_malloc(sizeof(YujiASTInt));
      copy->value.int_->value = node->value.int_->value;
      break;

    case YUJI_AST_FLOAT:
      copy->value.float_ = yuji_malloc(sizeof(YujiASTFloat));
      copy->value.float_->value = node->value.float_->value;
      break;

    case YUJI_AST_STRING:
      copy->value.string = yuji_malloc(sizeof(YujiASTString));
      copy->value.string->value = yuji_string_init_from_cstr(node->value.string->value->data);
      break;

    case YUJI_AST_BIN_OP:
      copy->value.bin_op = yuji_malloc(sizeof(YujiASTBinOp));
      copy->value.bin_op->left = yuji_ast_node_copy(node->value.bin_op->left);
      copy->value.bin_op->right = yuji_ast_node_copy(node->value.bin_op->right);
      copy->value.bin_op->operator = strdup(node->value.bin_op->operator);
      break;

    case YUJI_AST_IDENTIFIER:
      copy->value.identifier = yuji_malloc(sizeof(YujiASTIdentifier));
      copy->value.identifier->value = strdup(node->value.identifier->value);
      break;

    case YUJI_AST_ASSIGN:
      copy->value.assign = yuji_malloc(sizeof(YujiASTAssign));
      copy->value.assign->name = strdup(node->value.assign->name);
      copy->value.assign->value = yuji_ast_node_copy(node->value.assign->value);
      break;

    case YUJI_AST_LET:
      copy->value.let = yuji_malloc(sizeof(YujiASTLet));
      copy->value.let->name = strdup(node->value.let->name);
      copy->value.let->value = yuji_ast_node_copy(node->value.let->value);
      break;

    case YUJI_AST_BLOCK:
      copy->value.block = yuji_malloc(sizeof(YujiASTBlock));
      copy->value.block->exprs = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.block->exprs, YujiASTNode, expr, {
        YujiASTNode* expr_copy = yuji_ast_node_copy(expr);
        yuji_dyn_array_push(copy->value.block->exprs, expr_copy);
      });
      break;

    case YUJI_AST_FN:
      copy->value.fn = yuji_malloc(sizeof(YujiASTFunction));
      copy->value.fn->name = strdup(node->value.fn->name);
      copy->value.fn->params = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.fn->params, void*, param, {
        char* param_copy = strdup((char*)param);
        yuji_dyn_array_push(copy->value.fn->params, param_copy);
      });
      copy->value.fn->body = yuji_ast_node_copy(node->value.fn->body);
      break;

    case YUJI_AST_CALL:
      copy->value.call = yuji_malloc(sizeof(YujiASTCall));
      copy->value.call->name = strdup(node->value.call->name);
      copy->value.call->args = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.call->args, YujiASTNode, arg, {
        YujiASTNode* arg_copy = yuji_ast_node_copy(arg);
        yuji_dyn_array_push(copy->value.call->args, arg_copy);
      });
      break;

    case YUJI_AST_USE:
      copy->value.use = yuji_malloc(sizeof(YujiASTUse));
      copy->value.use->value = strdup(node->value.use->value);
      break;

    case YUJI_AST_BOOL:
      copy->value.boolean = yuji_malloc(sizeof(YujiASTBool));
      copy->value.boolean->value = node->value.boolean->value;
      break;

    case YUJI_AST_WHILE:
      copy->value.while_stmt = yuji_malloc(sizeof(YujiASTWhile));
      copy->value.while_stmt->condition = yuji_ast_node_copy(node->value.while_stmt->condition);
      copy->value.while_stmt->body = yuji_malloc(sizeof(YujiASTBlock));
      copy->value.while_stmt->body->exprs = yuji_dyn_array_init();

      if (node->value.while_stmt->body && node->value.while_stmt->body->exprs) {
        YUJI_DYN_ARRAY_ITER(node->value.while_stmt->body->exprs, YujiASTNode, expr, {
          YujiASTNode* expr_copy = yuji_ast_node_copy(expr);
          yuji_dyn_array_push(copy->value.while_stmt->body->exprs, expr_copy);
        });
      }

      break;

    case YUJI_AST_IF:
      copy->value.if_stmt = yuji_malloc(sizeof(YujiASTIf));
      copy->value.if_stmt->branches = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.if_stmt->branches, YujiASTIfBranch, branch, {
        YujiASTIfBranch* branch_copy = yuji_malloc(sizeof(YujiASTIfBranch));
        branch_copy->condition = yuji_ast_node_copy(branch->condition);
        branch_copy->body = yuji_malloc(sizeof(YujiASTBlock));
        branch_copy->body->exprs = yuji_dyn_array_init();

        YUJI_DYN_ARRAY_ITER(branch->body->exprs, YujiASTNode, expr, {
          YujiASTNode* expr_copy = yuji_ast_node_copy(expr);
          yuji_dyn_array_push(branch_copy->body->exprs, expr_copy);
        });
        yuji_dyn_array_push(copy->value.if_stmt->branches, branch_copy);
      });

      if (node->value.if_stmt->else_body) {
        copy->value.if_stmt->else_body = yuji_malloc(sizeof(YujiASTBlock));
        copy->value.if_stmt->else_body->exprs = yuji_dyn_array_init();

        if (node->value.if_stmt->else_body->exprs) {
          YUJI_DYN_ARRAY_ITER(node->value.if_stmt->else_body->exprs, YujiASTNode, expr, {
            if (expr) {
              YujiASTNode* expr_copy = yuji_ast_node_copy(expr);
              yuji_dyn_array_push(copy->value.if_stmt->else_body->exprs, expr_copy);
            }
          });
        }
      } else {
        copy->value.if_stmt->else_body = NULL;
      }

      break;

    case YUJI_AST_NULL:
      copy->value.null = node->value.null;
      break;

    case YUJI_AST_RETURN:
      copy->value.return_stmt = yuji_malloc(sizeof(YujiASTReturn));
      copy->value.return_stmt->value = yuji_ast_node_copy(node->value.return_stmt->value);
      break;
  }

  return copy;
}

YUJI_AST_INIT(while, YUJI_AST_WHILE, {
  node->value.while_stmt = yuji_malloc(sizeof(YujiASTWhile));
  node->value.while_stmt->condition = condition;
  node->value.while_stmt->body = body;
}, YujiASTNode* condition, YujiASTBlock* body)

YujiASTBlock* yuji_ast_extract_block(YujiASTNode* block_node) {
  if (!block_node || block_node->type != YUJI_AST_BLOCK) {
    return NULL;
  }

  YujiASTBlock* block = block_node->value.block;
  block_node->value.block = NULL;
  yuji_ast_free(block_node);
  return block;
}

YujiASTIfBranch* yuji_ast_if_branch_init(YujiASTNode* condition, YujiASTBlock* body) {
  YujiASTIfBranch* branch = yuji_malloc(sizeof(YujiASTIfBranch));
  branch->condition = condition;
  branch->body = body;
  return branch;
}

YUJI_AST_INIT(if, YUJI_AST_IF, {
  node->value.if_stmt = yuji_malloc(sizeof(YujiASTIf));
  node->value.if_stmt->branches = branches;
  node->value.if_stmt->else_body = else_body;
}, YujiDynArray* branches, YujiASTBlock* else_body)

YUJI_AST_INIT(null, YUJI_AST_NULL, {
  static YujiASTNull* global_null = NULL;

  if (!global_null) {
    global_null = yuji_malloc(sizeof(YujiASTNull));
  }
  node->value.null = global_null;
}, void)

YUJI_AST_INIT(return, YUJI_AST_RETURN, {
  node->value.return_stmt = yuji_malloc(sizeof(YujiASTReturn));
  node->value.return_stmt->value = value;
}, YujiASTNode* value)
