#include "yuji/core/interpreter.h"
#include "yuji/core/memory.h"
#include "yuji/core/module.h"
#include "yuji/core/types/dyn_array.h"
#include "yuji/core/types/string.h"
#include "yuji/core/value.h"
#include "yuji/utils.h"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static int _string_to_file_mode(const char* str) {
  int flags = 0;

  if (strcmp(str, "r") == 0) {
    flags = O_RDONLY;
  } else if (strcmp(str, "r+") == 0) {
    flags = O_RDWR;
  } else if (strcmp(str, "w") == 0) {
    flags = O_WRONLY | O_CREAT | O_TRUNC;
  } else if (strcmp(str, "w+") == 0) {
    flags = O_RDWR | O_CREAT | O_TRUNC;
  } else if (strcmp(str, "a") == 0) {
    flags = O_WRONLY | O_CREAT | O_APPEND;
  } else if (strcmp(str, "a+") == 0) {
    flags = O_RDWR | O_CREAT | O_APPEND;
  } else {
    yuji_panic("invalid open mode");
  }

  return flags;
}

static YujiValue* io_print(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size == 0) {
    yuji_panic("print function requires at least one argument");
  }

  YUJI_DYN_ARRAY_ITER(args, YujiValue, arg, {
    char* str = yuji_value_to_string(arg);
    printf("%s", str);
    yuji_free(str);
  })

  return yuji_value_null_init();
}

static YujiValue* io_println(YujiScope* scope, YujiDynArray* args) {
  YujiValue* result = io_print(scope, args);
  printf("\n");
  return result;
}

static YujiValue* io_input(YujiScope* scope, YujiDynArray* args) {
  if (args->size == 1) {
    YujiValue* tmp = io_print(scope, args);
    yuji_value_free(tmp);
  }

  YujiString* result = yuji_string_init();
  char buffer[1024];

  if (fgets(buffer, sizeof(buffer), stdin)) {
    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    yuji_string_append_cstr(result, buffer);
  }

  YujiValue* val = yuji_value_string_init(result);
  yuji_string_free(result);
  return val;
}

static YujiValue* io_format(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size == 0) {
    yuji_panic("format function requires at least one argument (format string)");
  }

  YujiValue* fmt_val = yuji_dyn_array_get(args, 0);

  if (!yuji_value_type_is(fmt_val->type, VT_STRING)) {
    yuji_panic("format function expects a string as first argument");
  }

  char* fmt = fmt_val->value.string->data;
  YujiString* result = yuji_string_init();

  size_t arg_index = 1;

  for (size_t i = 0; fmt[i] != '\0'; i++) {
    if (fmt[i] == '{' && fmt[i + 1] == '}') {
      if (arg_index >= args->size) {
        yuji_string_free(result);
        yuji_panic("format: not enough arguments for placeholders");
      }

      YujiValue* val = yuji_dyn_array_get(args, arg_index++);
      char* val_str = yuji_value_to_string(val);
      yuji_string_append_cstr(result, val_str);
      yuji_free(val_str);
      i++;
    } else {
      yuji_string_append_char(result, fmt[i]);
    }
  }

  YujiValue* formatted = yuji_value_string_init(result);
  yuji_string_free(result);
  return formatted;
}

static YujiValue* io_open(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 2) {
    yuji_panic("open function expects 2 arguments, got %ld", args->size);
  }

  YujiValue* name = yuji_dyn_array_get(args, 0);
  YujiValue* mode = yuji_dyn_array_get(args, 1);

  if (name->type != VT_STRING) {
    yuji_panic("open function expects an string argument, got %s", yuji_value_to_string(name));
  } else if (mode->type != VT_STRING) {
    yuji_panic("open function expects an string argument, got %s", yuji_value_to_string(mode));
  }

  int fd = open(name->value.string->data, _string_to_file_mode(mode->value.string->data), 0644);

  if (fd < 0) {
    yuji_panic("open failed: %s", strerror(errno));
  }

  return yuji_value_int_init(fd);
}

static YujiValue* io_close(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("close function expects 1 arguments, got %ld", args->size);
  }

  YujiValue* fd = yuji_dyn_array_get(args, 0);

  if (fd->type != VT_INT) {
    yuji_panic("close function expects an int argument, got %s", yuji_value_to_string(fd));
  }

  close((int)fd->value.int_);

  return yuji_value_null_init();
}

static YujiValue* io_write(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 2) {
    yuji_panic("write function expects 2 arguments, got %ld", args->size);
  }

  YujiValue* fd = yuji_dyn_array_get(args, 0);
  YujiValue* data = yuji_dyn_array_get(args, 1);

  if (fd->type != VT_INT) {
    yuji_panic("write function expects an int argument, got %s", yuji_value_to_string(fd));
  } else if (data->type != VT_STRING) {
    yuji_panic("write function expects an string argument, got %s", yuji_value_to_string(fd));
  }

  ssize_t r = write((int)fd->value.int_, data->value.string->data, data->value.string->size);

  if (r < 0) {
    yuji_panic("write failed: %s", strerror(errno));
  }

  fsync((int)fd->value.int_);

  return yuji_value_null_init();
}

static YujiValue* io_read(YujiScope* scope, YujiDynArray* args) {
  YUJI_UNUSED(scope);

  if (args->size != 1) {
    yuji_panic("read expects 1 argument");
  }

  YujiValue* fd = yuji_dyn_array_get(args, 0);

  if (fd->type != VT_INT) {
    yuji_panic("read expects int fd");
  }

  YujiString* str = yuji_string_init();
  char buf[4096];


  ssize_t n;

  while ((n = read((int)fd->value.int_, buf, 4096)) > 0) {
    yuji_string_append(str, buf, (size_t)n);
  }

  if (n < 0) {
    yuji_string_free(str);
    yuji_panic("read failed: %s", strerror(errno));
  }

  YujiValue* v = yuji_value_string_init(str);
  yuji_string_free(str);
  return v;
}

YUJI_DEFINE_MODULE(io, {
  YUJI_MODULE_REGISTER(int, module, "stdin", STDIN_FILENO);
  YUJI_MODULE_REGISTER(int, module, "stdout", STDOUT_FILENO);
  YUJI_MODULE_REGISTER(int, module, "stderr", STDERR_FILENO);

  YUJI_MODULE_REGISTER_FUNC(module, "print", YUJI_FN_INF_ARGUMENT, io_print);
  YUJI_MODULE_REGISTER_FUNC(module, "println", YUJI_FN_INF_ARGUMENT, io_println);
  YUJI_MODULE_REGISTER_FUNC(module, "input", YUJI_FN_ARGC(1), io_input);
  YUJI_MODULE_REGISTER_FUNC(module, "format", YUJI_FN_INF_ARGUMENT, io_format);
  YUJI_MODULE_REGISTER_FUNC(module, "open", YUJI_FN_ARGC(2), io_open);
  YUJI_MODULE_REGISTER_FUNC(module, "close", YUJI_FN_ARGC(1), io_close);
  YUJI_MODULE_REGISTER_FUNC(module, "write", YUJI_FN_ARGC(2), io_write);
  YUJI_MODULE_REGISTER_FUNC(module, "read", YUJI_FN_ARGC(1), io_read);
})
