//LRU Eviction Algorithm Implementation

#include <sys/types.h>
#include <stdlib.h>

typedef struct pair_t
{
  uint8_t *key;
  void *val;
  uint32_t size;
  struct pair_t *prev;
  struct pair_t *next;
  pthread_mutex_t *mutex;
} pair;

struct evict_t;
typedef void (*add_func)(struct evict_t *e, pair *kv);
typedef size_t (*remove_func)(struct evict_t *e);

typedef struct evict_t {
  pair *lrupair;
  pair *mrupair;
  add_func add;
  remove_func remove;
} evict_class;

void lru_add(evict_class *e, pair *kv);

size_t lru_remove(evict_class *e);
