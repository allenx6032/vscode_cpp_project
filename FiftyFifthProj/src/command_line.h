// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This class works with command lines: building and parsing.
// Arguments with prefixes ('--', '-', and on Windows, '/') are switches.
// Switches will precede all other arguments without switch prefixes.
// Switches can optionally have values, delimited by '=', e.g., "-switch=value".
// An argument of "--" will terminate switch parsing during initialization,
// interpreting subsequent tokens as non-switch arguments, regardless of prefix.

// There is a singleton read-only CommandLine that represents the command line
// that the current process was started with.  It must be initialized in main().

#ifndef BASE_COMMAND_LINE_H_
#define BASE_COMMAND_LINE_H_

#include <stddef.h>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "build_config.h"

namespace base {

class FilePath;

class CommandLine {
 public:
#if defined(OS_WIN)
  // The native command line string type.
  // See StringTypeToUTF8() and UTF8ToStringType() below.
  using StringType = std::u16string;
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  using StringType = std::string;
#endif

  using CharType = StringType::value_type;
  using StringVector = std::vector<StringType>;
  using SwitchMap = std::multimap<std::string, StringType, std::less<>>;

  // A constructor for CommandLines that only carry switches and arguments.
  enum NoProgram { NO_PROGRAM };
  explicit CommandLine(NoProgram no_program);

  // Construct a new command line with |program| as argv[0].
  explicit CommandLine(const FilePath& program);

  // Construct a new command line from an argument list.
  CommandLine(int argc, const CharType* const* argv);
  explicit CommandLine(const StringVector& argv);

  // Override copy and assign to ensure |switches_by_stringpiece_| is valid.
  CommandLine(const CommandLine& other);
  CommandLine& operator=(const CommandLine& other);

  ~CommandLine();

  // Converts the platform string type to/from UTF-8.
  static std::string StringTypeToUTF8(const StringType& input);
  static StringType UTF8ToStringType(std::string_view input);

#if defined(OS_WIN)
  // By default this class will treat command-line arguments beginning with
  // slashes as switches on Windows, but not other platforms.
  //
  // If this behavior is inappropriate for your application, you can call this
  // function BEFORE initializing the current process' global command line
  // object and the behavior will be the same as Posix systems (only hyphens
  // begin switches, everything else will be an arg).
  static void set_slash_is_not_a_switch();

  // Normally when the CommandLine singleton is initialized it gets the command
  // line via the GetCommandLineW API and then uses the shell32 API
  // CommandLineToArgvW to parse the command line and convert it back to
  // argc and argv. Tests who don't want this dependency on shell32 and need
  // to honor the arguments passed in should use this function.
  static void InitUsingArgvForTesting(int argc, const char* const* argv);
#endif

  // Initialize the current process CommandLine singleton. On Windows, ignores
  // its arguments (we instead parse GetCommandLineW() directly) because we
  // don't trust the CRT's parsing of the command line, but it still must be
  // called to set up the command line. Returns false if initialization has
  // already occurred, and true otherwise. Only the caller receiving a 'true'
  // return value should take responsibility for calling Reset.
  static bool Init(int argc, const char* const* argv);

  // Destroys the current process CommandLine singleton. This is necessary if
  // you want to reset the base library to its initial state (for example, in an
  // outer library that needs to be able to terminate, and be re-initialized).
  // If Init is called only once, as in main(), Reset() is not necessary.
  // Do not call this in tests. Use base::test::ScopedCommandLine instead.
  static void Reset();

  // Get the singleton CommandLine representing the current process's
  // command line. Note: returned value is mutable, but not thread safe;
  // only mutate if you know what you're doing!
  static CommandLine* ForCurrentProcess();

  // Returns true if the CommandLine has been initialized for the given process.
  static bool InitializedForCurrentProcess();

#if defined(OS_WIN)
  static CommandLine FromString(const std::u16string& command_line);
#endif

  // Initialize from an argv vector.
  void InitFromArgv(int argc, const CharType* const* argv);
  void InitFromArgv(const StringVector& argv);

  // Constructs and returns the represented command line string.
  // CAUTION! This should be avoided on POSIX because quoting behavior is
  // unclear.
  StringType GetCommandLineString() const {
    return GetCommandLineStringInternal(false);
  }

#if defined(OS_WIN)
  // Constructs and returns the represented command line string. Assumes the
  // command line contains placeholders (eg, %1) and quotes any program or
  // argument with a '%' in it. This should be avoided unless the placeholder is
  // required by an external interface (eg, the Windows registry), because it is
  // not generally safe to replace it with an arbitrary string. If possible,
  // placeholders should be replaced *before* converting the command line to a
  // string.
  StringType GetCommandLineStringWithPlaceholders() const {
    return GetCommandLineStringInternal(true);
  }
#endif

  // Constructs and returns the represented arguments string.
  // CAUTION! This should be avoided on POSIX because quoting behavior is
  // unclear.
  StringType GetArgumentsString() const {
    return GetArgumentsStringInternal(false);
  }

#if defined(OS_WIN)
  // Constructs and returns the represented arguments string. Assumes the
  // command line contains placeholders (eg, %1) and quotes any argument with a
  // '%' in it. This should be avoided unless the placeholder is required by an
  // external interface (eg, the Windows registry), because it is not generally
  // safe to replace it with an arbitrary string. If possible, placeholders
  // should be replaced *before* converting the arguments to a string.
  StringType GetArgumentsStringWithPlaceholders() const {
    return GetArgumentsStringInternal(true);
  }
#endif

  // Returns the original command line string as a vector of strings.
  const StringVector& argv() const { return argv_; }

  // Get and Set the program part of the command line string (the first item).
  FilePath GetProgram() const;
  void SetProgram(const FilePath& program);

  // Enables/disables the parsing of switches for future argument appending.
  // True by default, but can be set to false to ensure that no re-ordering
  // is done.
  void SetParseSwitches(bool parse_switches) {
    parse_switches_ = parse_switches;
  }

  // Returns true if this command line contains the given switch.
  // Switch names must be lowercase.
  // The second override provides an optimized version to avoid inlining codegen
  // at every callsite to find the length of the constant and construct a
  // std::string_view.
  bool HasSwitch(std::string_view switch_string) const;
  bool HasSwitch(const char switch_constant[]) const;

  // Returns the last value associated with the given switch. If the switch has
  // no value or isn't present, this method returns the empty string. Switch
  // names must be lowercase.
  //
  // The "string" version returns an 8-bit representation. On Windows, this will
  // convert to UTF-8, on 8-bit systems it will return the raw input.
  std::string GetSwitchValueString(std::string_view switch_string) const;
  FilePath GetSwitchValuePath(std::string_view switch_string) const;
  StringType GetSwitchValueNative(std::string_view switch_string) const;

  // Returns all values associated with the given switch.
  std::vector<std::string> GetSwitchValueStrings(
      std::string_view switch_string) const;
  std::vector<StringType> GetSwitchValuesNative(
      std::string_view switch_string) const;

  // Get a copy of all switches, along with their values.
  const SwitchMap& GetSwitches() const { return switches_; }

  // Append a switch [with optional value] to the command line.
  // Note: Switches will precede arguments regardless of appending order.
  //
  // The version that takes an 8-bit switch value converts from UTF-8 on
  // Windows.
  void AppendSwitch(const std::string& switch_string);
  void AppendSwitchPath(const std::string& switch_string, const FilePath& path);
  void AppendSwitchNative(const std::string& switch_string,
                          const StringType& value);
  void AppendSwitch(const std::string& switch_string, const std::string& value);

  // Copy a set of switches (and any values) from another command line.
  // Commonly used when launching a subprocess.
  void CopySwitchesFrom(const CommandLine& source,
                        const char* const switches[],
                        size_t count);

  // Get the remaining arguments to the command.
  StringVector GetArgs() const;

  // Append an argument to the command line. Note that the argument is quoted
  // properly such that it is interpreted as one argument to the target command.
  // AppendArg is primarily for ASCII; non-ASCII input is interpreted as UTF-8.
  // Note: Switches will precede arguments regardless of appending order.
  void AppendArg(const std::string& value);
  void AppendArgPath(const FilePath& value);
  void AppendArgNative(const StringType& value);

  // Append the switches and arguments from another command line to this one.
  // If |include_program| is true, include |other|'s program as well.
  void AppendArguments(const CommandLine& other, bool include_program);

  // Insert a command before the current command.
  // Common for debuggers, like "gdb --args".
  void PrependWrapper(const StringType& wrapper);

#if defined(OS_WIN)
  // Initialize by parsing the given command line string.
  // The program name is assumed to be the first item in the string.
  void ParseFromString(const std::u16string& command_line);
#endif

 private:
  // Disallow default constructor; a program name must be explicitly specified.
  CommandLine() = delete;
  // Allow the copy constructor. A common pattern is to copy of the current
  // process's command line and then add some flags to it. For example:
  //   CommandLine cl(*CommandLine::ForCurrentProcess());
  //   cl.AppendSwitch(...);

  // Internal version of GetCommandLineString. If |quote_placeholders| is true,
  // also quotes parts with '%' in them.
  StringType GetCommandLineStringInternal(bool quote_placeholders) const;

  // Internal version of GetArgumentsString. If |quote_placeholders| is true,
  // also quotes parts with '%' in them.
  StringType GetArgumentsStringInternal(bool quote_placeholders) const;

  // The singleton CommandLine representing the current process's command line.
  static CommandLine* current_process_commandline_;

  // The argv array: { program, [(--|-|/)switch[=value]]*, [--], [argument]* }
  StringVector argv_;

  // Parsed-out switch keys and values.
  SwitchMap switches_;

  // The index after the program and switches, any arguments start here.
  size_t begin_args_;

  // Whether or not to parse arguments that look like switches as switches.
  bool parse_switches_;
};

}  // namespace base

#endif  // BASE_COMMAND_LINE_H_
