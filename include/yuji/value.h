#pragma once

#include "yuji/ast.h"

typedef enum {
  VT_NUMBER,
  VT_FUNCTION,
  VT_CFUNCTION,
  VT_STRING,
  VT_NULL,
  VT_BOOL,
} ValueType;

typedef struct YujiValue YujiValue;

typedef YujiValue* (*YujiCFunction)(DynArr* args);

typedef struct {
  ASTNode* node;
} YujiFunction;

struct YujiValue {
  ValueType type;
  union {
    int number;
    YujiFunction function;
    char* string;
    YujiCFunction cfunction;
    bool bool_;
  } value;
};

void value_free(YujiValue* value);

YujiValue* value_number_init(int number);
YujiValue* value_function_init(ASTNode* node);
YujiValue* value_string_init(const char* string);
YujiValue* value_null_init();
YujiValue* value_cfunction_init(YujiCFunction func);
YujiValue* value_bool_init(bool bool_);

bool value_to_bool(YujiValue* value);
