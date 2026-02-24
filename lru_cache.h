#ifndef LRU_CACHE_H__
#define LRU_CACHE_H__

#include <unordered_map>
#include <functional>
#include <stdexcept>

class lru_cache
{
    public:
        lru_cache( 
                const size_t max_entries,
                const std::function< void (void*) >cleanup_func ) :
            max_size_(max_entries), 
            cleanup_memory_(cleanup_func) 
        {
            if( !cleanup_func )
            {
                throw std::invalid_argument(
                        "Cleanup Memory Function Cannot be NULL!");
            }
        }
    
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
        //
        // do not allow empty:
        // empty constructor
        // copy constructor
        // assignment operator.
        //
        lru_cache( void ) = delete;
        lru_cache( const lru_cache & ) = delete;
        lru_cache & operator= ( const lru_cache & ) = delete;

        std::unordered_map<int, void*> cache_;

        const size_t max_size_;
        const std::function< void (void*) > cleanup_memory_;
};



#endif /* #define LRU_CACHE_H__ */
