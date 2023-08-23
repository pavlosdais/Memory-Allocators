#include <stdio.h>
#include <unistd.h>  // sbrk
#include "pool.h"

typedef struct _list_ptr* list_ptr;
struct _list_ptr
{
    list_ptr next;
};

typedef struct _pool_allocator
{
    list_ptr free_list;
}
PoolAllocator;

allocator_p pool_create(const size_t chunks_num, const size_t chunk_size)
{
    // calculate the total sizes needed for the pool
    const size_t total_size = chunks_num * (sizeof(struct _list_ptr) + chunk_size);
    Pointer memory = sbrk(total_size + sizeof(PoolAllocator));
    if (memory == (Pointer)-1) return NULL;

    allocator_p alloc = memory;
    alloc->free_list = NULL;
    memory += sizeof(*alloc);

    // initialize the free list with memory chunks
    for (size_t i = 0; i < chunks_num; i++)
    {
        const list_ptr chunk = memory + i * (chunk_size + sizeof(struct _list_ptr));
        chunk->next = alloc->free_list;
        alloc->free_list = chunk;
    }

    return alloc;
}

Pointer pool_alloc(const allocator_p alloc)
{
    if (!alloc || !alloc->free_list) return NULL;

    // get a memory chunk from the free list
    const list_ptr chunk = alloc->free_list;
    alloc->free_list = chunk->next;

    // return a pointer to the memory chunk after the list node
    return (Pointer)chunk + sizeof(struct _list_ptr);
}

void pool_free(const allocator_p alloc, const Pointer obj)
{
    if (!alloc || !obj) return;

    // convert the object pointer to a chunk pointer
    const list_ptr chunk = obj - sizeof(struct _list_ptr);

    // add the chunk back to the free list
    chunk->next = alloc->free_list;
    alloc->free_list = chunk;
}
