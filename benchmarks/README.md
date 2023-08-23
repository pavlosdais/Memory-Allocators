# Results

**Scenario 1: Small Allocations**

| Allocator                 | Elapsed Time |
|---------------------------|--------------|
| **Malloc**                | 0.7098 s     |
| Pool Allocator            | 0.4053 s     |
| Free List Allocator       | 0.2821 s     |
| Segregated Storage        | 0.7719 s     |

**Scenario 2: Varying Allocation Sizes**

| Allocator                 | Elapsed Time |
|---------------------------|--------------|
| **Malloc**                | 1.3454 s     |
| Pool Allocator            | 0.3744 s     |
| Free List Allocator       | 0.2909 s     |
| Segregated Storage        | 0.8145 s     |

**Scenario 3: Large Allocations**

| Allocator                 | Elapsed Time |
|---------------------------|--------------|
| **Malloc**                | 2.1041 s     |
| Pool Allocator            | 0.3644 s     |
| Free List Allocator       | 0.2957 s     |
| Segregated Storage        | 1.1878 s     |
