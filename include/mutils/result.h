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
#include <optional>

namespace mutils {

/**
 * A Simple Result class inspired heavily by rust
 */
template <typename T, typename E> class Result {
private:
  enum {
    Good,
    Bad,
  } m_variant;

  union {
    T m_good;
    E m_bad;
  };

public:
  /**
   * Construct a Good Result
   */
  Result(T value) : m_variant(Good), m_good(std::move(value)) {
  }

  /**
   * Construct a Bad Result
   */
  Result(E error) : m_variant(Bad), m_bad(std::move(error)) {
  }

  // Results are move-only
  Result(Result&&) = default;

  ~Result() {
    if (m_variant == Good) {
      m_good.~T();
    } else {
      m_bad.~E();
    }
  }

  /**
   * Returns the value contained within the Result if it is Good
   * otherwise, throws the Error using C++ throw keyword
   */
  T value_or_throw() {
    if (m_variant == Good) {
      return std::move(m_good);
    } else {
      throw std::move(m_bad);
    }
  }

  /**
   * Returns the value contained within the Result if it is Good
   * otherwise, panics and the program crashes
   *
   * **UNRECOVERABLE**
   */
  T value_or_panic() {
    if (m_variant == Good) {
      return std::move(m_good);
    } else {
      PANIC("Result contained an Error, but was accessed via value_or_panic()");
    }
  }

  /**
   * Returns the contained value within the Result if it is Good
   * otherwise, returns the provided alternative value
   */
  T value_or(T alternative) {
    if (m_variant == Good) {
      return std::move(m_good);
    } else {
      return std::move(alternative);
    }
  }

  bool is_good() const {
    return m_variant == Good;
  }

  std::optional<T> value() {
    if (m_variant == Good) {
      return std::move(m_good);
    } else {
      return {};
    }
  }

  std::optional<E> error() {
    if (m_variant == Bad) {
      return std::move(m_bad);
    } else {
      return {};
    }
  }
};
} // namespace mutils
