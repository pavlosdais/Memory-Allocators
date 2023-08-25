#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../src/SegregatedStorage/segregated.h"
#include "../src/FreeList/free_list.h"
#include "../src/Pool/pool.h"
#include "../src/Stack/stack.h"

#define ITERATION_NUM 10
#define NUM_OPERATIONS_PER_ITERATION 10000
#define ALLOC_PROBABILITY 0.5

#define MIN_SIZE 64
#define MID_SIZE 1024
#define MAX_SIZE 4096

#define ALLOC_SIZE 999999999

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// how many times to run the tests
#define RUN_TIMES 1

// function prototypes
void test_malloc(void);
void test_stack(void);
void test_SegregatedStorage(void);
void test_Pool(void);
void test_Free_List(void);

int main(void)
{
    srand(time(NULL));

    // run the tests
    test_malloc();
    test_stack();
    test_Pool();
    // test_Free_List();
    test_SegregatedStorage();

    return 0;
}

double run_seg(const allocator_ss alloc, const size_t max_size)
{
    const clock_t start_time = clock();

    for (size_t iter = 0; iter < ITERATION_NUM; iter++)
    {
        Pointer* ptrs = calloc(NUM_OPERATIONS_PER_ITERATION, sizeof(Pointer));
        for (size_t op = 1; op < NUM_OPERATIONS_PER_ITERATION; op++)
        {
            if ((double)rand() / RAND_MAX < ALLOC_PROBABILITY)
            {
                const size_t size = 1 + rand() % max_size;
                ptrs[op] = seg_alloc(alloc, size);
            }
            else
            {
                const size_t index = rand() % op;
                if (ptrs[index] != NULL)
                {
                    seg_free(alloc, ptrs[index]);
                    ptrs[index] = NULL;
                }
            }
        }
        // free any remaining allocated memory
        for (size_t op = 0; op < NUM_OPERATIONS_PER_ITERATION; op++)
            if (ptrs[op] != NULL) seg_free(alloc, ptrs[op]);
        free(ptrs);
    }

    return (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

void test_SegregatedStorage(void)
{
    printf(ANSI_COLOR_RED "TESTING SEGREGATED STORAGE ALLOCATOR..\n" ANSI_COLOR_RESET);
    double time_scenario1 = 0, time_scenario2 = 0, time_scenario3 = 0;

    allocator_ss alloc = seg_create();

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 1: Small Allocations
        time_scenario1 += run_seg(alloc, MIN_SIZE);
    printf("\tScenario 1: Small Allocations\n\tElapsed time: %.4f seconds\n\n", time_scenario1/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 2: Varying Allocation Sizes
        time_scenario2 += run_seg(alloc, MID_SIZE);
    printf("\tScenario 2: Varying Allocation Sizes\n\tElapsed time: %.4f seconds\n\n", time_scenario2/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 3: Large Allocations
        time_scenario3 += run_seg(alloc, MAX_SIZE);
    printf("\tScenario 3: Large Allocations\n\tElapsed time: %.4f seconds\n", time_scenario3/RUN_TIMES);

    seg_destroy(alloc);  // clean up
}

double run_pool(const allocator_p alloc)
{
    const clock_t start_time = clock();

    for (size_t iter = 0; iter < ITERATION_NUM; iter++)
    {
        Pointer* ptrs = calloc(NUM_OPERATIONS_PER_ITERATION, sizeof(Pointer));
        for (size_t op = 1; op < NUM_OPERATIONS_PER_ITERATION; op++)
        {
            if ((double)rand() / RAND_MAX < ALLOC_PROBABILITY)
            {
                ptrs[op] = pool_alloc(alloc);
            }
            else
            {
                const size_t index = rand() % op;
                if (ptrs[index] != NULL)
                {
                    pool_free(alloc, ptrs[index]);
                    ptrs[index] = NULL;
                }
            }
        }
        // free any remaining allocated memory
        for (size_t op = 0; op < NUM_OPERATIONS_PER_ITERATION; op++)
            if (ptrs[op] != NULL) pool_free(alloc, ptrs[op]);
        free(ptrs);
    }

    return (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

void test_Pool(void)
{
    printf(ANSI_COLOR_RED "TESTING POOL ALLOCATOR..\n" ANSI_COLOR_RESET);
    double time_scenario1 = 0, time_scenario2 = 0, time_scenario3 = 0;

    allocator_p alloc1 = pool_create(ALLOC_SIZE, MIN_SIZE);
    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 1: Small Allocations
        time_scenario1 += run_pool(alloc1);
    printf("\tScenario 1: Small Allocations\n\tElapsed time: %.4f seconds\n\n", time_scenario1/RUN_TIMES);

    allocator_p alloc2 = pool_create(ALLOC_SIZE, MID_SIZE);
    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 2: Varying Allocation Sizes
        time_scenario2 += run_pool(alloc2);
    printf("\tScenario 2: Varying Allocation Sizes\n\tElapsed time: %.4f seconds\n\n", time_scenario2/RUN_TIMES);

    allocator_p alloc3 = pool_create(ALLOC_SIZE, MAX_SIZE);
    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 3: Large Allocations
        time_scenario3 += run_pool(alloc3);
    printf("\tScenario 3: Large Allocations\n\tElapsed time: %.4f seconds\n", time_scenario3/RUN_TIMES);
}

double run_frl(const allocator_f alloc, const size_t max_size)
{
    const clock_t start_time = clock();

    for (size_t iter = 0; iter < ITERATION_NUM; iter++)
    {
        Pointer* ptrs = calloc(NUM_OPERATIONS_PER_ITERATION, sizeof(Pointer));
        for (size_t op = 1; op < NUM_OPERATIONS_PER_ITERATION; op++)
        {
            if ((double)rand() / RAND_MAX < ALLOC_PROBABILITY)
            {
                const size_t size = 1 + rand() % max_size;
                ptrs[op] = frl_alloc(alloc, size);
            }
            else
            {
                const size_t index = rand() % op;
                if (ptrs[index] != NULL)
                {
                    frl_free(alloc, ptrs[index]);
                    ptrs[index] = NULL;
                }
            }
        }
        // free any remaining allocated memory
        for (size_t op = 0; op < NUM_OPERATIONS_PER_ITERATION; op++)
            if (ptrs[op] != NULL) frl_free(alloc, ptrs[op]);
        free(ptrs);
    }

    return (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

void test_Free_List(void)
{
    printf(ANSI_COLOR_RED "TESTING FREE LIST ALLOCATOR..\n" ANSI_COLOR_RESET);
    double time_scenario1 = 0, time_scenario2 = 0, time_scenario3 = 0;

    allocator_f alloc = frl_create(ALLOC_SIZE);
    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 1: Small Allocations
        time_scenario1 += run_frl(alloc, MIN_SIZE);
    printf("\tScenario 1: Small Allocations\n\tElapsed time: %.4f seconds\n\n", time_scenario1/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 2: Varying Allocation Sizes
        time_scenario2 += run_frl(alloc, MID_SIZE);
    printf("\tScenario 2: Varying Allocation Sizes\n\tElapsed time: %.4f seconds\n\n", time_scenario2/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 3: Large Allocations
        time_scenario3 += run_frl(alloc, MAX_SIZE);
    printf("\tScenario 3: Large Allocations\n\tElapsed time: %.4f seconds\n", time_scenario3/RUN_TIMES);
}

double run_stack(const allocator_s alloc, const size_t max_size)
{
    const clock_t start_time = clock();

    for (size_t iter = 0; iter < ITERATION_NUM; iter++)
    {
        Pointer* ptrs = calloc(NUM_OPERATIONS_PER_ITERATION, sizeof(Pointer));
        for (size_t op = 1; op < NUM_OPERATIONS_PER_ITERATION; op++)
        {
            const size_t size = 1 + rand() % max_size;
            ptrs[op] = st_alloc(alloc, size);

            // modified: always free the previous allocation
            st_free(alloc, ptrs[op]);
        }
        free(ptrs);
    }

    return (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

void test_stack(void)
{
    printf(ANSI_COLOR_RED "TESTING STACK ALLOCATOR..\n" ANSI_COLOR_RESET);
    double time_scenario1 = 0, time_scenario2 = 0, time_scenario3 = 0;

    allocator_s alloc = st_create(ALLOC_SIZE);
    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 1: Small Allocations
        time_scenario1 += run_stack(alloc, MIN_SIZE);
    printf("\tScenario 1: Small Allocations\n\tElapsed time: %.4f seconds\n\n", time_scenario1/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 2: Varying Allocation Sizes
        time_scenario2 += run_stack(alloc, MID_SIZE);
    printf("\tScenario 2: Varying Allocation Sizes\n\tElapsed time: %.4f seconds\n\n", time_scenario2/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 3: Large Allocations
        time_scenario3 += run_stack(alloc, MAX_SIZE);
    printf("\tScenario 3: Large Allocations\n\tElapsed time: %.4f seconds\n", time_scenario3/RUN_TIMES);

    st_destroy(alloc); // clean up
}

double run_malloc(const size_t max_size)
{
    const clock_t start_time = clock();

    for (size_t iter = 0; iter < ITERATION_NUM; iter++)
    {
        Pointer* ptrs = calloc(NUM_OPERATIONS_PER_ITERATION, sizeof(Pointer));
        for (size_t op = 1; op < NUM_OPERATIONS_PER_ITERATION; op++)
        {
            if ((double)rand() / RAND_MAX < ALLOC_PROBABILITY)
            {
                const size_t size = 1 + rand() % max_size;
                ptrs[op] = malloc(size);
            }
            else
            {
                const size_t index = rand() % op;
                if (ptrs[index] != NULL)
                {
                    free(ptrs[index]);
                    ptrs[index] = NULL;
                }
            }
        }
        // free any remaining allocated memory
        for (size_t op = 0; op < NUM_OPERATIONS_PER_ITERATION; op++)
            if (ptrs[op] != NULL) free(ptrs[op]);
        free(ptrs);
    }

    return (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

void test_malloc(void)
{
    printf(ANSI_COLOR_RED "TESTING MALLOC..\n" ANSI_COLOR_RESET);
    double time_scenario1 = 0, time_scenario2 = 0, time_scenario3 = 0;

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 1: Small Allocations
        time_scenario1 += run_malloc(MIN_SIZE);
    printf("\tScenario 1: Small Allocations\n\tElapsed time: %.4f seconds\n\n", time_scenario1/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 2: Varying Allocation Sizes
        time_scenario2 += run_malloc(MID_SIZE);
    printf("\tScenario 2: Varying Allocation Sizes\n\tElapsed time: %.4f seconds\n\n", time_scenario2/RUN_TIMES);

    for (size_t i = 0; i < RUN_TIMES; i++)  // Scenario 3: Large Allocations
        time_scenario3 += run_malloc(MAX_SIZE);
    printf("\tScenario 3: Large Allocations\n\tElapsed time: %.4f seconds\n", time_scenario3/RUN_TIMES);
}
