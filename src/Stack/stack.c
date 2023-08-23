#include <stdio.h>
#include <unistd.h>  // sbrk
#include "stack.h"

struct _stack_alloc
{
    Pointer curr;  // current position of the memory
    Pointer end;   // memory limit
};

allocator_s st_create(const size_t size)
{
    Pointer memory = sbrk(size + sizeof(struct _stack_alloc));
    if (memory == (Pointer)-1) return NULL;

    const allocator_s alloc = memory;

    alloc->curr = memory + sizeof(struct _stack_alloc);
    alloc->end = alloc->curr + size;

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
