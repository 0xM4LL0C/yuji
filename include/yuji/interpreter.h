#pragma once

#include "yuji/map.h"
typedef struct {
  Map* variables;
} Interpreter;

Interpreter* interpreter_init();
void interpreter_free(Interpreter* interpreter);
