
**Scenario 1: Small Allocations**

| Allocator                 | Elapsed Time |
|---------------------------|--------------|
| **Malloc**                | 0.6573 s     |
| Stack Allocator           | 0.1286 s     |
| Pool Allocator            | 0.1923 s     |
| Free List Allocator       | Ran out      |
| Segregated Storage        | 0.6060 s     |

**Scenario 2: Varying Allocation Sizes**

| Allocator                 | Elapsed Time |
|---------------------------|--------------|
| **Malloc**                | 1.1183 s     |
| Stack Allocator           | 0.1290 s     |
| Pool Allocator            | 0.1913 s     |
| Free List Allocator       | Ran out      |
| Segregated Storage        | 0.6982 s     |

**Scenario 3: Large Allocations**

| Allocator                 | Elapsed Time |
|---------------------------|--------------|
| **Malloc**                | 1.7651 s     |
| Stack Allocator           | 0.1301 s     |
| Pool Allocator            | 0.1904 s     |
| Free List Allocator       | Ran out      |
| Segregated Storage        | 1.0681 s     |
