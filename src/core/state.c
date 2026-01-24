#include "yuji/core/state.h"
#include "yuji/core/ast.h"
#include "yuji/core/interpreter.h"
#include "yuji/core/lexer.h"
#include "yuji/core/memory.h"
#include "yuji/core/parser.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
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
  char* buffer = strdup(str);

  char* token = strtok(buffer, "\n");

  while (token) {
    yuji_dyn_array_push(result, strdup(token));
    token = strtok(NULL, "\n");
  }

  yuji_free(buffer);
  return result;
}

YujiASTNode* yuji_get_ast(const char* string, const char* source_name) {
  // LEXER
  YujiDynArray* input = get_input(string);
  YujiLexer* lexer = yuji_lexer_init(input, source_name);
  YujiDynArray* tokens = yuji_dyn_array_init();
  yuji_lexer_tokenize(lexer, tokens);

  // PARSER
  YujiParser* parser = yuji_parser_init(tokens);
  YujiASTNode* ast = yuji_parser_parse(source_name, parser);

  // CLEANUP
  yuji_parser_free(parser);

  YUJI_DYN_ARRAY_ITER(tokens, YujiToken, token, {
    yuji_token_free(token);
  })
  yuji_dyn_array_free(tokens);

  YUJI_DYN_ARRAY_ITER(input, char*, line, {
    yuji_free(line);
  })
  yuji_dyn_array_free(input);
  yuji_lexer_free(lexer);

  return ast;
}

YujiASTNode* yuji_get_ast_from_file(const char* filename) {
  FILE* file = fopen(filename, "r");

  if (!file) {
    yuji_panic("error opening file '%s': %s", filename, strerror(errno));
  }

  char buffer[64];
  YujiString* str = yuji_string_init();

  while (fgets(buffer, sizeof(buffer), file)) {
    yuji_string_append(str, buffer, strlen(buffer));
  }

  fclose(file);
  YujiASTNode* ast = yuji_get_ast(str->data, filename);
  yuji_string_free(str);

  return ast;
}

void yuji_eval_string(YujiState* state, const char* string) {
  yuji_check_memory(state);
  yuji_check_memory((void*)string);

  YujiDynArray* input = get_input(string);
  YujiASTNode* ast = yuji_get_ast(string, "<string>");
  YujiValue* result = yuji_interpreter_eval(state->interpreter, ast);

  yuji_value_free(result);

  yuji_ast_free(ast);

  YUJI_DYN_ARRAY_ITER(input, char*, line, {
    yuji_free(line);
  })
  yuji_dyn_array_free(input);
}

void yuji_eval_file(YujiState* state, const char* filename) {
  yuji_check_memory(state);
  yuji_check_memory((void*)filename);

  YujiASTNode* ast = yuji_get_ast_from_file(filename);
  YujiValue * result = yuji_interpreter_eval(state->interpreter, ast);

  yuji_value_free(result);

  yuji_ast_free(ast);
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
