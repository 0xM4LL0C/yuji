#include "yuji/core/state.h"
#include "yuji/core/interpreter.h"
#include "yuji/core/lexer.h"
#include "yuji/core/memory.h"
#include "yuji/core/parser.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/value.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

YujiState* yuji_state_init() {
  YujiState* state = yuji_malloc(sizeof(YujiState));

  state->interpreter = yuji_interpreter_init();

  return state;
}

void yuji_state_free(YujiState *state) {
  yuji_check_memory(state);

  yuji_interpreter_free(state->interpreter);
  yuji_free(state);
}

static YujiDynArray* get_input(const char* str) {
  YujiDynArray* result = yuji_dyn_array_init();
  yuji_dyn_array_push(result, strdup(str));
  return result;
}

static YujiValue* run_input(YujiState* state, YujiDynArray* input, const char* source_name) {
  // LEXER
  YujiLexer* lexer = yuji_lexer_init(input, source_name);
  YujiDynArray* tokens = yuji_dyn_array_init();
  yuji_lexer_tokenize(lexer, tokens);

  // PARSER
  YujiParser* parser = yuji_parser_init(tokens);
  YujiDynArray* ast = yuji_parser_parse(parser);

  YujiValue* last_result = NULL;

  // INTERPRETER
  YUJI_DYN_ARRAY_ITER(ast, YujiASTNode, node, {
    if (last_result) {
      yuji_value_free(last_result);
    }
    last_result = yuji_interpreter_eval(state->interpreter, node);
  })

  // CLEANUP
  YUJI_DYN_ARRAY_ITER(ast, YujiASTNode, node, {
    yuji_ast_free(node);
  })
  yuji_dyn_array_free(ast);

  yuji_parser_free(parser);

  YUJI_DYN_ARRAY_ITER(tokens, YujiToken, token, {
    yuji_token_free(token);
  })
  yuji_dyn_array_free(tokens);
  yuji_lexer_free(lexer);

  return last_result;
}

void yuji_eval_string(YujiState* state, const char* string) {
  yuji_check_memory(state);
  yuji_check_memory((void*)string);

  YujiDynArray* input = get_input(string);
  YujiValue* result = run_input(state, input, "<string>");

  if (result) {
    yuji_value_free(result);
  }

  YUJI_DYN_ARRAY_ITER(input, char, str, {
    yuji_free(str);
  })
  yuji_dyn_array_free(input);
}

void yuji_eval_file(YujiState* state, const char* filename) {
  yuji_check_memory(state);
  yuji_check_memory((void*)filename);

  FILE* file = fopen(filename, "r");

  if (!file) {
    yuji_panic("error opening file '%s': %s", filename, strerror(errno));
  }

  char buffer[64];
  YujiDynArray* input = yuji_dyn_array_init();

  while (fgets(buffer, sizeof(buffer), file)) {
    yuji_dyn_array_push(input, strdup(buffer));
  }

  fclose(file);

  YujiValue* result = run_input(state, input, filename);

  if (result) {
    yuji_value_free(result);
  }

  YUJI_DYN_ARRAY_ITER(input, char, str, {
    yuji_free(str);
  })
  yuji_dyn_array_free(input);
}

void yuji_print_call_stack(YujiState* state) {
  if (!state->interpreter->call_stack->data->size) {
    return;
  }

  printf("Call stack traceback:\n");

  size_t i = 0;
  YUJI_DYN_ARRAY_ITER(state->interpreter->call_stack->data, YujiCallFrame, frame, {
    printf("  #%zu: in function '%s'\n", i, frame->function_name);
    i++;
  })
}
