#pragma once

#include <string>
#include <vector>
namespace mutils {

struct ProgramEnvironment {

  struct EnvEntry {
    std::string name;

    /**
     * Assign the environment variable
     */
    void operator=(const char *value);

    operator const char *();

    /**
     * Clear the value of the current variable in the environment
     */
    void clear();
  };

  EnvEntry operator[](std::string var_name);

  /*
   * Clear the values of all variables in the environment
   */
  void clear();

  std::vector<std::string> list_vars();
};

inline ProgramEnvironment env;

}; // namespace mutils
