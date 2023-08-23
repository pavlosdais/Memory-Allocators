#pragma once  // include at most once

// generic pointer
typedef void* Pointer;

// allocator handle
typedef struct _seg_allocator* allocator_ss;

// megabytes of the pool
#define POOLMB 1


// create allocator
allocator_ss seg_create(void);

// allocate memory for the object
Pointer seg_alloc(const allocator_ss alloc, const size_t);

// resize a previously allocated memory block and return a pointer to it
Pointer seg_realloc(const allocator_ss, const Pointer, const size_t);

// deallocate the object
void seg_free(const allocator_ss, const Pointer);

// destory memory used by the allocator
void seg_destroy(const allocator_ss);
