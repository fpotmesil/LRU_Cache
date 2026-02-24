#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <cstring>
#include <iostream>
#include <map>

#include "lru_cache.h"

//
// print_entry function requires -std=c++20
// because of the auto& to map entry.
// could use an iterator to get around this.
//
#if 0
fpotm@LAPTOP-EA8OT4CH MINGW64 /c/dev
$ g++ -Wall timespec_lru_map.cpp -std=c++20
timespec_lru_map.cpp: In function 'int main()':
timespec_lru_map.cpp:78:35: warning: unknown conversion type character 'D' in format [-Wformat=]
   78 |     strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
      |                                   ^
timespec_lru_map.cpp:78:38: warning: unknown conversion type character 'T' in format [-Wformat=]
   78 |     strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
      |                                      ^
timespec_lru_map.cpp:80:35: warning: unknown conversion type character 'j' in format [-Wformat=]
   80 |     printf("Raw timespec.tv_sec: %jd\n", (intmax_t)ts.tv_sec);
      |                                   ^
timespec_lru_map.cpp:80:12: warning: too many arguments for format [-Wformat-extra-args]
   80 |     printf("Raw timespec.tv_sec: %jd\n", (intmax_t)ts.tv_sec);
      |            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~

fpotm@LAPTOP-EA8OT4CH MINGW64 /c/dev
$ ./a.exe
Current time: 02/24/26 01:18:53.908240100 UTC
Raw timespec.tv_sec: 1771895933
Raw timespec.tv_nsec: 908240100
[1771895933.908240100] = 1
[1771895933.908658800] = 2
[1771895933.908660000] = 2
[1771895933.908660600] = 3
[1771895933.908661200] = 4
[1771895933.908661700] = 5
[1771895933.908662200] = 6
[1771895933.908662800] = 7
First map entry timestamp: 1771895933.908240100, key: 1

fpotm@LAPTOP-EA8OT4CH MINGW64 /c/dev
#endif

void cleanup_stack_string( void* str )
{
    (void)(str); // error: unused parameter ‘str’ [-Werror=unused-parameter]
    // do nothing for stack allocated strings.
    // or anything allocated on the stack.
    // could try to free it just to see what happens!

    std::cout << __func__ << " called with arg: '"
        << (char*)str << "' - doing nothing, stack string!" 
        << std::endl;
}

void cleanup_allocated_string( void* str )
{
    if( str )
    {
        std::cout << __func__ << " called with arg: '" 
            << (char*)str << "' at address: " << str << std::endl;
    }
    else
    {
        std::cout << __func__ << " called with null" << std::endl; 
    }

    if( str )
    {
        free(str);
        str = nullptr;
    }
}
 
void print_entry(
        const auto& entry)
{
    std::cout << '[' 
        << entry.first.tv_sec <<"."
        <<entry.first.tv_nsec << "] = " 
        << entry.second << '\n';
}

//
// beginning block for timespec example from
// example from https://en.cppreference.com/w/c/chrono/timespec
// 
// map custom comparator from examples at
// https://en.cppreference.com/w/cpp/container/map/map.html
//
int main(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
    printf("Raw timespec.tv_sec: %jd\n", (intmax_t)ts.tv_sec);
    printf("Raw timespec.tv_nsec: %09ld\n", ts.tv_nsec);


    std::map<struct timespec, int, TimeSpecCmp> lru_map;

    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 1);

    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 2);


    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 3);


    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 4);


    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 5);


    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 6);

    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 7);


    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 8);
    //
    // test map ordering when inserting immediately in order
    //
    std::cout << "Map after initial 7 insertions: " << std::endl;
    for (const auto& entry : lru_map)
        print_entry(entry);

    //
    // test the map beginning gives me the 'oldest' entry.
    //
    auto iter = lru_map.begin();
    std::cout << "First map entry timestamp: " 
        << iter->first.tv_sec << "." 
        << iter->first.tv_nsec << ", key: "
        << iter->second << '\n';

    //
    // should be erasing LRU Cache key 1
    //
    lru_map.erase(iter);

    //
    // re-insert LRU Cache key 1 with updated timestamp
    //
    timespec_get(&ts, TIME_UTC);
    lru_map.emplace(ts, 1);

    //
    // now check map ordering to make sure key 1 is actually the newest
    // and not the oldeset
    //
    std::cout << "Map after erasing and re-inserting key 1 "
        "to check ordering by time: " 
        << std::endl;

    for (const auto& entry : lru_map)
        print_entry(entry);


    //
    // ok, test the preliminary lru_cache functions 
    //
    lru_cache test1(3, cleanup_allocated_string);
    void * str1 = strdup("test1");
    test1.lru_put(1, str1);

    void * str2 = test1.lru_get(1);
    std::cout << "get(1): " << (char*)str2 << std::endl;

    void * str3 = test1.lru_remove(1);
    std::cout << "remove(1): " << (char*)str3 << std::endl;
    cleanup_allocated_string(str3);

    void * nullTest = nullptr;
    test1.lru_put(1, nullTest); // insert null test on clear.

    //
    // add another function lru_replace() to fix this case.
    //
    // will insert if key does not exist, replace value if
    // key does exist.
    //
    void * notNullTest = strdup( "This will be a memory leak" );
    test1.lru_put(1, notNullTest); // test error message

    void * fred = strdup("this challenge is pretty fun.  and challenging really");
    test1.lru_put(2, fred);

    void * dogs = strdup("my dogs are insane");
    test1.lru_put(3, dogs);

    void * kids = strdup("my kids are amazing");
    test1.lru_put(4, kids);

    void * wife = strdup("my wife is pretty.  and pretty hot.  and super smart.");
    test1.lru_put(5, wife);

    test1.lru_clear();

    //
    // strings are on the stack, 
    // so cleanup function doesnt do anything,
    // but still has to be set for lru_cache constructor.
    //
    lru_cache stack_strings_cache(3, cleanup_stack_string);

    char stack_str1[]  = "Test stack string #1";
    stack_strings_cache.lru_put(1, stack_str1 );

    char stack_str2[]  = "Test stack string #2";
    stack_strings_cache.lru_put(2, stack_str2 );

    char stack_str3[]  = "Test stack string #3";
    stack_strings_cache.lru_put(3, stack_str3 );

    char stack_str4[]  = "Test stack string #4";
    stack_strings_cache.lru_put(4, stack_str4 );

    char stack_str5[]  = "Test stack string #5";
    stack_strings_cache.lru_put(5, stack_str5 );

    stack_strings_cache.lru_clear();


}

