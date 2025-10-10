#pragma once

#include "yuji/core/ast.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  VT_INT,
  VT_FLOAT,
  VT_FUNCTION,
  VT_CFUNCTION,
  VT_STRING,
  VT_NULL,
  VT_BOOL,
  VT_ARRAY,
} YujiValueType;

typedef struct YujiValue YujiValue;

// forward declaration
struct YujiScope;

typedef struct {
  YujiASTFunction* node;
} YujiFunction;

typedef struct {
  size_t argc;
  YujiValue* (*func)(struct YujiScope* scope, YujiDynArray* args);
} YujiCFunction;

struct YujiValue {
  YujiValueType type;
  int refcount;
  union {
    int64_t int_;
    double float_;
    YujiFunction function;
    YujiString* string;
    YujiCFunction* cfunction;
    bool bool_;
    YujiDynArray* array;
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
char* yuji_value_type_to_string(YujiValueType type);
bool yuji_value_type_is(YujiValueType type, YujiValueType expected);

YujiValue* yuji_value_int_init(int64_t number);
YujiValue* yuji_value_float_init(double number);
YujiValue* yuji_value_function_init(YujiASTFunction* node);
YujiValue* yuji_value_string_init(YujiString* string);
YujiValue* yuji_value_null_init();
YujiValue* yuji_value_cfunction_init(size_t argc,
                                     YujiValue * (*func)(struct YujiScope* scope, YujiDynArray* args));
YujiValue* yuji_value_bool_init(bool bool_);
YujiValue* yuji_value_array_init(YujiDynArray* array);
