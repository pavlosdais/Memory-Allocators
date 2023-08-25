#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>  // max_align_t
#include "stack.h"

struct _stack_alloc
{
    Pointer curr;  // current position of the memory
    Pointer end;   // memory limit
};

allocator_s st_create(const size_t size)
{
    // round up for alignment
    const size_t struct_size = sizeof(struct _stack_alloc);
    const size_t alignment = _Alignof(max_align_t);

    // alligned size
    const size_t struct_aligned = struct_size + (-struct_size & (alignment - 1));

    const size_t mem_size = size + struct_aligned;
    const Pointer memory = malloc(mem_size);
    if (!memory) return NULL;
 
    const allocator_s alloc = memory;
    alloc->curr = memory + struct_aligned;
    alloc->end  = memory + mem_size;
    return alloc;
}

Pointer st_alloc(const allocator_s alloc, const size_t size)
{
    if (!alloc || alloc->curr + size > alloc->end)
        return NULL;
    else
    {
        const Pointer memory = alloc->curr;
        alloc->curr += size;
        return memory;
    }
}

void st_free(const allocator_s alloc, const Pointer obj)
{
    if (!alloc || !obj) return;

    const size_t obj_size = alloc->curr - obj;
    alloc->curr -= obj_size;
}

void st_destroy(const allocator_s alloc)
{
    if (!alloc) return;
    free(alloc);
}
