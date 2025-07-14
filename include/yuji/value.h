#pragma once

#include "yuji/ast.h"

typedef enum {
  VT_NUMBER,
  VT_FUNCTION,
  VT_STRING,
} ValueType;

typedef struct {
  ASTNode* node;
} YujiFunction;

typedef struct {
  ValueType type;
  union {
    int number;
    YujiFunction function;
    char* string;
  } value;
} YujiValue;

void value_free(YujiValue* value);

YujiValue* value_number_init(int number);
YujiValue* value_function_init(ASTNode* node);
YujiValue* value_string_init(const char* string);
