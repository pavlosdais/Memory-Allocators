# Memory Allocators

This repository contains implementations of various [memory allocators](https://en.wikipedia.org/wiki/Memory_management). These allocators use different strategies for managing memory, each with its own set of trade-offs and benefits. This project aims to provide an understanding of memory allocation techniques and their applications. 

# Why Choose Custom Memory Allocators?

Using a custom memory allocator can lead to improved performance and memory management for specific tasks when compared to the general-purpose `malloc` function. Custom allocators optimize memory usage, decrease fragmentation, and have faster allocation and deallocation, which can be particularly beneficial for applications with distinct memory requirements.

# Implemented Allocators

This project currently implements the following memory allocators:

- [Stack](https://github.com/pavlosdais/Memory-Allocators/tree/main/src/Stack#readme)
- [Pool](https://github.com/pavlosdais/Memory-Allocators/tree/main/src/Pool#readme)
- [Free List](https://github.com/pavlosdais/Memory-Allocators/tree/main/src/FreeList#readme)
- [Segregated Storage](https://github.com/pavlosdais/Memory-Allocators/tree/main/src/SegregatedStorage#readme)

The source code of every allocator can be found over at the `src` directory.

# Benchmarks
Detailed benchmark results for different allocation scenarios (small/ varying/ large) using the memory allocators are available [here](https://github.com/pavlosdais/Memory-Allocators/tree/main/benchmarks/README.md). This file is included in the repository for your reference over at the `benchmarks` directory.

---
© Pavlos Dais
