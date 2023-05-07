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

#include <filesystem>

#include "fcntl.h"
#include "./panic.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

namespace mutils {

typedef std::filesystem::path Path;

//
// A specialized structure for managing the reading
// and referencing into a text document
//
// **WARNING**: Do not use this for binary files,
// this class is designed for use exlusively
// with **sane** plaintext docs
//
// i.e Source Code, Config Files, Notes
//
struct TextFile {

  enum class AccessPattern {
    Sequential,
    Random,
  };

  struct LocationInfo {
    size_t line_no;
    size_t col_no;
  };

  //
  // A reference to a span of characters
  // within a text document
  //
  struct Span {
    TextFile& file;
    size_t    start;
    size_t    stop;

    size_t length() {
      return stop - start + 1;
    }

    LocationInfo start_pos() {
      return file.location_at(start);
    }

    LocationInfo stop_pos() {
      return file.location_at(stop);
    }

    Span(TextFile& f) : file(f){};

    operator std::string_view() {
      char const* start_ptr = file.raw_content.data() + start;
      size_t      length    = stop - start;
      return std::string_view(start_ptr, length);
    }
  };

  //
  // Read a Text file character-by-character
  //
  struct Reader {
    friend struct TextFile;

    LocationInfo loc() {
      return file.location_at(cursor);
    }

    char next() {
      if (cursor == file.raw_content.size()) {
        return '\0';
      }
      return file.raw_content[cursor++];
    }

    void back() {
      cursor--;
    }

    void begin_span() {
      span_start = cursor;
    }

    Span end_span() {
      Span s(file);
      s.start = span_start;
      s.stop  = cursor;
      return s;
    }

  private:
    Reader(TextFile& file, size_t start = 0) : file(file), cursor(start) {
    }

    size_t    cursor     = 0;
    size_t    span_start = 0;
    TextFile& file;
  };

  friend struct Reader;
  /**
   * Files may not be copied
   */
  TextFile(TextFile&) = delete;

  TextFile(Path loc) {
    this->m_path = loc;

    auto file_descriptor = open(loc.c_str(), O_RDONLY);

    if (file_descriptor == -1) {
      // TODO: Add a better error message
      mutils::PANIC("Failed to open file");
    }

    struct stat st;

    if (fstat(file_descriptor, &st) == -1) {
      // TODO: Add better error message
      mutils::PANIC("Failed to stat file");
    }

    auto file_size = st.st_size;

    char const* memory_mapping =
        static_cast<char const*>(mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0u));

    if (memory_mapping == MAP_FAILED) {
      mutils::PANIC("Failed to map file into memory");
    }

    // Transform the mmap into a string_view
    this->raw_content = std::string_view(memory_mapping, file_size);

    char const* start = raw_content.data();
    size_t      count = 0;
    for (char const& c : raw_content) {
      if (c == '\n') {
        std::string_view substr(start, count);

        start = &c + 1;
        count = 0;

        m_lines.push_back(substr);
      } else {
        count++;
      }
    }
    if (count == 0) {
      m_lines.push_back(std::string_view(start, count));
    }
  }

  ~TextFile() {
    munmap((void*)raw_content.data(), raw_content.size());
  }

  std::string_view get_line(size_t index) const {
    return m_lines[index - 1];
  }

  std::vector<std::string_view> const& lines() const {
    return m_lines;
  }

  Reader reader() {
    return Reader(*this, 0);
  }

  //
  // Return the data between two indexes (inclusive)
  //
  std::string_view data_between(size_t start, size_t end) {
    return raw_content.substr(start, end - start + 1);
  }

  struct SourceLineIndexes {
    size_t start_idx; // The index of the first character of a line
    size_t end_idx;   // The index of the last character of a line
  };

  SourceLineIndexes source_line_index(size_t line_no) {
    auto line = get_line(line_no);

    auto line_start = line.data();
    auto data_start = raw_content.data();

    size_t index = line_start - data_start;

    SourceLineIndexes idxs;
    idxs.start_idx = index;
    idxs.end_idx   = index + line.size() - 1;
    return idxs;
  }

  char operator[](size_t idx) {
    return raw_content[idx];
  }

private:
  Path m_path;

  std::string_view              raw_content;
  std::vector<std::string_view> m_lines;

  //
  // Get the row and column number at a specific index
  // into the file
  //
  LocationInfo location_at(size_t idx) {
    LocationInfo loc;

    // Make the initial guess, this is basically never going to be correct
    size_t average_words_per_line = raw_content.size() / m_lines.size();
    size_t line_number_guess      = (idx / average_words_per_line) + 1;

    // Make sure we dont go out of bounds
    if (line_number_guess >= m_lines.size()) {
      line_number_guess = m_lines.size() - 1;
    }

    // This is the line we are guessing

    // We binary search for the correct line,
    // with the initial guess as the central point

    size_t lower_bound = 0;
    size_t upper_bound = m_lines.size() - 1;
    while (true) {

      auto   line_bounding_indexes = source_line_index(line_number_guess);
      size_t current_start         = line_bounding_indexes.start_idx;
      size_t current_end           = line_bounding_indexes.end_idx;

      if (current_start > idx) {
        // Guess lower
        upper_bound       = line_number_guess;
        line_number_guess = (line_number_guess + lower_bound) / 2;
      } else if (current_end < idx) {
        // Guess higher
        lower_bound       = line_number_guess;
        line_number_guess = (line_number_guess + upper_bound) / 2;
      } else {
        // We are within the right line
        loc.line_no = line_number_guess;
        loc.col_no  = (idx - current_start) + 1;
        return loc;
      }
    }
    mutils::PANIC("File source location lookup failed");
  }
};

}; // namespace mutils
