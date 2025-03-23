#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main_abigrind();

struct ABIAllocNode
{
    struct ABIAllocNode *next;
    struct ABIAllocNode *previous;
    char *file;
    char *tag;
    size_t address;
    size_t num_bytes;
    int line_num;
};

struct ABIFreedNode
{
    struct ABIFreedNode *next;
    char *alloc_file;
    char *free_file;
    char *alloc_tag;
    char *free_tag;
    int alloc_line_num;
    int free_line_num;
    size_t address;
    size_t num_bytes;
};

static struct ABIAllocNode *ABI_ALLOCATED = NULL;
static struct ABIFreedNode *ABI_FREED = NULL;

void *malloc_abigrind(size_t size, char *file, int line, char *tag)
{
    struct ABIAllocNode *ptr = malloc(sizeof(struct ABIAllocNode) + size);
    ptr->next = ABI_ALLOCATED;
    ptr->address = (size_t)ptr;
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
    return (void *)(ptr + 1);
}

char *strdup_abigrind(char *str, char *file, int line, char *tag)
{
    size_t len = strlen(str);
    char *ptr = malloc_abigrind(len, file, line, tag);
    strncpy(ptr, str, len);
    return ptr;
}

void free_abigrind(void *ptr, char *file, int line, char *tag)
{
    struct ABIAllocNode *alloc_ptr = ((struct ABIAllocNode *)ptr) - 1;

    struct ABIAllocNode *current = ABI_ALLOCATED;
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
            struct ABIFreedNode *free_record = malloc(sizeof(struct ABIFreedNode));
            free_record->address = alloc_ptr->address;
            free_record->alloc_tag = alloc_ptr->tag;
            free_record->free_tag = strdup(tag);
            free_record->num_bytes = alloc_ptr->num_bytes;
            free_record->free_file = file;
            free_record->free_line_num = line;
            free_record->alloc_file = alloc_ptr->file;
            free_record->alloc_line_num = alloc_ptr->line_num;
            free_record->next = ABI_FREED;
            ABI_FREED = free_record;
            free(alloc_ptr);
            return;
        }
        current = current->next;
    }
    struct ABIFreedNode *current_free = ABI_FREED;
    while (current_free != NULL)
    {
        if (current_free->address == (size_t)alloc_ptr)
        {
            printf("Attempt to free a dangling pointer %p at %s:%i.\n\tOriginally allocated %lli bytes at %s:%i - %s\n\tAlready freed at %s:%i - %s\n", alloc_ptr, file, line, current_free->num_bytes, current_free->alloc_file, current_free->alloc_line_num, current_free->alloc_tag, current_free->free_file, current_free->free_line_num, current_free->free_tag);
            return;
        }
        current_free = current_free->next;
    }
    printf("Attempt to free unknown pointer %p at %s:%i.\n\t(this may be due to the allocation coming from a file compiled without abigrind)\n", ptr, file, line);
    free(ptr);
}

void *realloc_abigrind(void *ptr, size_t size, char *file, int line, char *tag)
{
    struct ABIAllocNode *alloc_ptr = ((struct ABIAllocNode *)ptr) - 1;

    struct ABIAllocNode *current = ABI_ALLOCATED;
    while (current != NULL)
    {
        if (current == alloc_ptr)
        {
            size_t old_ptr = (size_t)alloc_ptr;
            size_t old_bytes = alloc_ptr->num_bytes;
            alloc_ptr = realloc(alloc_ptr, sizeof(struct ABIAllocNode) + size);
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
            if (old_ptr != (size_t)alloc_ptr)
            {
                struct ABIFreedNode *freed = malloc(sizeof(struct ABIFreedNode));
                freed->address = old_ptr;
                freed->alloc_file = alloc_ptr->file;
                freed->alloc_line_num = alloc_ptr->line_num;
                freed->alloc_tag = strdup(alloc_ptr->tag);
                freed->free_file = file;
                freed->free_line_num = line;
                freed->free_tag = strdup(tag);
                freed->next = ABI_FREED;
                freed->num_bytes = old_bytes;
                ABI_FREED = freed;
            }
            alloc_ptr->num_bytes = size;
            alloc_ptr->address = (size_t)alloc_ptr;
            alloc_ptr->line_num = line;
            alloc_ptr->file = file;
            alloc_ptr->tag = strdup(tag);
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
    struct ABIAllocNode *node = ABI_ALLOCATED;
    while (node != NULL)
    {
        printf("%lli bytes at %s:%i not freed: %s\n", node->num_bytes, node->file, node->line_num, node->tag);
        node = node->next;
    }
    return result;
}
