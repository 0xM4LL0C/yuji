#pragma once

#define ANSI_RED "\x1b[31m"
#define ANSI_RESET "\x1b[0m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_PURPLE "\x1b[35m"

#ifndef NDEBUG
#define LOG(fmt, ...) \
    printf(ANSI_BLUE "LOG: " ANSI_PURPLE "%s:%d:%s" ANSI_RESET " | " fmt "\n", \
      __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif

void panic(const char* message, ...) __attribute__((noreturn)) ;
void check_memory_is_not_null(void* ptr);

char* format_string(const char* format, ...);
