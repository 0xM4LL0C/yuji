#include "yuji/core/ast.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include <string.h>
#include <stdlib.h>


void yuji_ast_free(YujiASTNode* node) {
  yuji_check_memory(node);

  switch (node->type) {
    case YUJI_AST_NULL:
    case YUJI_AST_BREAK:
    case YUJI_AST_CONTINUE:
      break;

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
      if (node->value.fn->name) { // for anon funcs
        yuji_free(node->value.fn->name);
      }

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

    case YUJI_AST_RETURN:
      yuji_ast_free(node->value.return_stmt->value);
      yuji_free(node->value.return_stmt);
      break;

    case YUJI_AST_ARRAY:
      YUJI_DYN_ARRAY_ITER(node->value.array->elements, YujiASTNode, element, {
        yuji_ast_free(element);
      });
      yuji_dyn_array_free(node->value.array->elements);
      yuji_free(node->value.array);
      break;

    case YUJI_AST_INDEX_ACCESS:
      yuji_ast_free(node->value.index_access->object);
      yuji_ast_free(node->value.index_access->index);
      yuji_free(node->value.index_access);
      break;

    case YUJI_AST_INDEX_ASSIGN:
      yuji_ast_free(node->value.index_assign->object);
      yuji_ast_free(node->value.index_assign->index);
      yuji_ast_free(node->value.index_assign->value);
      yuji_free(node->value.index_assign);
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
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_BREAK);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_CONTINUE);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_ARRAY);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_INDEX_ACCESS);
      _YUJI_AST_NODE_TYPE_CASE(YUJI_AST_INDEX_ASSIGN);
  }

  yuji_panic("Unknown node type: %d", type);

#undef _YUJI_AST_NODE_TYPE_CASE
}

YujiASTNode* yuji_ast_node_copy(YujiASTNode* node) {
  yuji_check_memory(node);

  switch (node->type) {
    case YUJI_AST_NULL:
      return yuji_ast_null_init();

    case YUJI_AST_BREAK:
      return yuji_ast_break_init();

    case YUJI_AST_CONTINUE:
      return yuji_ast_continue_init();

    case YUJI_AST_INT:
      return yuji_ast_int_init(node->value.int_->value);

    case YUJI_AST_FLOAT:
      return yuji_ast_float_init(node->value.float_->value);

    case YUJI_AST_STRING:
      return yuji_ast_string_init(node->value.string->value->data);

    case YUJI_AST_BOOL:
      return yuji_ast_bool_init(node->value.boolean->value);

    case YUJI_AST_IDENTIFIER:
      return yuji_ast_identifier_init(node->value.identifier->value);

    case YUJI_AST_USE:
      return yuji_ast_use_init(node->value.use->value);

    case YUJI_AST_BIN_OP:
      return yuji_ast_bin_op_init(
               yuji_ast_node_copy(node->value.bin_op->left),
               node->value.bin_op->operator,
               yuji_ast_node_copy(node->value.bin_op->right)
             );

    case YUJI_AST_ASSIGN:
      return yuji_ast_assign_init(
               node->value.assign->name,
               yuji_ast_node_copy(node->value.assign->value)
             );

    case YUJI_AST_LET:
      return yuji_ast_let_init(
               node->value.let->name,
               yuji_ast_node_copy(node->value.let->value)
             );

    case YUJI_AST_BLOCK: {
      if (!node->value.block) {
        return yuji_ast_block_init(NULL);
      }

      YujiDynArray* exprs_copy = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.block->exprs, YujiASTNode, expr, {
        yuji_dyn_array_push(exprs_copy, yuji_ast_node_copy(expr));
      });
      YujiASTNode* copy = yuji_ast_block_init(exprs_copy);
      yuji_dyn_array_free(exprs_copy);
      return copy;
    }

    case YUJI_AST_FN: {
      YujiDynArray* params_temp = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.fn->params, char*, param, {
        yuji_dyn_array_push(params_temp, param);
      });
      YujiASTBlock* body_temp = yuji_malloc(sizeof(YujiASTBlock));
      body_temp->exprs = node->value.fn->body->value.block->exprs;
      YujiASTNode* copy = yuji_ast_fn_init(node->value.fn->name, params_temp, body_temp);
      yuji_dyn_array_free(params_temp);
      yuji_free(body_temp);
      return copy;
    }

    case YUJI_AST_CALL: {
      YujiDynArray* args_copy = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.call->args, YujiASTNode, arg, {
        yuji_dyn_array_push(args_copy, yuji_ast_node_copy(arg));
      });
      return yuji_ast_call_init(node->value.call->name, args_copy);
    }

    case YUJI_AST_WHILE: {
      YujiASTNode* cond_copy = yuji_ast_node_copy(node->value.while_stmt->condition);
      YujiASTBlock* body_copy = yuji_malloc(sizeof(YujiASTBlock));
      body_copy->exprs = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.while_stmt->body->exprs, YujiASTNode, expr, {
        yuji_dyn_array_push(body_copy->exprs, yuji_ast_node_copy(expr));
      });
      return yuji_ast_while_init(cond_copy, body_copy);
    }

    case YUJI_AST_IF: {
      YujiDynArray* branches_copy = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.if_stmt->branches, YujiASTIfBranch, branch, {
        YujiASTNode* cond_copy = yuji_ast_node_copy(branch->condition);
        YujiASTBlock* body_copy = yuji_malloc(sizeof(YujiASTBlock));
        body_copy->exprs = yuji_dyn_array_init();
        YUJI_DYN_ARRAY_ITER(branch->body->exprs, YujiASTNode, expr, {
          yuji_dyn_array_push(body_copy->exprs, yuji_ast_node_copy(expr));
        });
        YujiASTIfBranch* branch_copy = yuji_ast_if_branch_init(cond_copy, body_copy);
        yuji_dyn_array_push(branches_copy, branch_copy);
      });
      YujiASTBlock* else_copy = NULL;

      if (node->value.if_stmt->else_body) {
        else_copy = yuji_malloc(sizeof(YujiASTBlock));
        else_copy->exprs = yuji_dyn_array_init();
        YUJI_DYN_ARRAY_ITER(node->value.if_stmt->else_body->exprs, YujiASTNode, expr, {
          yuji_dyn_array_push(else_copy->exprs, yuji_ast_node_copy(expr));
        });
      }

      return yuji_ast_if_init(branches_copy, else_copy);
    }

    case YUJI_AST_RETURN:
      return yuji_ast_return_init(yuji_ast_node_copy(node->value.return_stmt->value));

    case YUJI_AST_ARRAY: {
      YujiDynArray* elements_copy = yuji_dyn_array_init();
      YUJI_DYN_ARRAY_ITER(node->value.array->elements, YujiASTNode, element, {
        yuji_dyn_array_push(elements_copy, yuji_ast_node_copy(element));
      });
      return yuji_ast_array_init(elements_copy);
    }

    case YUJI_AST_INDEX_ACCESS:
      return yuji_ast_index_access_init(
               yuji_ast_node_copy(node->value.index_access->object),
               yuji_ast_node_copy(node->value.index_access->index)
             );

    case YUJI_AST_INDEX_ASSIGN:
      return yuji_ast_index_assign_init(
               yuji_ast_node_copy(node->value.index_assign->object),
               yuji_ast_node_copy(node->value.index_assign->index),
               yuji_ast_node_copy(node->value.index_assign->value)
             );
  }

  yuji_panic("Unknown node type: %d", node->type);
}

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

  if (name) {
    node->value.fn->name = strdup(name);
  } else {
    node->value.fn->name = NULL; // for anon funcs
  }

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

YUJI_AST_INIT(while, YUJI_AST_WHILE, {
  node->value.while_stmt = yuji_malloc(sizeof(YujiASTWhile));
  node->value.while_stmt->condition = condition;
  node->value.while_stmt->body = body;
}, YujiASTNode* condition, YujiASTBlock* body)

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

YUJI_AST_INIT(break, YUJI_AST_BREAK, {
  static YujiASTBreak* global_break = NULL;

  if (!global_break) {
    global_break = yuji_malloc(sizeof(YujiASTBreak));
  }
  node->value.break_stmt = global_break;
}, void)

YUJI_AST_INIT(continue, YUJI_AST_CONTINUE, {
  static YujiASTContinue* global_continue = NULL;

  if (!global_continue) {
    global_continue = yuji_malloc(sizeof(YujiASTContinue));
  }
  node->value.continue_stmt = global_continue;
}, void)

YUJI_AST_INIT(array, YUJI_AST_ARRAY, {
  node->value.array = yuji_malloc(sizeof(YujiASTArray));
  node->value.array->elements = elements;
}, YujiDynArray* elements)

YUJI_AST_INIT(index_access, YUJI_AST_INDEX_ACCESS, {
  node->value.index_access = yuji_malloc(sizeof(YujiASTIndexAccess));
  node->value.index_access->object = object;
  node->value.index_access->index = index;
}, YujiASTNode* object, YujiASTNode* index)

YUJI_AST_INIT(index_assign, YUJI_AST_INDEX_ASSIGN, {
  node->value.index_assign = yuji_malloc(sizeof(YujiASTIndexAssign));
  node->value.index_assign->object = object;
  node->value.index_assign->index = index;
  node->value.index_assign->value = value;
}, YujiASTNode* object, YujiASTNode* index, YujiASTNode* value)
