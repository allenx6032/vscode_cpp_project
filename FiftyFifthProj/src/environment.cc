// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "environment.h"

#include <stddef.h>

#include <string_view>
#include <vector>

#include "ptr_util.h"
#include "string_util.h"
#include "utf_string_conversions.h"
#include "build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
#include <stdlib.h>
#endif

namespace base {

namespace {

class EnvironmentImpl : public Environment {
 public:
  bool GetVar(std::string_view variable_name, std::string* result) override {
    if (GetVarImpl(variable_name, result))
      return true;

    // Some commonly used variable names are uppercase while others
    // are lowercase, which is inconsistent. Let's try to be helpful
    // and look for a variable name with the reverse case.
    // I.e. HTTP_PROXY may be http_proxy for some users/systems.
    char first_char = variable_name[0];
    std::string alternate_case_var;
    if (IsAsciiLower(first_char))
      alternate_case_var = ToUpperASCII(variable_name);
    else if (IsAsciiUpper(first_char))
      alternate_case_var = ToLowerASCII(variable_name);
    else
      return false;
    return GetVarImpl(alternate_case_var, result);
  }

  bool SetVar(std::string_view variable_name,
              const std::string& new_value) override {
    return SetVarImpl(variable_name, new_value);
  }

  bool UnSetVar(std::string_view variable_name) override {
    return UnSetVarImpl(variable_name);
  }

 private:
  bool GetVarImpl(std::string_view variable_name, std::string* result) {
#if defined(OS_WIN)
    DWORD value_length = ::GetEnvironmentVariable(
        reinterpret_cast<LPCWSTR>(UTF8ToUTF16(variable_name).c_str()), nullptr,
        0);
    if (value_length == 0)
      return false;
    if (result) {
      std::unique_ptr<char16_t[]> value(new char16_t[value_length]);
      ::GetEnvironmentVariable(
          reinterpret_cast<LPCWSTR>(UTF8ToUTF16(variable_name).c_str()),
          reinterpret_cast<LPWSTR>(value.get()), value_length);
      *result = UTF16ToUTF8(value.get());
    }
    return true;
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    const char* env_value = getenv(variable_name.data());
    if (!env_value)
      return false;
    // Note that the variable may be defined but empty.
    if (result)
      *result = env_value;
    return true;
#endif
  }

  bool SetVarImpl(std::string_view variable_name,
                  const std::string& new_value) {
#if defined(OS_WIN)
    // On success, a nonzero value is returned.
    return !!SetEnvironmentVariable(
        reinterpret_cast<LPCWSTR>(UTF8ToUTF16(variable_name).c_str()),
        reinterpret_cast<LPCWSTR>(UTF8ToUTF16(new_value).c_str()));
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    // On success, zero is returned.
    return !setenv(variable_name.data(), new_value.c_str(), 1);
#endif
  }

  bool UnSetVarImpl(std::string_view variable_name) {
#if defined(OS_WIN)
    // On success, a nonzero value is returned.
    return !!SetEnvironmentVariable(
        reinterpret_cast<LPCWSTR>(UTF8ToUTF16(variable_name).c_str()), nullptr);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    // On success, zero is returned.
    return !unsetenv(variable_name.data());
#endif
  }
};

// Parses a null-terminated input string of an environment block. The key is
// placed into the given string, and the total length of the line, including
// the terminating null, is returned.
size_t ParseEnvLine(const NativeEnvironmentString::value_type* input,
                    NativeEnvironmentString* key) {
  // Skip to the equals or end of the string, this is the key.
  size_t cur = 0;
  while (input[cur] && input[cur] != '=')
    cur++;
  *key = NativeEnvironmentString(&input[0], cur);

  // Now just skip to the end of the string.
  while (input[cur])
    cur++;
  return cur + 1;
}

}  // namespace

namespace env_vars {

#if defined(OS_POSIX) || defined(OS_FUCHSIA)
// On Posix systems, this variable contains the location of the user's home
// directory. (e.g, /home/username/).
const char kHome[] = "HOME";
#endif

}  // namespace env_vars

Environment::~Environment() = default;

// static
std::unique_ptr<Environment> Environment::Create() {
  return std::make_unique<EnvironmentImpl>();
}

bool Environment::HasVar(std::string_view variable_name) {
  return GetVar(variable_name, nullptr);
}

#if defined(OS_WIN)

std::u16string AlterEnvironment(const char16_t* env,
                                const EnvironmentMap& changes) {
  std::u16string result;

  // First copy all unmodified values to the output.
  size_t cur_env = 0;
  std::u16string key;
  while (env[cur_env]) {
    const char16_t* line = &env[cur_env];
    size_t line_length = ParseEnvLine(line, &key);

    // Keep only values not specified in the change vector.
    EnvironmentMap::const_iterator found_change = changes.find(key);
    if (found_change == changes.end())
      result.append(line, line_length);

    cur_env += line_length;
  }

  // Now append all modified and new values.
  for (EnvironmentMap::const_iterator i = changes.begin(); i != changes.end();
       ++i) {
    if (!i->second.empty()) {
      result.append(i->first);
      result.push_back('=');
      result.append(i->second);
      result.push_back(0);
    }
  }

  // An additional null marks the end of the list. We always need a double-null
  // in case nothing was added above.
  if (result.empty())
    result.push_back(0);
  result.push_back(0);
  return result;
}

#elif defined(OS_POSIX) || defined(OS_FUCHSIA)

std::unique_ptr<char*[]> AlterEnvironment(const char* const* const env,
                                          const EnvironmentMap& changes) {
  std::string value_storage;  // Holds concatenated null-terminated strings.
  std::vector<size_t> result_indices;  // Line indices into value_storage.

  // First build up all of the unchanged environment strings. These are
  // null-terminated of the form "key=value".
  std::string key;
  for (size_t i = 0; env[i]; i++) {
    size_t line_length = ParseEnvLine(env[i], &key);

    // Keep only values not specified in the change vector.
    EnvironmentMap::const_iterator found_change = changes.find(key);
    if (found_change == changes.end()) {
      result_indices.push_back(value_storage.size());
      value_storage.append(env[i], line_length);
    }
  }

  // Now append all modified and new values.
  for (EnvironmentMap::const_iterator i = changes.begin(); i != changes.end();
       ++i) {
    if (!i->second.empty()) {
      result_indices.push_back(value_storage.size());
      value_storage.append(i->first);
      value_storage.push_back('=');
      value_storage.append(i->second);
      value_storage.push_back(0);
    }
  }

  size_t pointer_count_required =
      result_indices.size() + 1 +  // Null-terminated array of pointers.
      (value_storage.size() + sizeof(char*) - 1) / sizeof(char*);  // Buffer.
  std::unique_ptr<char*[]> result(new char*[pointer_count_required]);

  // The string storage goes after the array of pointers.
  char* storage_data =
      reinterpret_cast<char*>(&result.get()[result_indices.size() + 1]);
  if (!value_storage.empty())
    memcpy(storage_data, value_storage.data(), value_storage.size());

  // Fill array of pointers at the beginning of the result.
  for (size_t i = 0; i < result_indices.size(); i++)
    result[i] = &storage_data[result_indices[i]];
  result[result_indices.size()] = 0;  // Null terminator.

  return result;
}

#endif  // OS_WIN

}  // namespace base
