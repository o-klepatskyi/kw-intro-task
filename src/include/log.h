#include <cstdio>

#if LOG_LEVEL >= 1
#define LogInfo(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define LogInfo(fmt, ...)
#endif

#if LOG_LEVEL >= 2
#define LogError(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else 
#define LogError(fmt, ...)
#endif