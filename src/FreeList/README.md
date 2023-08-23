# Free List Allocator

The [free list](https://en.wikipedia.org/wiki/Free_list) allocator is a memory allocation technique that maintains a linked list of free blocks to efficiently manage memory allocations and deallocations.

## How It Works

1. **Free List Management:**
   - The allocator maintains a linked list of free memory blocks, each with its size and a reference to the next free block.
  
2. **Memory Allocation:**
   - When memory is requested (`frl_alloc`), the allocator searches the free list for a suitable memory block.
   - If a block larger than the requested size is found, it may be split into two blocks to optimize memory utilization.

3. **Memory Deallocation:**
   - When memory is deallocated (`frl_free`), the freed memory block is inserted back into the free list.
   - Coalescence is also performed, merging adjacent free memory blocks to minimize fragmentation.

## Features

- Efficient memory allocation using free memory blocks.
- Reduced memory fragmentation due to optimal memory recycling.
- Suitable for scenarios with varying memory allocation sizes.

## Limitations
- Maintaining a linked list of free blocks can introduce overhead in terms of memory usage and allocation time.
