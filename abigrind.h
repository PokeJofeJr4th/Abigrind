#ifndef ABIGRIND_H_
#define ABIGRIND_H_
#include <stdlib.h>

#ifdef ENABLE_ABIGRIND
#define malloc(x) malloc_abigrind(x, __FILE__, __LINE__, "malloc")
#define malloc_tagged(x, tag) malloc_abigrind(x, __FILE__, __LINE__, tag)
#define realloc(x, y) realloc_abigrind(x, y, __FILE__, __LINE__, "realloc")
#define realloc_tagged(x, y, tag) realloc_abigrind(x, y, __FILE__, __LINE__, tag)
#define strdup(x) strdup_abigrind(x, __FILE__, __LINE__, "strdup")
#define strdup_tagged(x, tag) strdup_abigrind(x, __FILE__, __LINE__, tag)
#define free(x) free_abigrind(x, __FILE__, __LINE__, "free")
#define free_tagged(x, tag) free_abigrind(x, __FILE__, __LINE__, tag)
#define main main_abigrind
void *malloc_abigrind(size_t size, char *file, int line, char *tag);
void *realloc_abigrind(char *ptr, size_t size, char *file, int line, char *tag);
char *strdup_abigrind(char *str, char *file, int line, char *tag);
void free_abigrind(void *ptr, char *file, int line, char *tag);
#else
#define malloc_tagged(x, tag) malloc(x)
#define realloc_tagged(x, y, tag) realloc(x, y)
#define strdup_tagged(x, tag) strdup(x)
#define free_tagged(x, tag) free(x)
#endif

#endif