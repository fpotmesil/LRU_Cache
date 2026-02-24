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
    lru_cache test1(5);
    void * str1 = strdup("test1");
    test1.lru_put(1, str1);

    void * str2 = test1.lru_get(1);
    std::cout << "get(1): " << (char*)str2 << std::endl;

    void * str3 = test1.lru_remove(1);
    std::cout << "remove(1): " << (char*)str3 << std::endl;
    free(str3);


}

