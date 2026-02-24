#if 0
LRU Cache

least recently used cache


key 'K' and value 'V'
number of entries is limited to N

if the number of entries exceeds N,
the least recently used keys would be
removed from the cache

** Freds assumption - we remove the cache entry value, not just the lru key(s)

A usage is considered whenever we 'put' or 'get' a key

cache operations are
init(N)
put(K,V)
get(K) ---> V

for the sake of implementation, we can
use 'int' type for keys and 'void*' type
for values.

*** Freds assumption - the 'void*' is actually going to be something, 
    we are really storing some sort of value that has been allocated
    somewhere previously, like a string or a node of some sort.

get() returns NULL if the key is not found.

for example
lru_init(&lru, 3)
lru_put(&lru, 1, ...) // adds key 1
lru_put(&lru, 2, ...) // adds key 2
lru_put(&lru, 3, ...) // adds key 3

lru_get(&lru, 1); // returns value for key 1
lru_put(&lru, 4); // removes key 2
lru_put( &lru, 5); // removes key 3

the actual cache (key/value pairs) are going to be an unordered_map

to keep the max size, my thought is maintain some sort of secondary
data struct that holds the key value and a timestamp value.

probably getting pretty involved here.

when inserting into the cache, check max size
---------------------------------------------------------
1) easy case, we have space, just insert key/value into the cache.
    then insert key/timestamp into secondary structure.

2) medium insert case, get() or update() cache entry value for key.

3) hard insert case, max size exceeded, key does not match, 
    and we have to remove the oldest entry.
----------------------------------------------------------------------
#endif

#include "lru_cache.h"


//
// FJP FIXME:
// this seems troublesome.
// max_size_ should be const and forced to be
// set from a constructor argument so the max size
// cannot be changed once the cache has entries.
// default constuctor should just be set to =delete
// copy constructor should also be invalidated
// only constructor with size should be allowed.
//
void lru_cache::lru_init( const size_t max_entries )
{
    max_size_ = max_entries;
}

void* lru_cache::lru_get( const int key )
{
    if( !cache_.empty() )
    {
        std::unordered_map<int, void*>::const_iterator iter = cache_.find(key);

        if( iter != cache_.end() )
        {
            return iter->second; 
        }
    }

    return nullptr;
}

void lru_cache::lru_put( const int key, void* value )
{
    cache_.insert_or_assign(key, value);
}


void* lru_cache::lru_remove( const int key )
{
    if( !cache_.empty() )
    {
        std::unordered_map<int, void*>::const_iterator iter = cache_.find(key);

        if( iter != cache_.end() )
        {
            void * rval = iter->second;
            cache_.erase(iter);
            return rval; 
        }
    }

    return nullptr;
}


