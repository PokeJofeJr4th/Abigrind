#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main_abigrind();

struct AllocNode
{
    struct AllocNode *next;
    struct AllocNode *previous;
    char *file;
    char *tag;
    size_t num_bytes;
    int line_num;
};

static struct AllocNode *ABI_ALLOCATED = NULL;

void *malloc_abigrind(size_t size, char *file, int line, char *tag)
{
    struct AllocNode *ptr = malloc(sizeof(struct AllocNode) + size);
    ptr->next = ABI_ALLOCATED;
    if (ABI_ALLOCATED != NULL)
    {
        ABI_ALLOCATED->previous = ptr;
    }
    ptr->line_num = line;
    ptr->file = file;
    ptr->num_bytes = size;
    ptr->previous = NULL;
    ptr->tag = strdup(tag);
    ABI_ALLOCATED = ptr;
    return ptr + 1;
}

char *strdup_abigrind(char *str, char *file, int line)
{
    size_t len = strlen(str);
    char *ptr = malloc_abigrind(len, file, line, "strdup");
    strncpy(ptr, str, len);
}

void free_abigrind(void *ptr, char *file, int line)
{
    struct AllocNode *alloc_ptr = ((struct AllocNode *)ptr) - 1;

    struct AllocNode *current = ABI_ALLOCATED;
    while (current != NULL)
    {
        if (current == alloc_ptr)
        {
            if (alloc_ptr->previous == NULL)
            {
                ABI_ALLOCATED = alloc_ptr->next;
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
        current = current->next;
    }
    printf("Attempt to free unknown pointer %p at %s:%i.\n\t(this may be due to a double-free or the allocation coming from a file compiled without abigrind)\n", ptr, file, line);
    free(ptr);
}

void *realloc_abigrind(void *ptr, size_t size, char *file, int line)
{
    struct AllocNode *alloc_ptr = ((struct AllocNode *)ptr) - 1;

    struct AllocNode *current = ABI_ALLOCATED;
    while (current != NULL)
    {
        if (current == alloc_ptr)
        {
            alloc_ptr = realloc(alloc_ptr, sizeof(struct AllocNode) + size);
            if (alloc_ptr->previous == NULL)
            {
                ABI_ALLOCATED = alloc_ptr;
            }
            else
            {
                alloc_ptr->previous->next = alloc_ptr;
            }
            if (alloc_ptr->next != NULL)
            {
                alloc_ptr->next->previous = alloc_ptr;
            }
            return (void *)(alloc_ptr + 1);
        }
        current = current->next;
    }
    printf("Attempt to realloc unknown pointer %p at %s:%i.\n\t(this may be due to a double-free or the allocation coming from a file compiled without abigrind)\n", ptr, file, line);
    return realloc(ptr, size);
}

int main()
{
    int result = main_abigrind();
    struct AllocNode *node = ABI_ALLOCATED;
    while (node != NULL)
    {
        printf("%lli bytes at %s:%i not freed: %s\n", node->num_bytes, node->file, node->line_num, node->tag);
        node = node->next;
    }
    return result;
}
