#pragma once

typedef enum {
  VT_NUMBER,
} ValueType;

typedef struct {
  ValueType type;
  union {
    int number;
  } value;
} YujiValue;

void value_free(YujiValue* value);

YujiValue* value_number_init(int number);
