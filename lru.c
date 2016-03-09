//LRU implementation

#include "lru.h"

void lru_add(evict_class *e, pair *kv, uint64_t htable_index)
{
  if(e->mrupair != NULL)
    {
      kv->lru->next = e->mrupair;
      e->mrupair->prev = kv->lru;
    }
  else
    {
      kv->lru->next = NULL;
      e->lrupair = kv->lru;
    }
  e->mrupair = kv->lru;
  kv->lru->tabindex = htable_index;
}

uint64_t lru_remove(evict_class *e)
{
  if(e->lrupair == NULL)
    {
      printf("Value too large to be stored in cache.\n");
      exit(1);
    }

  uint64_t index = e->lrupair->tabindex;
  e->lrupair->tabindex = 0;

  if(e->lrupair->prev != NULL)
    {
      e->lrupair->prev->next = NULL;
      pair *newlru = e->lrupair->prev;
      e->lrupair->prev = NULL;
      e->lrupair = newlru;
    }
  else
    {
      e->lrupair = NULL;
      e->mrupair = NULL;
    }
  return index;
}
