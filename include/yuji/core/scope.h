#pragma once

#include "yuji/core/interpreter.h"
#include "yuji/core/types/map.h"


YujiScope* yuji_scope_init(YujiScope* parent);
void yuji_scope_free(YujiScope* scope);
