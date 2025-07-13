#include "yuji/interpreter.h"
#include "yuji/map.h"
#include "yuji/utils.h"
#include <stdlib.h>

Interpreter* interpreter_init() {
  Interpreter* interpreter = malloc(sizeof(Interpreter));

  check_memory_is_not_null(interpreter);

  interpreter->variables = map_init();

  return interpreter;
}

void interpreter_free(Interpreter* interpreter) {
  map_free(interpreter->variables);
  free(interpreter);
}
