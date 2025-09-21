#pragma once

#include "yuji/api.h"
#include <stddef.h>

NO_RETURN void yuji_panic(const char* fmt, ...);
void yuji_check_memory(void* ptr);

__attribute__((malloc)) void* yuji_malloc(size_t size);
void* yuji_realloc(void* ptr, size_t size);
void yuji_free(void* ptr);
