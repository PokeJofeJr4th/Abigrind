#ifndef ABIGRIND_H_
#define ABIGRIND_H_
#include <stdlib.h>

#ifdef ENABLE_ABIGRIND
#define malloc_tagged(x, tag) malloc_abi(x, __FILE__, __LINE__, tag)
#define malloc(x) malloc_abi(x, __FILE__, __LINE__, "<no tag>")
#define free(x) free_abi(x, __FILE__, __LINE__)
#define main main_abi
void *malloc_abi(size_t size, char *file, int line, char *tag);
void free_abi(void *ptr, char *file, int line);
#else
#define malloc_tagged(x, tag) malloc(x)
#endif

#endif