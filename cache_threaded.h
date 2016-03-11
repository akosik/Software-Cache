#pragma once
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "lru_threaded.h"
#include <pthread.h>

/*
The user must define an evicition struct that holds an add and delete function
pointer in it. They can also define an add or delete function and pass them to
the cache create function.  If the user does not wish to implment an eviction
policy then they should include the lru header and link with lru.c.
*/

//Key and Value Type
typedef const uint8_t *keyy_t;
typedef const void *val_t;

//Forward Declaration of cache object
struct cache_obj;
typedef struct cache_obj *cache_t;

// For a given key string, return a pseudo-random integer:
typedef uint64_t (*hash_func)(keyy_t key);

struct memtex
{
  size_t memsize;
  pthread_mutex_t mutex;
};

struct lentex
{
  size_t length;
  pthread_mutex_t mutex;
};

struct capex
{
  size_t capacity;
  pthread_mutex_t mutex;
};

struct cache_obj
{
  //The list of key-value pairs
  pair *dict;
  //The total possible size of the dict, following c++ naming conventions
  struct capex *cap;
  //Number of keys in the cache
  struct lentex *len;
  //Size taken up by values
  struct memtex *mem;
  //hash function
  hash_func hash;
  //eviction struct
  evict_class *evict;
};

typedef struct threadi
{
  cache_t cache;
  keyy_t key;
  val_t val;
  size_t val_size;
} tinfo;

typedef struct cache_query
{
  pthread_t thread;
  tinfo *tinfo;
} cachequery;

void *changeval(void *cacheval, const void *newval, size_t val_size);

// Create a new cache object with a given maximum memory capacity.
cache_t create_cache(uint64_t maxmem, hash_func hash, add_func add, remove_func remove);

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(void *arg);

// Retrieve the value associated with key in the cache, or NULL if not found
val_t cache_get(void *arg);

// Delete an object from the cache, if it's still there
void cache_delete(void *arg);

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(void *arg);

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache);
