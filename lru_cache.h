#ifndef LRU_CACHE_H__
#define LRU_CACHE_H__

#include <unordered_map>

class lru_cache
{
    public:
        lru_cache( void ) : max_size_(0) {}
        lru_cache( const size_t max_entries ) : max_size_(max_entries) {}
    
        void lru_init( const size_t max_entries );
        void* lru_get( const int key );
        void lru_put( const int key, void* value );
        //
        // memory leaks bother me even for fun tests
        // remove the entry from the cache and return it
        // so it can be freed.
        //
        void* lru_remove( const int key );




    private:
        std::unordered_map<int, void*> cache_;
        size_t max_size_;
};



#endif /* #define LRU_CACHE_H__ */
