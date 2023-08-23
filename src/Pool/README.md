# Pool Allocator

The [pool](https://en.wikipedia.org/wiki/Memory_pool) allocator is a memory allocation technique that preallocates a fixed-size pool of memory blocks to efficiently manage allocations of a specific size.

## How It Works

1. **Memory Pool Creation:**
   - The allocator creates a memory pool by pre-allocating a specific number of memory chunks, each of a fixed size.
   - The memory pool is managed by a linked list, where each memory chunk is linked together.

2. **Memory Allocation:**
   - When memory is requested (`pool_alloc`), the allocator retrieves a memory chunk from the free list within the memory pool.

3. **Memory Deallocation:**
   - When memory is deallocated (`pool_free`), the allocator adds the memory chunk back to the free list, making it available for subsequent allocations.

## Features

- Efficient memory allocation using pre-allocated memory chunks.
- Reduced memory fragmentation through immediate memory recycling.
- Improved performance by eliminating the need for complex memory allocation algorithms.

## Limitations
- The fixed block size can lead to inefficiencies when allocating larger-than-expected memory chunks.
