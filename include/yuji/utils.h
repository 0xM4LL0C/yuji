#pragma once

#define YUJI_LOG(...) printf("%s:%s:%d ", __FILE__, __FUNCTION__, __LINE__); printf(__VA_ARGS__); printf("\n");
#define YUJI_CONCAT(A, B) A##B
