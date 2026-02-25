
#include <iostream>

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
    std::cout << "Cannot set LRU Cache size to " << max_entries 
        << "\nThe lru_cache lru_init function has been disabled.\n" 
        << "Please initialize the maximum cache entry size in the constructor"
        << std::endl;

    (void)(max_entries);
    // max_size_ = max_entries;
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


//
// first get the oldest timestamp from the timestamp->key map - 
// sorted in reverse order, should just be begin()
//
// grab the iterator and get the key for the timestamp
// the key has to be removed from the key->timestamp map
//
int lru_cache::remove_oldest_timestamp_entry( void )
{
    int rval = -1;

    if( !lru_timestamp_map_.empty() )
    {
        auto iter = lru_timestamp_map_.begin();

        std::cout << "Oldest timestamp map entry timestamp: " 
            << iter->first.tv_sec << "." 
            << iter->first.tv_nsec << ", key: "
            << iter->second << '\n';

        int key = iter->second;
        rval = key;
        lru_timestamp_map_.erase(iter);
        
        if( !lru_key_map_.empty() )
        {
            auto iter2 = lru_key_map_.find(key);

            std::cout << "Key map timestamp: " 
                << iter2->second.tv_sec << "." 
                << iter2->second.tv_nsec << ", key: "
                << iter2->first << '\n';
        }
        else
        {
            std::cout << __func__ << ": ERROR - lru_key_map is empty!" 
                << std::endl;
        }
    }
    else
    {
        std::cout << __func__ << ": ERROR - lru_timestamp_map is empty!" 
            << std::endl;
    }

    return rval;
}


//
//  FJP FIXME.  still has to be a better way.
//
// first find the timestamp for the given key in the lru_key_map_
// then find the key for the given timestamp in the lru_timestamp_map_
//
// remove both entries and update entries in both maps.
//
//
void lru_cache::update_key_in_map( const int key )
{
    std::unordered_map<int, struct timespec>::const_iterator key_map_citer = 
        lru_key_map_.find(key);

    if( key_map_citer != lru_key_map_.end() )
    {
        struct timespec rval = key_map_citer->second;

        std::cout << __func__ << ": erasing key " << key 
            << " from lru_key_map " << std::endl;

        lru_key_map_.erase(key_map_citer);

        std::map<struct timespec, int, TimeSpecCmp>::const_iterator timestamp_map_citer = 
            lru_timestamp_map_.find(rval);

        if( timestamp_map_citer != lru_timestamp_map_.end() )
        {
            std::cout << __func__ << ": erasing key " << key 
                << " from lru_timestamp_map " << std::endl;

            lru_timestamp_map_.erase(timestamp_map_citer);
        }
        else
        {
            //
            // did not find any timestamp -> key pair for a given timestamp
            // even though it was in the key -> timestamp map
            // this is bad, something is suboptimal
            //
            //
            std::cout << __func__ << ": DID NOT FIND TIMESTAMP FOR KEY " << key 
                << " in lru_timestamp_map - this is an error!" << std::endl;
        }
    }
    else
    {
        //
        // did not find any timestamp for a given key value.
        // better be the very first insert for this key or something is amiss.
        //
        std::cout << __func__ << ": DID NOT FIND KEY " << key 
            << " in lru_key_map - is this the first insert?" << std::endl;
    }

    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    lru_key_map_.emplace(key, ts);
    lru_timestamp_map_.emplace(ts, key);
}


//
// FJP FIXME - using insert_or_assign() will give us a 
// memory leak on replacing value unless stack allocated!
//
void lru_cache::lru_put( const int key, void* value )
{
    //
    // check if we have to delete the oldest touched entry.
    //
    if( cache_.size() >= max_size_ )
    {
        std::cout << __func__ << ":  MAX CACHE SIZE! "
            << "  removing oldest timestamp entry..." << std::endl;
        int temp_key = remove_oldest_timestamp_entry();

        if( temp_key < 0 )
        {
            std::cout << __func__ << ": ERROR removing oldest timestamp entry!"
                << std::endl;
        }
        else
        {
            std::cout << "have to free memory and remove key " 
                << temp_key << " from the cache!" << std::endl;

            auto iter = cache_.find(temp_key);

            if( iter != cache_.end() )
            {
                void * rval = iter->second;
                cleanup_memory_(rval);
                cache_.erase(iter);
            }
            else
            {
                std::cout << __func__ << ": ERROR finding key " 
                    << temp_key << " in cache " << std::endl;
            }
        }
    }
    //
    // need some more logic for this one.  maybe someday.
    //
    // cache_.insert_or_assign(key, value);
    //
    const auto[iter, inserted] = cache_.emplace(key, value);

    if( inserted )
    {
        std::cout << "cache key# " << key
            << " storing address " << value << std::endl;
        //
        // inserted so update the timestamp -> key mapping
        // It's tricky to rock a rhyme, to rock a rhyme that's right on time
        // It's tricky, it's tricky, tricky, tricky, tricky
        // here we go.
        //
        update_key_in_map(key);
    }
    else
    {
        std::cout << "cache key# " << key
            << " storing existing address " << iter->second 
            << ", lru_put failed." << std::endl;
    }
}


void lru_cache::lru_clear( void )
{
    std::cout << "cache has " << cache_.size() 
        << " entries.  Clearing entries now." << std::endl;

    for( auto& [key, value]: cache_ )
    {
        std::cout << "clearing memory for cache key# " 
            << key << ", value: " << value << std::endl;
        
        cleanup_memory_(value);
    }

    std::cout << "clearing cache now." << std::endl;
    cache_.clear();
}

//
// FJP TODO:  This needs to be finished, 
// the timestamp lookup map is invalidated when this function is used.
// need to remove the key from both the key->timestamp and timestamp->key maps!!
//
// another function, remove_key_in_map() needs to be implemented to remove!
//
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


