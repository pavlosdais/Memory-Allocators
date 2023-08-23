#pragma once  // include at most once

typedef void* Pointer;

// allocator handle
typedef struct _free_allocator* allocator_f;


// create allocator using the specified size
allocator_f frl_create(const size_t);

// allocate memory for the object
Pointer frl_alloc(const allocator_f, const size_t);

// deallocate the object
void frl_free(const allocator_f, const Pointer);
