#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "free_list.h"

// Define a structure for a block in the free list
typedef struct _list_ptr
{
    size_t size;
    struct _list_ptr* next;
}
_list_ptr;
typedef _list_ptr* list_ptr;

struct _free_allocator
{
    list_ptr free_list;
};


allocator_f frl_create(const size_t size)
{
    const Pointer memory = sbrk(size + sizeof(struct _free_allocator));
    if (memory == (Pointer)-1) return NULL;

    const allocator_f alloc = memory;
    alloc->free_list = memory + sizeof(struct _free_allocator);

    alloc->free_list->size = size - sizeof(_list_ptr);
    alloc->free_list->next = NULL;

    return alloc;
}

Pointer frl_alloc(const allocator_f alloc, const size_t size)
{
    if (!alloc) return NULL;

    list_ptr curr = alloc->free_list, prev = NULL;

    while (curr != NULL)
    {
        if (curr->size >= size)
        {
            // if the block size is big enough, allocate from it
            if (curr->size >= size + sizeof(_list_ptr))
            {
                const list_ptr newBlock = curr + size + sizeof(_list_ptr);
                newBlock->size = curr->size - size - sizeof(_list_ptr);
                newBlock->next = curr->next;
                curr->next = newBlock;
                curr->size = size;
            }

            // remove the allocated block from the free list
            if (prev)
                prev->next = curr->next;
            else
                alloc->free_list = curr->next;

            return (Pointer)curr + sizeof(_list_ptr);
        }

        prev = curr;
        curr = curr->next;
    }

    // no suitable block found
    return NULL;
}

void frl_free(const allocator_f alloc, const Pointer obj)
{
    if (!obj || !obj) return;

    const list_ptr block = obj - sizeof(_list_ptr);
    block->next = alloc->free_list;
    alloc->free_list = block;

    // perform coalescence - merge adjacent free memory if possible
    list_ptr curr = alloc->free_list, prev = NULL;
    while (curr != NULL)
    {
        const list_ptr next_block = curr->next;

        // the next block is adjacent to the freed block
        if (next_block && block + block->size + sizeof(_list_ptr) == next_block)
        {
            // merge them
            block->size +=  next_block->size + sizeof(_list_ptr);
            block->next = next_block->next;
            break;
        }
        // the previous block is adjacent to the freed block
        else if (prev && prev + prev->size + sizeof(_list_ptr) == block)
        {
            // merge them
            prev->size += block->size + sizeof(_list_ptr);
            prev->next = block->next;
            break;
        }

        prev = curr;
        curr = curr->next;
    }
}
