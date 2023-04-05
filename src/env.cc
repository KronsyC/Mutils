//
//
// Cross-Platform Environment Variable Access (Unix-Like + Windows)
//
//
//

#include "../include/env.h"
#include <cstdlib>
#include <stdlib.h>
using namespace mutils;

#ifdef WIN32

#include <windows.h>
// Windows-compatible definitions for environment access

const char *mp_get_environ_var(const char *vname) { return getenv(vname); }

std::vector<std::string> mp_get_all_env_vars() {

  char *e = GetEnvironmentStrings();

  std::vector<std::string> values;

  char *cursor = e;
  std::string val;
  bool write = true;
  while (true) {
    if (*cursor == 0 && *(cursor + 1) == 0)
      break;

    if (*cursor == '=') {
      write = false;
      values.push_back(val);
    }

    if (write)
      val += *cursor;

    if (*cursor == 0)
      write = true;
  }

  FreeEnvironmentStrings(e);
  return values;
}

void mp_unset_environ_var(const char *vname) {
  SetEnvironmentVariable(vname, NULL);
}

void mp_set_environ_var(const char *vname, const char *vvalue) {
  SetEnvironmentVariable(vname, vvalue);
}

void mp_clear_environ_vars() {
  for (std::string var : mp_get_all_env_vars()) {
    mp_unset_environ_var(var.c_str());
  }
}
#else

//
// UNIX-compatible definitions for environment access
//

extern char **environ;

// Get a specific var
const char *mp_get_environ_var(const char *vname) {
  return secure_getenv(vname);
}

// Set a specific var
void mp_set_environ_var(const char *vname, const char *vvalue) {
  setenv(vname, vvalue, 1);
}

// Unset a specific var
void mp_unset_environ_var(const char *vname) { unsetenv(vname); }

std::vector<std::string> mp_get_all_env_vars() {

  std::vector<std::string> variables;

  for (char **element = environ; *element != 0; element++) {
    char *var = *element;
    std::string vname = "";
    for (char *c = var; *c != 0 && *c != '='; c++) {
      vname += *c;
    }
    variables.push_back(vname);
  }

  return variables;
}

// Clear all vars
void mp_clear_environ_vars() { clearenv(); }

#endif

void ProgramEnvironment::clear() { mp_clear_environ_vars(); }

ProgramEnvironment::EnvEntry
ProgramEnvironment::operator[](std::string var_name) {
  ProgramEnvironment::EnvEntry env;
  env.name = var_name;
  return env;
}

void ProgramEnvironment::EnvEntry::operator=(const char *value) {
  mp_set_environ_var(this->name.c_str(), value);
}

ProgramEnvironment::EnvEntry::operator const char *() {
  return mp_get_environ_var(this->name.c_str());
}
void ProgramEnvironment::EnvEntry::clear() {
  mp_unset_environ_var(this->name.c_str());
}

std::vector<std::string> ProgramEnvironment::list_vars() {

  return mp_get_all_env_vars();
}
