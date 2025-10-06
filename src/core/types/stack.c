#include "yuji/core/types/stack.h"
#include "yuji/core/memory.h"
#include "yuji/core/types/dyn_array.h"

YujiStack* yuji_stack_init() {
  YujiStack* stack = yuji_malloc(sizeof(YujiStack));

  stack->data = yuji_dyn_array_init();

  return stack;
}

void yuji_stack_free(YujiStack* stack) {
  yuji_dyn_array_free(stack->data);
  yuji_free(stack);
}

void yuji_stack_push(YujiStack* stack, void* value) {
  yuji_dyn_array_push(stack->data, value);
}

void* yuji_stack_pop(YujiStack* stack) {
  return yuji_dyn_array_pop(stack->data);
}

void* yuji_stack_peek(YujiStack* stack) {
  return yuji_dyn_array_get(stack->data, stack->data->size - 1);
}
