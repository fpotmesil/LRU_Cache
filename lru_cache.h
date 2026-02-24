#ifndef LRU_CACHE_H__
#define LRU_CACHE_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>

struct TimeSpecCmp
{
    bool operator()(
            const struct timespec& lhs, 
            const struct timespec& rhs) const
    {
        if( lhs.tv_sec != rhs.tv_sec )
        {
            return lhs.tv_sec < rhs.tv_sec;
        }

        return lhs.tv_nsec < rhs.tv_nsec;
    }
};

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
    
        // 
        // changed lru_init to a no-op for now.
        // eventually we could put it back in and resize the cache
        // now that we have a deleter callback so memory can be cleaned.
        //
        void lru_init( const size_t max_entries );

        //
        // get the value stored at key
        //
        void* lru_get( const int key );

        //
        // put a new value or update an existing value.
        // if the max size is exceeded, remove the oldest value
        // and call the cleanup_memory_ callback to clear any
        // memory with user-passed function.
        //
        void lru_put( const int key, void* value );

        //
        // memory leaks bother me even for fun tests
        // remove the entry from the cache and return it
        // so it can be freed.
        //
        void* lru_remove( const int key );

        //
        // remove all the entries and call the callback to clear 
        // any allocated memory
        //
        void lru_clear( void );

    private:
        //
        // do not allow:
        // empty constructor
        // copy constructor
        // assignment operator.
        //
        lru_cache( void ) = delete;
        lru_cache( const lru_cache & ) = delete;
        lru_cache & operator= ( const lru_cache & ) = delete;

        void update_key_in_map( const int key );
        int remove_oldest_timestamp_entry( void );



        //
        // map to store nanosecond insert/update timestamps
        // and the key that was last accessed/updated at time x.
        //
        std::map<struct timespec, int, TimeSpecCmp> lru_timestamp_map_;

        //
        // fast access to my timestamp map by key
        //
        std::unordered_map<int, struct timespec> lru_key_map_;

        //
        // actual cache is stored in a hash map
        //
        std::unordered_map<int, void*> cache_;

        //
        // maximum number or cache entries
        //
        const size_t max_size_;

        //
        // callback for user to set to something to clear any allocated memory
        // that is used to store the void*
        //
        // FJP TODO - make the cache dynamic so we can store any void* in any
        // cache node and call a set callback for whatever value type.
        //
        const std::function< void (void*) > cleanup_memory_;
};

typedef std::unordered_map<int, struct timespec>::const_iterator key_map_citer;
typedef std::map<struct timespec, int, TimeSpecCmp>::const_iterator timestamp_map_citer;




#endif /* #define LRU_CACHE_H__ */
