# Segregated Storage Allocator

The [segregated storage](https://www.cs.nmsu.edu/~ekerriga/presentation/index2.html) allocator is a memory management technique designed to enhance memory allocation and reduce fragmentation. By organizing memory blocks based on object sizes, this allocator optimizes memory usage and helps maintain predictable memory performance.

## How it works

1. **Memory Pool:**
   - The allocator maintains a (relatively large) memory pool from which it allocates memory in aligned blocks, improving memory access.
   - The size of the memory pool can be customized to fit the requirements of your application.

2. **Memory Allocation and Deallocation:**
   - The allocator categorizes objects into different pool sizes (4, 8, 16 bytes, and so on) based on their requested sizes.
   - For each pool size, there is a list of free memory blocks that can be reused for subsequent allocations.
   - If this list is empty, the allocator retrieves memory from the dedicated page of that pool size.

3. **Hash Table for Page Tracking:**
   - The allocator uses a hash table to associate memory pages with their corresponding pool sizes.
   - For deallocation (`seg_free`), the allocator first identifies the potential memory page the object belongs to (since memory is aligned). Then, using the hash table, it determines the associated pool size for that page, allowing efficient recycling of memory.

## Benefits
- Efficient memory allocation tailored to object sizes.
- Reduced memory fragmentation through recycling of deallocated memory.
- Enhanced memory access performance through aligned memory blocks.

## Limitations
- Due to fixed block sizes, there can be some *internal* fragmentation, resulting in wasted memory.
