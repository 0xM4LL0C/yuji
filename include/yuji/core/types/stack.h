#pragma once

#include "yuji/core/types/dyn_array.h"

typedef struct {
  YujiDynArray* data;
} YujiStack;

YujiStack* yuji_stack_init();
void yuji_stack_free(YujiStack* stack);

void yuji_stack_push(YujiStack* stack, void* value);
void* yuji_stack_pop(YujiStack* stack);
void* yuji_stack_peek(YujiStack* stack);
