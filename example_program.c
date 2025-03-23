#include "abigrind.h"
#include <stdlib.h>

int main()
{
    void *ptr_a = malloc_tagged(100, "First pointer");
    void *ptr_b = malloc_tagged(101, "Second pointer");
    void *ptr_c = malloc_tagged(102, "Third pointer");

    ptr_b = realloc_tagged(ptr_b, 200, "reallocation go brrrr");

    // free((void *)100000);
    free_tagged(ptr_b, "We free it the first time");
    free_tagged(ptr_b, "We free it the second time");
    return 0;
}
