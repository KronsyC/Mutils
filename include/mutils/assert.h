/// Copyright (c) 2023 Samir Bioud
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
/// OR OTHER DEALINGS IN THE SOFTWARE.
///


#pragma once
#include "./panic.h"
#include "./stringify.h"
#include <string>
#ifndef NDEBUG

// Debug mode is on
#  define COMPTIME_STRINGIFY_DETAIL(x) #x
#  define COMPTIME_STRINGIFY(x) COMPTIME_STRINGIFY_DETAIL(x)

#  define MUTILS_ASSERTION_ERROR(condition, ASSERTION_REASON, errmsg)                                                  \
    if (!(condition)) {                                                                                                \
      const auto message = std::string("[[Assertion Error @ " __FILE__ ":" COMPTIME_STRINGIFY(__LINE__) " ]] :: ") +   \
                           std::string(ASSERTION_REASON) + "\n\t" + std::string(errmsg);                               \
      mutils::PANIC(message);                                                                                          \
      exit(1);                                                                                                         \
    }

#  define MUTILS_BINARY_ASSERTION_ERROR(a, b, operator_name, operator_symbol, errmsg)                                  \
    {                                                                                                                  \
      auto __$a_eval = a;                                                                                                 \
      auto __$b_eval = b;                                                                                                 \
      MUTILS_ASSERTION_ERROR(__$a_eval operator_symbol __$b_eval,                                                            \
                             "MUTILS_ASSERT." operator_name                                                            \
                             "( " COMPTIME_STRINGIFY(a) ", " COMPTIME_STRINGIFY(b) " )  -> [" +                        \
                                 mutils::stringify(__$a_eval) + " " #operator_symbol " " + mutils::stringify(__$b_eval) +    \
                                 "] evaluated to false",                                                               \
                             errmsg);                                                                                  \
    }


#  define MUTILS_ASSERT(expr, error)                                                                                   \
    MUTILS_ASSERTION_ERROR((bool)(expr), "MUTILS_ASSERT.true " COMPTIME_STRINGIFY(expr), error)

#  define MUTILS_ASSERT_NOT(expr, error)                                                                               \
    MUTILS_ASSERTION_ERROR(!(bool)(expr), "MUTILS_ASSERT.false " COMPTIME_STRINGIFY(expr), error)

#  define MUTILS_ASSERT_EQ(a, b, error) MUTILS_BINARY_ASSERTION_ERROR(a, b, "equal", ==, error)

#  define MUTILS_ASSERT_NEQ(a, b, error) MUTILS_BINARY_ASSERTION_ERROR(a, b, "not_equal", !=, error)

#  define MUTILS_ASSERT_GT(a, b, error) MUTILS_BINARY_ASSERTION_ERROR(a, b, "greater", >, error)
#  define MUTILS_ASSERT_LT(a, b, error) MUTILS_BINARY_ASSERTION_ERROR(a, b, "lesser", <, error)

#  define MUTILS_ASSERT_GTE(a, b, error) MUTILS_BINARY_ASSERTION_ERROR(a, b, "greater_eq", >=, error)
#  define MUTILS_ASSERT_LTE(a, b, error) MUTILS_BINARY_ASSERTION_ERROR(a, b, "lesser_eq", <=, error)

#else

#  define MUTILS_ASSERT(expr, error)
#  define MUTILS_ASSERT_NOT(expr, error)

#  define MUTILS_ASSERT_EQ(a, b, error)
#  define MUTILS_ASSERT_NEQ(a, b, error)

#  define MUTILS_ASSERT_GT(a, b, error)
#  define MUTILS_ASSERT_LT(a, b, error)

#  define MUTILS_ASSERT_GTE(a, b, error)
#  define MUTILS_ASSERT_LTE(a, b, error)

#endif
