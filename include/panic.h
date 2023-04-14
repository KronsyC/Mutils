#pragma once

#include <iostream>
#include<string>
namespace mutils {
[[noreturn]] __attribute__((always_inline)) inline void PANIC(std::string msg) {
  std::string panic = "\033[0;31;1mpanic:  \033[0;0m";
  std::cerr << panic << msg << std::endl;
  exit(1);
}
}; // namespace mutils
