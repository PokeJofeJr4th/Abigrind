#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main_abi();

struct AllocNode
{
    struct AllocNode *next;
    struct AllocNode *previous;
    char *file;
    size_t num_bytes;
    int line_num;
    char *tag;
};

static struct AllocNode *ABI_HEAD = NULL;

void *malloc_abi(size_t size, char *file, int line, char *tag)
{
    struct AllocNode *ptr = malloc(sizeof(struct AllocNode) + size);
    ptr->next = ABI_HEAD;
    if (ABI_HEAD != NULL)
    {
        ABI_HEAD->previous = ptr;
    }
    ptr->line_num = line;
    ptr->file = file;
    ptr->num_bytes = size;
    ptr->previous = NULL;
    ptr->tag = strdup(tag);
    ABI_HEAD = ptr;
    return ptr + 1;
}

void free_abi(void *ptr, char *file, int line)
{
    struct AllocNode *alloc_ptr = ((struct AllocNode *)ptr) - 1;

    struct AllocNode *cur = ABI_HEAD;

    while (cur != NULL)
    {
        if (cur == alloc_ptr)
        {
            if (alloc_ptr->previous == NULL)
            {
                ABI_HEAD = alloc_ptr->next;
            }
            else
            {
                alloc_ptr->previous->next = alloc_ptr->next;
            }
            if (alloc_ptr->next != NULL)
            {
                alloc_ptr->next->previous = alloc_ptr->previous;
            }
            free(alloc_ptr->tag);
            free(alloc_ptr);
            return;
        }
        cur = cur->next;
    }
    printf("Attempt to free unknown pointer %p at %s:%i.\n\t(this may be due to a double-free or the allocation coming from a file compiled without abigrind)\n", ptr, file, line);
    free(ptr);
}

int main()
{
    int result = main_abi();
    struct AllocNode *node = ABI_HEAD;
    while (node != NULL)
    {
        printf("%lli bytes at %s:%i not freed: %s\n", node->num_bytes, node->file, node->line_num, node->tag);
        node = node->next;
    }
    return result;
}
