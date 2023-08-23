#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> // memcpy
#include <unistd.h>
#include "segregated.h"


// 4 KB
#define FKB 4096

#define PAGESIZE FKB

// 1MB = 256 pages
#define ONEMB 1048576

#define POOLSIZE ONEMB*POOLMB


#define SIZES_NUM 11
static const size_t pool_sizes[] = { 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, PAGESIZE };


// store memory addresses
struct _list_ptr
{
    Pointer page;
    struct _list_ptr* next;
}; typedef struct _list_ptr* list_ptr;

// pool & page representation
struct _pool
{
    Pointer curr;
    Pointer end;
}; typedef struct _pool* Page;

typedef struct hash_table* HashTable;

struct _seg_allocator
{
    Page mpool;                      // current pool
    HashTable pages;                 // page -> pool size
    Page page[SIZES_NUM];            // store a page for each pool size
    list_ptr free_pages[SIZES_NUM];  // list of free space for each pool size
};


//////////////////////////////////////////////////////////////////////////////////////////////
// =============================== Hash Table ===============================================
//////////////////////////////////////////////////////////////////////////////////////////////
// source: a slightly modified version of https://github.com/pavlosdais/Abstract-Data-Types/tree/main/modules/HashTable/SeparateChaining#readme

// when max load factor is exceeded, rehashing operation occurs
#define MAX_LOAD_FACTOR 0.8

// bucket
typedef struct node
{
    Pointer key;          // pointer to the key we are storing
    char data;            // pointer to the data we are storing

    uint32_t hash_value;  // hash value of the data
    struct node* next;    // next element in the bucket (NULL if it's the last)
}
node;

typedef struct hash_table
{
    node** buckets;     // buckets (lists) storing the data
    uint8_t capacity;   // the capacity of the hash table - index to the "hash_sizes" array
    uint64_t elements;  // number of elements in the hash table
}
hash_table; 

// available number of buckets, preferably prime numbers since it has been proven they have better behavior
static const uint64_t hash_sizes[] =
    { 16369, 32749, 65521, 131071, 262147, 524287, 1048573, 2097143, 4194301, 8388593, 16777213, 33554467, 67108879, 134217757,
    268435459, 536870923, 1073741827, 2147483647, 4294967291, 8589934583, 17179869143, 34359738337, 68719476731, 137438953447, 
    274877906899, 549755813881, 1099511627689, 2199023255531, 4398046511093, 8796093022151, 17592186044399, 35184372088777,
    70368744177643, 140737488355213, 281474976710597, 562949953421231, 1125899906842597, 2251799813685119, 4503599627370449,
    9007199254740881, 18014398509481951, 36028797018963913, 72057594037927931, 144115188075855859, 288230376151711717,
    576460752303423433, 1152921504606846883, 2305843009213693951, 4611686018427387847 };

#define get_hash(i) (hash_sizes[i])
#define COMPARE_PTR(v1, v2) ((size_t)v1-(size_t)v2)

// function prototypes
static inline void insert(const HashTable, const Pointer, const char, const uint32_t);
static inline void rehash(const HashTable);
static inline char hash_search(const HashTable, const Pointer, uint32_t*);

static inline unsigned int hash_ptr(const Pointer value)
{
    size_t val = (size_t)value;
    val ^= val >> 16;
    val *= 0x7feb352d;
    val ^= val >> 15;
    val *= 0x846ca68b;
    val ^= val >> 16;
    return (unsigned int)val;
}

static HashTable hash_create(void)
{    
    HashTable ht = malloc(sizeof(hash_table));
    assert(ht != NULL);  // allocation failure

    ht->capacity = 0;
    ht->buckets = calloc(sizeof(node), get_hash(ht->capacity));  // allocate memory for the buckets
    assert(ht->buckets != NULL);  // allocation failure

    ht->elements = 0;
    return ht;
}

static inline uint64_t hash_size(const HashTable ht)
{
    assert(ht != NULL);
    return ht->elements;
}

static inline bool hash_insert(const HashTable ht, const Pointer key, const char value)
{
    // check to see if value already exists in the hash table
    const uint32_t hash_value = hash_ptr(key);

    if (((float)ht->elements / get_hash(ht->capacity)) > MAX_LOAD_FACTOR)  // max load factor exceeded, try to rehash
    {
        if (get_hash(ht->capacity) != hash_sizes[sizeof(hash_sizes) / sizeof(hash_sizes[0]) - 1])  // if a new, available, size exists
            rehash(ht);  // rehash
    }
    
    // insert value
    insert(ht, key, value, hash_value);

    ht->elements++;  // value inserted, increment the number of elements in the hash table
    return true;
}

static inline void insert(const HashTable ht, const Pointer key, const char value, const uint32_t hash_value)
{
    // create new bucket (list)
    node* new_node = malloc(sizeof(node));
    assert(new_node != NULL);  // allocation failure

    // fill the node's contents
    new_node->key = key;
    new_node->data = value;
    new_node->hash_value = hash_value;

    // insert value at the start of the bucket
    const uint32_t bucket = hash_value % get_hash(ht->capacity);
    new_node->next = ht->buckets[bucket];
    ht->buckets[bucket] = new_node;
}

static void rehash(HashTable ht)
{
    node** old_buckets = ht->buckets;  // save previous buckets

    const uint8_t old_capacity = ht->capacity;
    (ht->capacity)++;  // get next size

    // create the new number of buckets
    ht->buckets = calloc(sizeof(node), get_hash(ht->capacity));
    assert(ht->buckets != NULL);  // allocation failure

    // start rehash operation
    for (uint64_t i = 0; i < get_hash(old_capacity); i++)
    {
        node* bkt = old_buckets[i];
    
        while (bkt != NULL)
        {
            node* tmp = bkt;
            bkt = bkt->next;
            
            insert(ht, tmp->key, tmp->data, tmp->hash_value);

            free(tmp);
        }
    }
    free(old_buckets);
}

static inline char hash_exists(const HashTable ht, const Pointer key)
{
    uint32_t tmp = 0;
    return hash_search(ht, key, &tmp);
}

static inline char hash_search(const HashTable ht, const Pointer key, uint32_t* hash_value)
{   
    *hash_value = hash_ptr(key);
    node* bkt = ht->buckets[*hash_value % get_hash(ht->capacity)];
    
    // search for the value in the bucket h
    while (bkt != NULL)
    {
        if (COMPARE_PTR(key, bkt->key) == 0)  // value found
            return bkt->data;
        
        bkt = bkt->next;
    }

    return -1;
}

static void hash_destroy(const HashTable ht)
{
    // destroy the buckets
    if (ht->elements != 0)
    {
        for (uint64_t i = 0 ;; i++)
        {
            node* bkt = ht->buckets[i];
            while (bkt != NULL)
            {
                node* tmp = bkt;
                bkt = bkt->next;
                
                free(tmp);
                --(ht->elements);
            }
            if (ht->elements == 0) break;  // all elements deleted
        }
    }
    
    free(ht->buckets);
    free(ht);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ==================================== List ================================================
//////////////////////////////////////////////////////////////////////////////////////////////

// use sbrk to allocate memory
static inline Pointer allocate_memory(const size_t size)
{
    const Pointer memory = sbrk(size);
    if (memory == (Pointer)-1) return NULL;
    return memory;
}

static inline void insert_page(list_ptr* S, const Pointer value)
{
    const list_ptr new_node = malloc(sizeof(*new_node));
    assert(new_node != NULL);  // allocation failure
    new_node->page = value;
    new_node->next = *S;
    *S = new_node;
}

static inline Pointer delete_page(list_ptr* S)
{
    const Pointer data = (*S)->page;
    const list_ptr tmp = *S;
    (*S) = (*S)->next;
    free(tmp);
    return data;
}

#define is_list_empty(S) (S == NULL)

static void destoy_pages(list_ptr S)
{
    while (S != NULL)
    {
        const list_ptr tmp = S;
        S = S->next;
        free(tmp);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// =================================== Memory ===============================================
//////////////////////////////////////////////////////////////////////////////////////////////

// memory is PAGESIZE-aligned so we find the previous number of addr, divisible by the PAGESIZE,
// to find the page the addr potentially belongs to
static inline Pointer search_page(const Pointer addr)
{
    const size_t remainder = (size_t)addr % PAGESIZE;
    if (remainder == 0) return addr;
    return addr - remainder;
}

// allocates PAGESIZE-aligned memory
static inline Pointer allocate_aligned_memory(void)
{
    const Pointer unaligned_memory = allocate_memory(POOLSIZE + PAGESIZE - 1);
    const Pointer aligned_memory = (Pointer)(((uintptr_t)unaligned_memory + (PAGESIZE - 1)) & ~(PAGESIZE - 1));
    return aligned_memory;
}

// resets pool
static inline void mem_pool_reset(const allocator_ss alloc)
{
    const Pointer page = allocate_aligned_memory();
    alloc->mpool->curr = page;
    alloc->mpool->end = alloc->mpool->curr + (size_t)POOLSIZE;
}

// creates pool
static void mem_pool_create(const allocator_ss alloc)
{
    alloc->mpool = allocate_memory(sizeof(*alloc->mpool));
    assert(alloc->mpool != NULL);
    mem_pool_reset(alloc);
}

// routine that gets a page from the pool
static inline Pointer get_page(const allocator_ss alloc)
{
    // ran out of memory in the current pool, get a new pool
    if ((size_t)alloc->mpool->curr + PAGESIZE > (size_t)alloc->mpool->end)
        mem_pool_reset(alloc);
    
    const Pointer page = alloc->mpool->curr;
    alloc->mpool->curr += PAGESIZE;
    return page;
}

// rounds up a size given to the nearest number in the pool_sizes array
// eg 57 -> 64, 25 -> 32
static inline size_t find_nearest_size(const size_t size)
{
    for (size_t i = 0;; i++)
        if (size <= pool_sizes[i]) return i;
}

// creates page and a page->pool_index(and thus page size) relationship
static inline Pointer create_page(const allocator_ss alloc, const size_t pool_index)
{
    // get page
    const Pointer page = get_page(alloc);

    // insert it at the hash table
    hash_insert(alloc->pages, page, (char)pool_index);

    return page;
}

// finds memory for the pool_index size memory
// first searches for memory inside its free_pages list
// if empty gets memory from its page
static Pointer find_memory(const allocator_ss alloc, const size_t pool_index)
{
    // there are empty pool sizes in the list, get memory from it 
    if (!is_list_empty(alloc->free_pages[pool_index]))
        return delete_page(&alloc->free_pages[pool_index]);
    
    // no empty pool sizes in the list, get memory from the page
    // empty page - create a new one 
    if (alloc->page[pool_index]->curr == NULL || alloc->page[pool_index]->curr + pool_sizes[pool_index] > alloc->page[pool_index]->end)
    {
        alloc->page[pool_index]->curr = create_page(alloc, pool_index);
        alloc->page[pool_index]->end = alloc->page[pool_index]->curr + PAGESIZE;
    }
    
    const Pointer memory = alloc->page[pool_index]->curr;
    alloc->page[pool_index]->curr += pool_sizes[pool_index];
    return memory;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ================================= Functions ==============================================
////////////////////////////////////////////////////////////////////////////////////////////// 

allocator_ss seg_create(void)
{
    const allocator_ss alloc = allocate_memory(sizeof(*alloc));
    assert(alloc != NULL);

    mem_pool_create(alloc);
    alloc->pages = hash_create();

    for (size_t i = 0; i < SIZES_NUM; i++)
    {
        alloc->page[i] = allocate_memory(sizeof(*(alloc->page[i])));
        alloc->page[i]->curr = NULL;
        alloc->free_pages[i] = NULL;
    }
    
    return alloc;
}

Pointer seg_alloc(const allocator_ss alloc, const size_t size)
{
    // if the requested object is bigger than a page, use malloc
    if (size > PAGESIZE)
        return malloc(size);
    
    if (!alloc) return NULL;

    return find_memory(alloc, find_nearest_size(size));
}

Pointer seg_realloc(const allocator_ss alloc, const Pointer obj,  const size_t size)
{
    if (!alloc || !obj) return NULL;
    
    // find the potential page the object belongs in
    const Pointer page = search_page(obj);
    const char stack_index = hash_exists(alloc->pages, page);

    // object does not belong in the allocator's pool
    if (stack_index == -1) return NULL;

    // requested object is bigger than a page, use malloc
    const size_t obj_size = pool_sizes[stack_index];
    if (size > PAGESIZE)
    {
        const Pointer new_object = malloc(size);
        memcpy(new_object, obj, obj_size);
        return new_object;
    }

    const Pointer new_object = seg_alloc(alloc, find_nearest_size(size));
    memcpy(new_object, obj, obj_size);
    insert_page(&alloc->free_pages[stack_index], obj);
    return new_object;
}

void seg_free(const allocator_ss alloc, const Pointer obj)
{
    if (!alloc || !obj) return;
    
    // find the potential page the object belongs in
    const Pointer page = search_page(obj);
    const char stack_index = hash_exists(alloc->pages, page);
    if (stack_index == -1)  // object does not belong in the allocator's pool
    {
        // probably malloc'd so try to free it - undefined behavior if not
        free(obj);
        return;
    }

    // insert the object to the free list
    insert_page(&alloc->free_pages[stack_index], obj);
}

void seg_destroy(const allocator_ss alloc)
{
    // destroy metadata
    hash_destroy(alloc->pages);

    for (size_t i = 0; i < SIZES_NUM; i++)
        destoy_pages(alloc->free_pages[i]);
}
