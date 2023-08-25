#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>  // max_align_t
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
    // round up for alignment
    const size_t alignment = _Alignof(max_align_t);
    const size_t struct_size = sizeof(struct _free_allocator);

    // alligned size
    const size_t struct_aligned = struct_size + (-struct_size & (alignment - 1));

    const size_t mem_size = size + struct_aligned;
    const Pointer memory = malloc(mem_size);
    if (!memory) return NULL;

    const allocator_f alloc = memory;
    alloc->free_list = memory + struct_aligned;

    alloc->free_list->size = size - (size_t)sizeof(_list_ptr);
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
        if (prev && prev + prev->size + sizeof(_list_ptr) == curr)
        {
            prev->size += curr->size + sizeof(_list_ptr);
            prev->next = curr->next;
            curr = prev;
        }

        prev = curr;
        curr = curr->next;
    }
}
