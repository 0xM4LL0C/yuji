#pragma once

#include "yuji/module.h"
#include "yuji/value.h"

YujiValue* io_print(DynArr* args);
YujiModule* yuji_module_io_init();
