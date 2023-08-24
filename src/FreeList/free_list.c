#include <stdio.h>
#include <unistd.h>  // sbrk
#include <stdint.h>  // uintptr_t
#include <stddef.h>  // max_align_t
#include <math.h>  // log2
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

static inline size_t min(const size_t a, const size_t b)  { return a < b ? a : b; }

// use sbrk to allocate memory
static Pointer allocate_memory(const size_t size)
{
    const Pointer memory = sbrk(size);
    if (memory == (Pointer)-1) return NULL;
    return memory;
}

static Pointer allocate_aligned_memory(const size_t size, const size_t alignment)
{
    const Pointer unaligned_memory = allocate_memory(size + alignment - 1);
    const Pointer aligned_memory = (Pointer)(((uintptr_t)unaligned_memory + (alignment - 1)) & ~(alignment - 1));
    return aligned_memory;
}

allocator_f frl_create(const size_t size)
{
    const size_t mem_size = size + sizeof(struct _free_allocator);
    const size_t allignment = min(1 << (size_t)log2(mem_size), _Alignof(max_align_t));

    Pointer memory = allocate_aligned_memory(mem_size, allignment);
    if (memory == NULL)
        return NULL;

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
