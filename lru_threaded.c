//LRU implementation

#include "lru_threaded.h"

void lru_add(evict_class *e, pair *kv)
{
  if(e->mrupair != NULL)
    {
      kv->next = e->mrupair;
      e->mrupair->prev = kv;
    }
  else
    {
      kv->next = NULL;
      e->lrupair = kv;
    }
  e->mrupair = kv;
}

size_t lru_remove(evict_class *e)
{
  if(e->lrupair->prev != NULL) e->lrupair->prev->next = NULL;
  free(e->lrupair->key);
  e->lrupair->key = NULL;
  free(e->lrupair->val);
  e->lrupair->val = NULL;
  size_t size = e->lrupair->size;
  e->lrupair->size = NULL;
  if(e->lrupair->prev != NULL)
    {
      pair *newlru = e->lrupair->prev;
      e->lrupair->prev = NULL;
      e->lrupair = newlru;
    }
  else
    {
      e->lrupair = NULL;
      e->mrupair = NULL;
    }
  return size;
}
