#pragma once

#include "yuji/module.h"
#include "yuji/value.h"

YujiValue* io_print(DynArr* args);
YujiValue* io_println(DynArr* args);
YujiValue* io_input(DynArr* args);

YujiModule* yuji_module_io_init();
