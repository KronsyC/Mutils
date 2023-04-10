#pragma once

#include "ansi.h"
#include "funcmod.h"
#include <iostream>

namespace mutils {
[[noreturn]] INLINE inline void PANIC(const char *msg) {
  std::cerr << mutils::ansi::FormatBuilder()
                   .fg(mutils::ansi::Color::Red)
                   .bold()
                   .write("panic: ")
                   .reset()
                   .write(msg)
                   .str()
            << std::endl;
  exit(1);
}
}; // namespace mutils
