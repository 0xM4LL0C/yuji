#pragma once

#include "yuji/api.h"
#include <stddef.h>

NO_RETURN __attribute__((format(printf, 1, 2))) void yuji_panic(const char* fmt, ...);
void yuji_check_memory(void* ptr);

void yuji_free(void* ptr);
__attribute__((malloc)) void* yuji_malloc(size_t size);
__attribute__((malloc)) void* yuji_realloc(void* ptr, size_t size);
