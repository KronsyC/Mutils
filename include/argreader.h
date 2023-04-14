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
