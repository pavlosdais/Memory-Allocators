#pragma once  // include at most once

typedef void* Pointer;

// allocator handle
typedef struct _stack_alloc* allocator_s;


// create allocator using the specified size
allocator_s st_create(const size_t);

// allocate memory for the object
Pointer st_alloc(const allocator_s, const size_t);

// deallocate the object
// only works if the object is the last deallocated
void st_free(const allocator_s, const Pointer);
