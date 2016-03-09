#include <stdio.h>
#include "cache_threaded.h"
#include <assert.h>
#include <stdlib.h>

struct test_struct
{
  uint8_t *word;
  uint32_t num;
  uint8_t *stuff;
};

cache_t checkInit()
{
  cache_t cache = create_cache(255,NULL,NULL,NULL);
  printf("Test 0: Success!\n");
  return cache;
}

int checkSet(cache_t cache)
{
  keyy_t
    key0 = "hello",
    key1 = "thenumber3",
    key2 = "goodbye",
    key3 = "wow";
  uint8_t
    value0 = 1,
    value1 = 3;
  uint32_t value2 = 304;
  uint64_t value3 = 123123124;

  tinfo info = { .cache = cache, .key = key0, .val = &value0, .val_size = sizeof(uint8_t) };
  cachequery q = { .tinfo = &info };
  pthread_create(q.thread,NULL,cache_set,&q);
  printf("woo\n");
  /*cache_set(cache,key1,&value1,sizeof(uint8_t));
  cache_set(cache,key2,&value2,sizeof(uint32_t));
  cache_set(cache,key3,&value3,sizeof(uint64_t));*/

  assert(!strcmp(cache->dict[cache->hash(key0) % cache->cap->capacity].key,"hello"));
  /*assert(!strcmp(cache->dict[cache->hash(key1) % cache->capacity].key,"thenumber3"));
  assert(!strcmp(cache->dict[cache->hash(key2) % cache->capacity].key,"goodbye"));
  assert(!strcmp(cache->dict[cache->hash(key3) % cache->capacity].key,"wow"));*/

  uint8_t
    val1 = *(uint8_t*)cache->dict[cache->hash(key0)% cache->cap->capacity].val;
    /*val2 = *(uint8_t*)cache->dict[cache->hash(key1)% cache->capacity].val;
  uint32_t val3 = *(uint32_t*)cache->dict[cache->hash(key2)% cache->capacity].val;
  uint64_t val4 = *(uint64_t*)cache->dict[cache->hash(key3)% cache->capacity].val;*/

  assert(val1 == 1);
  /*assert(val2 == 3);
  assert(val3 == 304);
  assert(val4 == 123123124);*/

  printf("Test 1: Success!\n");
  return 1;
}
/*
int checkGetExistent(cache_t cache)
{
  uint64_t i;
    keyy_t
    key0 = "hello",
    key1 = "thenumber3",
    key2 = "goodbye",
    key3 = "wow";
  uint8_t
    value0 = 1,
    value1 = 3;
  uint32_t value2 = 304;
  uint64_t value3 = 123123124;

  uint8_t testval1 = *(uint8_t*)cache_get(cache,key0);
  uint8_t testval2 = *(uint8_t*)cache_get(cache,key1);
  uint32_t testval3 = *(uint32_t*)cache_get(cache,key2);
  uint64_t testval4 = *(uint64_t*)cache_get(cache,key3);

  if(testval1 != NULL) assert(testval1 == value0);
  if(testval2 != NULL) assert(testval2 == value1);
  if(testval3 != NULL) assert(testval3 == value2);
  if(testval4 != NULL) assert(testval4 == value3);

  printf("Test 2: Success!\n");
  return 1;
}

int checkGetModified(cache_t cache)
{
  keyy_t key = "hello";
  uint8_t val = 201;
  cache_set(cache,key,&val,sizeof(uint8_t));
  uint8_t testval = *(uint8_t*)cache_get(cache,key);
  assert(testval == 201);
  printf("Test 3: Success!\n");
  return 1;
}

int checkGetNonexistent(cache_t cache)
{
  keyy_t key = "sartre";
  assert(cache_get(cache,key) == NULL);
  printf("Test 4: Success!\n");
  return 1;
}

int checkDelete(cache_t cache)
{
  keyy_t key = "goodbye";
  cache_delete(cache,key);
  assert(cache_get(cache,key) == NULL);
  printf("Test 5: Success!\n");
  return 1;
}

int checkSize(cache_t cache)
{
  assert(cache_space_used(cache) == 10);
  printf("Test 6: Success!\n");
  return 1;
}

int checkResize(cache_t cache)
{
  uint8_t i;
  char key[1024] = "hello";
  size_t oldsize = cache->capacity;
  for(i = 0; i <= ((oldsize / 2) + 1); ++i)
    {
      strcat(key, "hi");
      cache_set(cache,key,&i,sizeof(uint8_t));
    }
  assert(cache->capacity == oldsize * 2);

  keyy_t
    key0 = "hello",
    key1 = "thenumber3",
    key2 = "hellohi",
    key3 = "hellohihihihihihihihihihihihihihihihihihihihihihihihihihihihihi";
  uint8_t
    value0 = 1,
    value1 = 3;
  uint64_t value2 = 0;
  uint64_t value3 = 28;

  if(cache->dict[cache->hash(key0) % cache->capacity].key != NULL)
    {
      uint8_t testval1 = *(uint8_t*)cache_get(cache,key0);
      assert(testval1 == value0);
    }
  if(cache->dict[cache->hash(key1) % cache->capacity].key != NULL)
    {
      uint8_t testval2 = *(uint8_t*)cache_get(cache,key1);
      assert(testval2 == value1);
    }
  if(cache->dict[cache->hash(key2) % cache->capacity].key != NULL)
    {
      uint32_t testval3 = *(uint64_t*)cache_get(cache,key2);
      assert(testval3 == value2);
    }
  if(cache->dict[cache->hash(key3) % cache->capacity].key != NULL)
    {
    uint64_t testval4 = *(uint64_t*)cache_get(cache,key3);
    if(testval4 != NULL) assert(testval4 == value3);
    }

  printf("Test 7: Success!\n");
  return 1;
}

int checkStructSet(cache_t cache)
{
  keyy_t keystruct = "struct";
  struct test_struct value4 = { .word = "it's a bag of words!", .num = 42, .stuff = NULL};

    cache_set(cache,keystruct,&value4,sizeof(struct test_struct));

    assert(!strcmp(cache->dict[cache->hash(keystruct)% cache->capacity].key,"struct"));

    uint8_t *val5 = (uint8_t*)((struct test_struct*) cache->dict[cache->hash(keystruct) % cache->capacity].val) ->word;
    uint32_t val6 = (uint32_t)((struct test_struct*) cache->dict[cache->hash(keystruct)% cache->capacity].val)  ->num;
    uint8_t *val7 = (uint8_t*)((struct test_struct*) cache->dict[cache->hash(keystruct) % cache->capacity].val) ->stuff;

    assert(!strcmp(val5,"it's a bag of words!"));
    assert(val6 == 42);
    assert(val7 == NULL);
    printf("Test 8: Success!\n");
    return 1;
}

int checkDestroy(cache_t cache)
{
  destroy_cache(cache);
  cache = NULL;
  printf("Test 9: Success!\n");
  return 1;
}

cache_t checkInitTiny()
{
  cache_t cache = create_cache(10,NULL,NULL,NULL);
  return cache;
}

int checkEviciton(cache_t cache)
{
  keyy_t
    key0 = "hello",
    key1 = "thenumber3",
    key2 = "goodbye",
    key3 = "wow";
  uint8_t
    value0 = 1,
    value1 = 3;
  uint32_t value2 = 304;
  uint64_t value3 = 123123124;

  cache_set(cache,key0,&value0,sizeof(uint8_t));
  cache_set(cache,key1,&value1,sizeof(uint8_t));
  cache_set(cache,key2,&value2,sizeof(uint32_t));
  cache_set(cache,key3,&value3,sizeof(uint64_t));

  assert(!strcmp(cache->dict[cache->hash(key3) % cache->capacity].key,"wow"));

  uint64_t val4 = *(uint64_t*)cache->dict[cache->hash(key3)% cache->capacity].val;

  assert(val4 == 123123124);
  printf("Test 10: Success!\n");
  return 1;
}
*/
int main(int argc, char** argv)
{
  cache_t cache = checkInit();
  checkSet(cache);
  /*checkGetExistent(cache);
  checkGetModified(cache);
  checkGetNonexistent(cache);
  checkDelete(cache);
  checkSize(cache);
  checkResize(cache);
  checkStructSet(cache);
  checkDestroy(cache);
  cache_t cache2 = checkInitTiny();
  checkEviciton(cache2);*/
  return 0;
}
