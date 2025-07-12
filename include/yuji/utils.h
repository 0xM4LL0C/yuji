#pragma once


#define ANSI_RED "\x1b[31m"
#define ANSI_RESET "\x1b[0m"
#define ANSI_BLUE "\x1b[34m"


#define LOG(...) \
  printf(ANSI_BLUE "LOG: " ANSI_RESET __VA_ARGS__); \
  printf("\n");

void panic(const char* message, ...) __attribute__((noreturn)) ;
void check_memory_is_not_null(void* ptr);

char* format_string(const char* format, ...);
