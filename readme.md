## Abigrind

A valgrind-like utility to assist memory management debugging

### Quickstart Guide

1. Compile the abigrind object file (with gcc: `gcc -c main.c -o abigrind.o`)
2. Add the `abigrind.h` and `abigrind.o` files to your project.
3. Make sure you have `#include "abigrind.h"` in each file you're compiling (normally every `.c` file). This won't change anything when compiling with your normal workflow.
4. To compile in debug mode, define the `ENABLE_ABIGRIND` preprocessor flag (with gcc, `-DENABLE_ABIGRIND`) and include `abigrind.o` as an input.

### About

Abigrind provides wrappers around the standard `malloc` and `free` functions that keep track of memory you allocate and fail to clean up. It does this by adding metadata to your allocations and maintaining a linked list of memory blocks. It also provides the `malloc_tagged` macro so that you can provide additional string metadata to assist in debugging.
