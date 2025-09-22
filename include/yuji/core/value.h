#pragma once

#include "yuji/core/ast.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  VT_NUMBER,
  VT_FUNCTION,
  VT_CFUNCTION,
  VT_STRING,
  VT_NULL,
  VT_BOOL,
} YujiValueType;

typedef struct YujiValue YujiValue;

// forward declaration
typedef struct YujiScope YujiScope;

typedef YujiValue* (*YujiCFunction)(YujiScope* scope, YujiDynArray* args);

typedef struct {
  YujiASTFunction* node;
} YujiFunction;

struct YujiValue {
  YujiValueType type;
  int refcount;
  union {
    int64_t number;
    YujiFunction function;
    YujiString* string;
    YujiCFunction cfunction;
    bool bool_;
  } value;
};

#define YUJI_VALUE_INIT(NAME, TYPE, BODY, ...) \
  YujiValue* yuji_value_##NAME##_init(__VA_ARGS__) { \
    YujiValue* value = yuji_malloc(sizeof(YujiValue)); \
    value->type = TYPE; \
    value->refcount = 1; \
    BODY; \
    return value; \
  }

void yuji_value_free(YujiValue* value);

bool yuji_value_to_bool(YujiValue* value);
char* yuji_value_to_string(YujiValue* value);

YujiValue* yuji_value_number_init(int64_t number);
YujiValue* yuji_value_function_init(YujiASTFunction* node);
YujiValue* yuji_value_string_init(YujiString* string);
YujiValue* yuji_value_null_init();
YujiValue* yuji_value_cfunction_init(YujiCFunction func);
YujiValue* yuji_value_bool_init(bool bool_);
