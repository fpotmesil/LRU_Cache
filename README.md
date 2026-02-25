# LRU_Cache
play LRU Cache

LRU Cache

least recently used cache


key 'K' and value 'V'
number of entries is limited to N

if the number of entries exceeds N,
the least recently used keys would be
removed from the cache

** Freds assumption - we remove the cache entry value, not just the lru key(s)
    This will mean allowing caller to free memory, we need a callback to free
    memory when a put() causes us to remove an old element **

A usage is considered whenever we 'put' or 'get' a key

cache operations are
```C++
init(N)
put(K,V)
get(K) ---> V
```

for the sake of implementation, we can
use ```C++ 'int' ``` type for keys and ```C++ 'void*' ``` type
for values.

** Freds assumption - the ```C++ 'void*' ``` is actually going to be something, 
    we are really storing some sort of value that has been allocated
    somewhere previously, like a string or a node of some sort. **

get() returns NULL if the key is not found.

for example
```C++
lru_init(&lru, 3)
lru_put(&lru, 1, ...) // adds key 1
lru_put(&lru, 2, ...) // adds key 2
lru_put(&lru, 3, ...) // adds key 3

lru_get(&lru, 1); // returns value for key 1
lru_put(&lru, 4); // removes key 2
lru_put( &lru, 5); // removes key 3
```
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

