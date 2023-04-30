/// Copyright (c) 2023 Samir Bioud
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
/// USE OR OTHER DEALINGS IN THE SOFTWARE.
///

#pragma once

#include <chrono>
#include <string>
#include <thread>

namespace mutils {

enum class LogLevel : uint8_t {
  Info,  // General / high-level information about the operation of the program
  Debug, // More detailed explanations of whats happening in the program
  Trace, // Run through what the program is doing, only usable in development mode
  Warn,  // Something unexpected happened, but we can continue going on as normal
  Error, // Severely degraded functionality, but the program still has control (it may gracefully crash)
  Fatal, // The program has lost control, and has no choice but to exit (segfault, and other such errors)
};

struct LogEntry {
  using Clock = std::chrono::high_resolution_clock;
  using Time  = std::chrono::time_point<Clock>;

  LogLevel        level;      // The severity/level of the log
  std::string     message;    // The message which the log contains
  std::thread::id thread_id;  // The thread from which the log was created
  Time            created_at; // The time at which the log was created
};

}; // namespace mutils
