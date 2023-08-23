# Stack Allocator

The [stack](https://blog.molecular-matters.com/2012/08/14/memory-allocation-strategies-a-linear-allocator/) allocator is a minimalistic technique designed for straightforward memory allocation and deallocation patterns. Operating as a linear memory region with stack-like behavior, this allocator is ideal for scenarios where you need a simple and efficient approach.

## How it works

1. **Memory Allocation:**
   - The allocator utilizes a linear memory region to allocate and manage memory.
   - Memory is allocated (`st_alloc`) sequentially from the start of the memory region.

2. **Memory Deallocation:**
   - When memory is deallocated (`st_free`), the stack pointer is moved back, releasing the most recently allocated memory block.

## Features

- Simple memory management approach.
- Efficient allocation and deallocation patterns.
- Minimal memory fragmentation due to the linear memory layout.

## Limitations
- Being a stack, the allocator operates in a last-in, first-out manner. This might not be suitable for all allocation patterns.
