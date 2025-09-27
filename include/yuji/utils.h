#pragma once

#define YUJI_LOG(...) printf("%s:%s:%d ", __FILE__, __FUNCTION__, __LINE__); printf(__VA_ARGS__); printf("\n");

#define YUJI_STRCMP(STR1, STR2) strcmp(STR1, STR2) == 0

#define YUJI_UNUSED(X) (void)(X)
