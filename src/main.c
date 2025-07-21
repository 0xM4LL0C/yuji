
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

  for (size_t i = 0; i < ast->size; i++) {
    interpreter_eval(interpreter, dyn_array_get(ast, i));
  }

  interpreter_free(interpreter);
  parser_free(parser);
  lexer_free(lexer);

  for (size_t i = 0; i < lines->size; i++) {
    free(dyn_array_get(lines, i));
  }

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
