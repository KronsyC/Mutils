#pragma once
#include "panic.h"

#ifndef NDEBUG

// Debug mode is on
#define COMPTIME_STRINGIFY_DETAIL(x) #x
#define COMPTIME_STRINGIFY(x) COMPTIME_STRINGIFY_DETAIL(x)

#define ASSERT(expr, error) \
if(!(bool)(expr)){ \
    mutils::panic(std::string("[[Assertion Error @ " __FILE__ ":" COMPTIME_STRINGIFY(__LINE__) "]] -> ") + error); \
    exit(1); \
}


#else

#define ASSERT(expr, error)

#endif