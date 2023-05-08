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

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include<cstring>

namespace mutils::string {


[[nodiscard]] inline std::vector<std::string_view> split(std::string_view s, char sep) noexcept {
  std::vector<std::string_view> ret;

  std::size_t last_idx = 0;

  std::size_t idx = 0;
  for (auto c : s) {
    if (c == sep) {
      auto substr = s.substr(last_idx, idx - last_idx);
      ret.push_back(substr);
      last_idx = idx + 1;
    } else if (idx == s.size() - 1) {
      auto substr = s.substr(last_idx, idx - last_idx);
      ret.push_back(substr);
    }
    idx++;
  }
  return ret;
}

//
// Capitalize the contents of the passed string_view
// returns a newly constructed string representing the
// transformed content of the view
//
[[nodiscard]] inline std::string capitalize(std::string_view s) noexcept {
  char const  DIFF = 'a' - 'A';
  std::string ret  = std::string(s);
  for (char& c : ret) {
    if (c >= 'a' && c <= 'z') {
      c -= DIFF;
    }
  }
  return ret;
}

//
// Capitalize the contents of the passed span<char>
// inplace
//
inline void capitalize_inplace(std::span<char> span) noexcept {
  char const DIFF = 'a' - 'A';
  for (char& c : span) {
    if (c >= 'a' && c <= 'z') {
      c -= DIFF;
    }
  }
}

//
// Lowercase the contents of the passed string_view
// returns a newly constructed string representing the
// transformed content of the view
//
[[nodiscard]] inline std::string lowercase(std::string_view s) noexcept {
  char const  DIFF = 'a' - 'A';
  std::string ret  = std::string(s);
  for (char& c : ret) {
    if (c >= 'A' && c <= 'Z') {
      c += DIFF;
    }
  }
  return ret;
}

//
// Lowercase the contents of the passed span<char>
// inplace
//
inline void lowercase_inplace(std::span<char> span) noexcept {
  char const DIFF = 'a' - 'A';
  for (char& c : span) {
    if (c >= 'A' && c <= 'Z') {
      c += DIFF;
    }
  }
}

[[nodiscard]] inline bool is_strippable_char(char c) noexcept {
  return (c >= 1 && c <= 32) || c == 127;
}

//
// Strip any trailing and leading whitespace to the string
//
// whitespace in this case is defined as any non-visible characters
// (spaces, tabs, control characters, etc.)
//
// returns a new stripped string
//
[[nodiscard]] inline std::string strip(std::string_view s) noexcept{
  std::size_t start = 0;

  for (char c : s) {
    if (is_strippable_char(c)) {
      start++;
    } else {
      break;
    }
  }

  std::size_t size = s.size() - start;

  for (auto c = s.rbegin(); c < s.rend(); c++) {
    if (size == 0) {
      break;
    }

    if (is_strippable_char(*c)) {
      size--;
    } else {
      break;
    }
  }
  return std::string(s.substr(start, size));
}

[[nodiscard]] inline std::string strip_start(std::string_view s) noexcept{
  std::size_t start = 0;

  for(char c : s){
    if(is_strippable_char(c)){
      start++;
    }
    else{
      break;
    }
  }
  return std::string(s.substr(start));
};

[[nodiscard]] inline std::string pad_start(std::string_view s, char with, size_t n) noexcept{
  std::string ret(n, with);
  ret.append(s);
  return ret;
};

[[nodiscard]] inline std::string center(std::string_view s, size_t cols) noexcept{
  const auto remainder = cols - s.size();

  if((remainder & 1) == 0){
    // Perfect alignment
    const auto pad_count = remainder >> 2;
    return pad_start(s, ' ', pad_count) + std::string(pad_count, ' ');
  }
  else{
    // Imperfect alignment, slighly align left
    const auto pad_count = remainder >> 2;
    return pad_start(s, ' ', pad_count + 1) + std::string(pad_count, ' ');
  }
}

}; // namespace mutils::string
