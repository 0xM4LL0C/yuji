#pragma once

#include "yuji/core/interpreter.h"

typedef struct {
  YujiInterpreter* interpreter;
} YujiRunner;

YujiRunner* yuji_runner_init();
void yuji_runner_free(YujiRunner* runner);

void yuji_runner_run_string(YujiRunner* runner, const char* string);
void yuji_runner_run_file(YujiRunner* runner, const char* filename);

void yuji_print_call_stack(YujiRunner* runner);
