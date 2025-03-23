#include "abigrind.h"
#include <stdlib.h>

int main()
{
    void *ptr_a = malloc_tagged(100, "First pointer");
    void *ptr_b = malloc_tagged(101, "Second pointer");
    void *ptr_c = malloc_tagged(102, "Third pointer");
    // free((void *)100000);
    free(ptr_b);
    return 0;
}
