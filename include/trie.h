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

#include <optional>
#include <unordered_map>
#include <vector>
namespace mutils {

template <typename KEY_SEGMENT_T, typename VALUE_T,
          typename STORED_T = std::optional<VALUE_T>>
class Trie {
protected:
  using Self = Trie<KEY_SEGMENT_T, VALUE_T>;
  using Key = std::vector<KEY_SEGMENT_T>;

  std::unordered_map<KEY_SEGMENT_T, Self> m_children;
  // We assume that STORED_T has a null-state default constructor
  STORED_T m_value;

  STORED_T m_lookup_deepest(Key key, size_t idx) {

    if (key.size() == idx) {
      return m_value;
    }

    auto child_index = key[idx];

    auto val = m_children.find(child_index);

    if (val == m_children.end()) {
      // do not make any allocations on lookup
      return m_value;
    } else {
      auto ret = val.m_lookup_deepest(key, idx + 1);

      if (ret) {
        return ret;
      } else {
        return m_value;
      }
    }
  }

  STORED_T m_lookup(Key key, size_t idx) {

    if (key.size() == idx) {
      return m_value;
    }

    auto find = key[idx];

    auto val = m_children.find(find);

    if (val == m_children.end())
      return STORED_T();
    return val->second.m_lookup(key, idx + 1);
  }

public:
  Trie() = default;

  void insert(KEY_SEGMENT_T at, VALUE_T val) { m_children[at].m_value = val; }

  void deep_insert(Key path, VALUE_T val) {

    if (path.size() == 1) {
      insert(path[0], val);
      return;
    }

    auto &child = m_children[path[0]];

    auto path_remaining =
        std::vector<KEY_SEGMENT_T>(path.begin() + 1, path.end());

    child.deep_insert(path_remaining, val);
  }

  //
  // Traverses the trie according to the key
  //
  // returns the deepest node that contains a value,
  // or a nullish value if there is no data along the path
  //
  STORED_T lookup_deepest(Key key) { return m_lookup_deepest(key, 0); }

  //
  // Traverses the trie according to the key
  //
  // returns the value at a specific node path
  // or a nullish value if the target contains no value
  //
  //
  STORED_T lookup(Key key) { return m_lookup(key, 0); }
};

}; // namespace mutils
