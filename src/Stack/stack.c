#include <stdio.h>
#include <unistd.h>  // sbrk
#include <stdint.h>  // uintptr_t
#include <stddef.h>  // max_align_t
#include <math.h>  // log2
#include "stack.h"

struct _stack_alloc
{
    Pointer curr;  // current position of the memory
    Pointer end;   // memory limit
};

static inline size_t min(const size_t a, const size_t b)  { return a < b ? a : b; }

// use sbrk to allocate memory
static inline Pointer allocate_memory(const size_t size)
{
    const Pointer memory = sbrk(size);
    if (memory == (Pointer)-1) return NULL;
    return memory;
}

static inline Pointer allocate_aligned_memory(const size_t size, const size_t alignment)
{
    const Pointer unaligned_memory = allocate_memory(size + alignment - 1);
    const Pointer aligned_memory = (Pointer)(((uintptr_t)unaligned_memory + (alignment - 1)) & ~(alignment - 1));
    return aligned_memory;
}

allocator_s st_create(const size_t size)
{
    const size_t mem_size = size + sizeof(struct _stack_alloc);
    const size_t allignment = min(1 << (size_t)log2(mem_size), _Alignof(max_align_t));
    const Pointer memory = allocate_aligned_memory(mem_size, allignment);

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
