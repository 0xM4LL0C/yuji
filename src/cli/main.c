#include "yuji/core/ast.h"
#include "yuji/core/interpreter.h"
#include "yuji/core/lexer.h"
#include "yuji/core/memory.h"
#include "yuji/core/parser.h"
#include "yuji/core/token.h"
#include "yuji/core/types/dyn_array.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int run_file(const char* filename) {
  FILE *file = fopen(filename, "r");

  if (!file) {
    yuji_panic("error opening file '%s': %s", filename, strerror(errno));
  }

  char buffer[64];
  YujiDynArray* input = yuji_dyn_array_init();

  while (fgets(buffer, sizeof(buffer), file)) {
    yuji_dyn_array_push(input, strdup(buffer));
    printf("%s", buffer);
  }

  fclose(file);

  // LEXER
  YujiLexer *lexer = yuji_lexer_init(input, filename);
  YujiDynArray* tokens = yuji_dyn_array_init();

  yuji_lexer_tokenize(lexer, tokens);

  YUJI_DYN_ARRAY_ITER(tokens, YujiToken, token, {
    const char* token_as_string = yuji_token_to_string(token);
    printf("%s\n", token_as_string);
    yuji_free((void*)token_as_string);
  })

  // PARSER
  YujiParser *parser = yuji_parser_init(tokens);

  YujiDynArray* ast = yuji_parser_parse(parser);

  YUJI_DYN_ARRAY_ITER(ast, YujiASTNode, node, {
    printf("%d\n", node->type);
  })

  // INTERPRETER
  YujiInterpreter* interpreter = yuji_interpreter_init();

  YUJI_DYN_ARRAY_ITER(ast, YujiASTNode, node, {
    yuji_interpreter_eval(interpreter, node);
  })

  // CLEANUP
  yuji_interpreter_free(interpreter);

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

  YUJI_DYN_ARRAY_ITER(input, char, str, {
    yuji_free(str);
  })
  yuji_dyn_array_free(input);

  return 0;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  return run_file(argv[1]);
}
