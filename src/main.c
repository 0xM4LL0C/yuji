
#include "yuji/interpreter.h"
#include "yuji/lexer.h"
#include "yuji/parser.h"
#include "yuji/types/dyn_array.h"
#include "yuji/utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int run_file(const char* file_name) {
  FILE* file = fopen(file_name, "r");

  if (!file) {
    strerror(errno);
    panic("can't read file '%s': %s", file_name, strerror(errno));
  }

  DynArr* lines = dyn_array_init();

  char* line = NULL;
  size_t line_len = 0;

  while (getline(&line, &line_len, file) != -1) {
    dyn_array_append(lines, strdup(line));
  }

  if (line) {
    free(line);
  }

  fclose(file);

  Lexer* lexer = lexer_init(lines, file_name);
  DynArr* tokens = lexer_tokenize(lexer);


  Parser* parser = parser_init(tokens);
  DynArr* ast = parser_parse(parser);

  Interpreter* interpreter = interpreter_init();

  DYN_ARR_ITER(ast, ASTNode, element, {
    interpreter_eval(interpreter, element);
  })


  interpreter_free(interpreter);
  parser_free(parser);
  lexer_free(lexer);

  DYN_ARR_ITER(lines, char*, element, {
    free(element);
  })

  dyn_array_free(lines);
  return 0;
}

int main(int argc, char** argv) {
  if (argc > 1) {
    return run_file(argv[1]);
  }

  fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
  return 1;
}
