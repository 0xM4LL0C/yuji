#pragma once

#define LOG(...) printf("%s:%s:%d ", __FILE__, __FUNCTION__, __LINE__); printf(__VA_ARGS__); printf("\n");

#define DEFER(...) for (int _i=1; _i; _i=0, __VA_ARGS__)