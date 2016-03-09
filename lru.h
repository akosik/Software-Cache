//LRU Eviction Algorithm Implementation

#include <stdlib.h>

typedef struct node_t
{
  uint64_t tabindex;
  struct node_t *prev;
  struct node_t *next;
}* node;

typedef struct pair_t
{
  uint8_t *key;
  void *val;
  uint32_t size;
  node lru;
} pair;

struct evict_t;
typedef void (*add_func)(struct evict_t *e, pair *kv,uint64_t htableindex);
typedef size_t (*remove_func)(struct evict_t *e);

typedef struct evict_t {
  node lrupair;
  node mrupair;
  add_func add;
  remove_func remove;
} evict_class;

void lru_add(evict_class *e, pair *kv, uint64_t hashtable_index);

uint64_t lru_remove(evict_class *e);
