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


//
// argreader.h
//
// A utility for reading in command-line arguments
//

#pragma once

#include <string>
#include <type_traits>

namespace mutils {

class ArgReader {

  //
  // -foo    | Enable the 'foo' flag  (boolean)
  // -foo-   | Disable the 'foo' flag (boolean)
  //
  // --foo bar       | Set the foo variable to either
  // --foo "bar baz" | bar, or bar baz
  //
  // --foo 123   | set the foo variable to 123
  //
  // --foo alpha | foo has multiple choices, and we choose alpha (Enum)
  //
  //
  // PROGRAM foo --mode alpha bar baz
  //
  //
  // We also have positional arguments
  //
  // some programs have different 'modes' that are accessed by different
  // sequences of positional arguments, i.e PROGRAM build foo
  //
  // As a result, the ArgReader should act more-so as a decision tree
  // where different switches and modes are valid depending on previous options
  //

public:
  struct CommandLineArgument {};

  struct IntegerArgument : CommandLineArgument {};

  struct BoolArgument : CommandLineArgument {};

  struct StringArgument : CommandLineArgument {};

  template <typename Choices>
    requires std::is_enum_v<Choices>
  struct EnumArgument : CommandLineArgument {
    // static_assert(std::is_enum_v<Choices>,
    //               "An EnumArgument must take an enum type");

    // EnumArgument(std::map<std::string, Choices> options){};
  };
};

}; // namespace mutils
