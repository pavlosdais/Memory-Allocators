#pragma once  // include at most once

typedef void* Pointer;

// allocator handle
typedef struct _pool_allocator* allocator_p;


// create allocator
// providing the number of chunks and their size
allocator_p pool_create(const size_t chunks_num, const size_t chunk_size);

// return a chunk of memory
Pointer pool_alloc(const allocator_p);

// deallocate the object
void pool_free(const allocator_p, const Pointer);
