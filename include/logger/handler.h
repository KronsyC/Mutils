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

#include "logentry.h"
#include<ostream>
#include<iostream>
namespace mutils {

//
// Loghandlers act as a way to provide custom internal logging mechanisms
// to the basic logger class
//
struct LogHandler {

  void on_log_create(LogEntry entry);
};

//
// The default log-handling behavior
//
struct StreamLogHandler : LogHandler {

  struct StreamLogHandlerConfiguration{
    bool colorize;

    std::ostream const* info_out = &std::cout;
    std::ostream const* warn_out = &std::cerr;
    std::ostream const* debug_out = &std::cout;
    std::ostream const* fatal_out = &std::cerr;
    std::ostream const* trace_out = &std::cout;
   
    std::string log_format = "";
  };


  using Config_T = StreamLogHandlerConfiguration;



  void on_log_create(LogEntry entry) {
    switch (entry.level) {
      case LogLevel::Info: {
        break;
      }
      case LogLevel::Warn: {
        break;
      }
      case LogLevel::Debug: {
        break;
      }
      case LogLevel::Error: {
        break;
      }
      case LogLevel::Fatal: {
        break;
      }
      case LogLevel::Trace: {
        break;
      }
    }
  }
};


}; // namespace mutils
