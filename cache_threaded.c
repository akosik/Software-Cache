//Homework 4: Software Cache
//Alec Kosik

#include <stdio.h>
#include "cache_threaded.h"


// Default hash known as djb2
uint64_t defaultHash(uint8_t *str)
{
  uint64_t hash = 5381;
  uint8_t c;

  while (c = *str++)
    hash = ((hash << 5) + hash) + c;

  return hash;
}

// helper for modifying the value of a key-value pair
void *changeval(void *cacheval, const void *newval, size_t val_size)
{
  if (cacheval != NULL) free(cacheval);
  cacheval = malloc(val_size);
  if(cacheval == NULL) exit(1);
  return memcpy(cacheval,newval,val_size);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cache Operations
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Allocates a block of memory twice the size of the original and copies
// all data into the new cache
/*
void cache_resize(cache_t cache)
{
  pair* newdict = calloc(cache->cap->capacity * 2,sizeof(pair));
  if (newdict == NULL) exit(1);
  memcpy(newdict,cache->dict,cache->cap->capacity);
  free(cache->dict);
  cache->dict = newdict;
  cache->cap->capacity *= 2;
}
*/

// Create a new cache object with a given maximum memory capacity.
// Pass NULL for last 3 functions for defaults
cache_t create_cache(uint64_t maxmem, hash_func hash, add_func add, remove_func remove)
{
  cache_t cache = malloc(sizeof(struct cache_obj));
  if (cache == NULL) exit(1);
  cache->dict = calloc(maxmem,sizeof(pair));
  if (cache->dict == NULL) exit(1);
  uint64_t i = 0;
  for( ; i < maxmem; ++i) pthread_mutex_init(cache->dict[i].mutex);
  cache->evict = calloc(1,sizeof(evict_class));
  if (cache->evict == NULL) exit(1);
  cache->cap->capacity = maxmem;
  cache->len->length = 0;
  cache->mem->memsize = 0;

  if(hash == NULL) cache->hash = defaultHash;
  else cache->hash = hash;

  if(add == NULL) cache->evict->add = lru_add;
  else cache->evict->add = add;

  if(remove == NULL) cache->evict->remove = lru_remove;
  else cache->evict->remove = remove;

  return cache;
}

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(void *arg)
{
  cachequery *query = arg;
  tinfo *q = query->tinfo;
  cache_t cache = q->cache;
  keyy_t key = q->key;
  val_t val = q->val;
  size_t val_size = q->val_size;

  pair *current;
  // Remove values until the cache has enough room for the new value
  pthread_mutex_lock(cache->mem->mutex);
  while(cache->mem->memsize + val_size > cache->cap->capacity)
    {
      size_t size = cache->evict->remove(cache->evict);
      cache->mem->memsize -= size;
      --cache->len->length;
    }
  pthread_mutex_unlock(cache->mem->mutex);

  uint64_t hashval = cache->hash(key) % cache->cap->capacity;
  // hash the key and perform linear probing until an open spot is found.
  // Since the cache resizes, the cache should never be full.
  for(current = &cache->dict[hashval]; current->key != NULL; current = &cache->dict[++hashval % cache->cap->capacity])
    {
      pthread_mutex_lock(current->mutex);
      //if the keys match, replace that pair
      if(!strcmp(current->key,key))
        {
          current->val = changeval(current->val,val,val_size);
          pthread_mutex_lock(cache->mem->mutex);
          cache->mem->memsize -= current->size;
          cache->mem->memsize += val_size;
          pthread_mutex_unlock(cache->mem->mutex);
          current->size = val_size;
          cache->evict->add(cache->evict,current);
          return;
        }
      pthread_mutex_unlock(current->mutex);
    }
  //if you found an open spot, save the pair there
  pthread_mutex_lock(current->mutex);
  current->key = malloc(sizeof(uint8_t) * strlen(key));
  strcpy(current->key,key);
  current->val = changeval(current->val,val,val_size);

  pthread_mutex_lock(cache->len->mutex);
  ++cache->len->length;
  pthread_mutex_unlock(cache->len->mutex);

  current->size = val_size;

  pthread_mutex_lock(cache->mem->mutex);
  cache->mem->memsize += val_size;
  pthread_mutex_unlock(cache->mem->mutex);

  cache->evict->add(cache->evict,current);
  pthread_mutex_unlock(current->mutex);

  /*
//resize if over half full
  if((cache->len->length / (float)cache->cap->capacity) > .5) cache_resize(cache);
  */
}

// Retrieve the value associated with key in the cache, or NULL if not found
val_t cache_get(void *arg)
{
  cachequery *query = arg;
  tinfo *q = query->tinfo;
  cache_t cache = q->cache;
  keyy_t key = q->key;

  //hash and check for a key match, else return NULL
  uint64_t n = 0;
  pair *current;
  uint64_t hashval = cache->hash(key) % cache->cap->capacity;
  for(; n < cache->cap->capacity; hashval = ++hashval % cache->cap->capacity)
    {
      current = &cache->dict[hashval];
      pthread_mutex_lock(current->mutex);
      if(current->key != NULL)
        {
          if(!strcmp(current->key,key))
            {
              cache->evict->add(cache->evict,current);
              return current->val;
            }
        }
      pthread_mutex_unlock(current->mutex);
      ++n;
    }
  return NULL;
}

// Delete an object from the cache, if it's still there
void cache_delete(void *arg)
{
  cachequery *query = arg;
  tinfo *q = query->tinfo;
  cache_t cache = q->cache;
  keyy_t key = q->key;

  uint64_t n = 0;
  pair *current;
  uint64_t hashval = cache->hash(key) % cache->cap->capacity;
  for(; n < cache->cap->capacity; hashval = ++hashval % cache->cap->capacity)
    {
      current = &cache->dict[hashval];
      pthread_mutex_lock(current->mutex);
      if(current->key != NULL)
        {
          if(!strcmp(current->key,key))
            {
              free(current->key);
              free(current->val);
              current->key = NULL;
              current->val = NULL;
              if(current->prev != NULL)
                {
                  pthread_mutex_lock(current->prev->mutex);
                  current->prev->next = current->next;
                  pthread_mutex_unlock(current->prev->mutex);
                }
              if(current->next != NULL)
                {
                  pthread_mutex_lock(current->prev->mutex);
                  current->next->prev = current->prev;
                  pthread_mutex_unlock(current->prev->mutex);
                }
              pthread_mutex_lock(cache->len->mutex);
              --cache->len->length;
              pthread_mutex_unlock(cache->len->mutex);

              pthread_mutex_lock(cache->mem->mutex);
              cache->mem->memsize -= current->size;
              pthread_mutex_unlock(cache->mem->mutex);

              return;
            }
        }
      pthread_mutex_unlock(current->mutex);
      ++n;
    }
}

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(void *arg)
{
  cachequery *query = arg;
  tinfo *q = query->tinfo;
  cache_t cache = q->cache;

  pthread_mutex_lock(cache->mem->memsize);
  size_t memsize = cache->mem->memsize;
  pthread_mutex_unlock(cache->mem->memsize);
}

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache)
{
  free(cache);
  cache = NULL;
}
