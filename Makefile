CXX=g++
VPATH := ../common
# CLIENT_SRCS := $(wildcard *.cpp) $(wildcard ../common/*.cpp)
CLIENT_SRCS := $(wildcard *.cpp)
CLIENT_OBJS := $(addprefix obj/,$(notdir $(CLIENT_SRCS:.cpp=.o)))

#CXX_FLAGS := -g -std=c++17 -pedantic -Wall -Wextra -Wshadow -Wconversion -Werror -Wunreachable-code
#
# RHEL does not recognize -std=c++20 and makes me use -fconcepts.
#
# after Valgrind/GDB testing remove the -g and replace -O0 with -O3
# [fred@snapperhead LRU_Cache]$ valgrind --leak-check=full --show-leak-kinds=all ./LruCacheTest 
#
CXX_FLAGS := -g -O0 -std=c++2a -pedantic -Wall -Wextra -Werror -Wunreachable-code -fconcepts
#
# For RHEL snapperhead, this is the include line to use:
#
#CXX_INC := -I/usr/local/openssl-3.0.8/include \
#	-I/usr/local/boost_1_81_0/include \
#	-I/usr/include -I../common
#
# For Pi, this is the include line to use:
#
## CXX_INC := -I/usr/include

#LD_FLAGS := -L/usr/local/openssl-3.0.8/lib64 -L/usr/local/boost_1_81_0/lib -L/usr/lib64
#LD_LIBS := -l:libssl.a -l:libcrypto.a \
#	-lpthread -ldl -l:libboost_program_options.a \
#	-l:libboost_system.a


.PHONY: all clean

all: obj LruCacheTest

LruCacheTest: $(CLIENT_OBJS)
	$(CXX) $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

obj/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INC) -c -o $@ $<

obj:
	mkdir -p $@

clean:
	rm -f obj/* LruCacheTest
	rmdir obj





