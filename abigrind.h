#ifndef ABIGRIND_H_
#define ABIGRIND_H_
#include <stdlib.h>

#ifdef ENABLE_ABIGRIND
    #define malloc_tagged(x, tag) malloc_abigrind(x, __FILE__, __LINE__, tag)
    #define malloc(x) malloc_abigrind(x, __FILE__, __LINE__, "<no tag>")
    #define realloc_tagged(x, y, tag) realloc_abigrind(x, y, __FILE__, __LINE__, tag)
    #define realloc(x, y) realloc_abigrind(x, y, __FILE__, __LINE__, "<no tag>")
    #define strdup(x) strdup_abigrind(x, __FILE__, __LINE__)
    #define free(x) free_abigrind(x, __FILE__, __LINE__)
    #define main main_abi
    void *malloc_abigrind(size_t size, char *file, int line, char *tag);
    void *realloc_abigrind(char *ptr, size_t size, char *file, int line, char *tag);
    char *strdup_abigrind(char *str, char *file, int line);
    void free_abigrind(void *ptr, char *file, int line);
#else
    #define malloc_tagged(x, tag) malloc(x)
#endif

#endif