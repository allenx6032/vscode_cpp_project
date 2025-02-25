// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "command_line.h"

#include <algorithm>
#include <iterator>
#include <ostream>
#include <string_view>

#include "file_path.h"
#include "logging.h"
#include "stl_util.h"
#include "string_split.h"
#include "string_tokenizer.h"
#include "string_util.h"
#include "utf_string_conversions.h"
#include "build_config.h"

#if defined(OS_WIN)
#include <windows.h>

#include <shellapi.h>
#endif

namespace base {

CommandLine* CommandLine::current_process_commandline_ = nullptr;

namespace {

const CommandLine::CharType kSwitchTerminator[] = FILE_PATH_LITERAL("--");
const CommandLine::CharType kSwitchValueSeparator[] = FILE_PATH_LITERAL("=");

// Since we use a lazy match, make sure that longer versions (like "--") are
// listed before shorter versions (like "-") of similar prefixes.
#if defined(OS_WIN)
// By putting slash last, we can control whether it is treaded as a switch
// value by changing the value of switch_prefix_count to be one less than
// the array size.
const CommandLine::CharType* const kSwitchPrefixes[] = {u"--", u"-", u"/"};
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
// Unixes don't use slash as a switch.
const CommandLine::CharType* const kSwitchPrefixes[] = {"--", "-"};
#endif
size_t switch_prefix_count = std::size(kSwitchPrefixes);

size_t GetSwitchPrefixLength(const CommandLine::StringType& string) {
  for (size_t i = 0; i < switch_prefix_count; ++i) {
    CommandLine::StringType prefix(kSwitchPrefixes[i]);
    if (string.compare(0, prefix.length(), prefix) == 0)
      return prefix.length();
  }
  return 0;
}

// Fills in |switch_string| and |switch_value| if |string| is a switch.
// This will preserve the input switch prefix in the output |switch_string|.
bool IsSwitch(const CommandLine::StringType& string,
              CommandLine::StringType* switch_string,
              CommandLine::StringType* switch_value) {
  switch_string->clear();
  switch_value->clear();
  size_t prefix_length = GetSwitchPrefixLength(string);
  if (prefix_length == 0 || prefix_length == string.length())
    return false;

  const size_t equals_position = string.find(kSwitchValueSeparator);
  *switch_string = string.substr(0, equals_position);
  if (equals_position != CommandLine::StringType::npos)
    *switch_value = string.substr(equals_position + 1);
  return true;
}

// Append switches and arguments, keeping switches before arguments
// if handle_switches is true.
void AppendSwitchesAndArguments(CommandLine* command_line,
                                const CommandLine::StringVector& argv,
                                bool handle_switches) {
  bool parse_switches = handle_switches;
  for (size_t i = 1; i < argv.size(); ++i) {
    CommandLine::StringType arg = argv[i];
#if defined(OS_WIN)
    TrimWhitespace(arg, TRIM_ALL, &arg);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    TrimWhitespaceASCII(arg, TRIM_ALL, &arg);
#endif

    CommandLine::StringType switch_string;
    CommandLine::StringType switch_value;
    parse_switches &= (arg != kSwitchTerminator);
    if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
#if defined(OS_WIN)
      command_line->AppendSwitchNative(UTF16ToASCII(switch_string),
                                       switch_value);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
      command_line->AppendSwitchNative(switch_string, switch_value);
#else
#error Unsupported platform
#endif
    } else {
      command_line->AppendArgNative(arg);
    }
  }
}

#if defined(OS_WIN)
// Quote a string as necessary for CommandLineToArgvW compatibility *on
// Windows*.
std::u16string QuoteForCommandLineToArgvW(const std::u16string& arg,
                                          bool quote_placeholders) {
  // We follow the quoting rules of CommandLineToArgvW.
  // http://msdn.microsoft.com/en-us/library/17w5ykft.aspx
  std::u16string quotable_chars(u" \\\"");
  // We may also be required to quote '%', which is commonly used in a command
  // line as a placeholder. (It may be substituted for a string with spaces.)
  if (quote_placeholders)
    quotable_chars.push_back('%');
  if (arg.find_first_of(quotable_chars) == std::u16string::npos) {
    // No quoting necessary.
    return arg;
  }

  std::u16string out;
  out.push_back('"');
  for (size_t i = 0; i < arg.size(); ++i) {
    if (arg[i] == '\\') {
      // Find the extent of this run of backslashes.
      size_t start = i, end = start + 1;
      for (; end < arg.size() && arg[end] == '\\'; ++end) {
      }
      size_t backslash_count = end - start;

      // Backslashes are escapes only if the run is followed by a double quote.
      // Since we also will end the string with a double quote, we escape for
      // either a double quote or the end of the string.
      if (end == arg.size() || arg[end] == '"') {
        // To quote, we need to output 2x as many backslashes.
        backslash_count *= 2;
      }
      for (size_t j = 0; j < backslash_count; ++j)
        out.push_back('\\');

      // Advance i to one before the end to balance i++ in loop.
      i = end - 1;
    } else if (arg[i] == '"') {
      out.push_back('\\');
      out.push_back('"');
    } else {
      out.push_back(arg[i]);
    }
  }
  out.push_back('"');

  return out;
}
#endif

}  // namespace

CommandLine::CommandLine(NoProgram no_program)
    : argv_(1), begin_args_(1), parse_switches_(true) {}

CommandLine::CommandLine(const FilePath& program)
    : argv_(1), begin_args_(1), parse_switches_(true) {
  SetProgram(program);
}

CommandLine::CommandLine(int argc, const CommandLine::CharType* const* argv)
    : argv_(1), begin_args_(1), parse_switches_(true) {
  InitFromArgv(argc, argv);
}

CommandLine::CommandLine(const StringVector& argv)
    : argv_(1), begin_args_(1), parse_switches_(true) {
  InitFromArgv(argv);
}

CommandLine::CommandLine(const CommandLine& other) = default;

CommandLine& CommandLine::operator=(const CommandLine& other) = default;

CommandLine::~CommandLine() = default;

#if defined(OS_WIN)

// static
std::string CommandLine::StringTypeToUTF8(const StringType& input) {
  return UTF16ToUTF8(input);
}

// static
CommandLine::StringType CommandLine::UTF8ToStringType(std::string_view input) {
  return UTF8ToUTF16(input);
}

// static
void CommandLine::set_slash_is_not_a_switch() {
  // The last switch prefix should be slash, so adjust the size to skip it.
  DCHECK(std::u16string_view(kSwitchPrefixes[std::size(kSwitchPrefixes) - 1]) ==
         std::u16string_view(u"/"));
  switch_prefix_count = std::size(kSwitchPrefixes) - 1;
}

// static
void CommandLine::InitUsingArgvForTesting(int argc, const char* const* argv) {
  DCHECK(!current_process_commandline_);
  current_process_commandline_ = new CommandLine(NO_PROGRAM);
  // On Windows we need to convert the command line arguments to std::u16string.
  base::CommandLine::StringVector argv_vector;
  for (int i = 0; i < argc; ++i)
    argv_vector.push_back(UTF8ToUTF16(argv[i]));
  current_process_commandline_->InitFromArgv(argv_vector);
}

#else

// static
std::string CommandLine::StringTypeToUTF8(const StringType& input) {
  return input;
}

// static
CommandLine::StringType CommandLine::UTF8ToStringType(std::string_view input) {
  return CommandLine::StringType(input);
}

#endif

// static
bool CommandLine::Init(int argc, const char* const* argv) {
  if (current_process_commandline_) {
    // If this is intentional, Reset() must be called first. If we are using
    // the shared build mode, we have to share a single object across multiple
    // shared libraries.
    return false;
  }

  current_process_commandline_ = new CommandLine(NO_PROGRAM);
#if defined(OS_WIN)
  current_process_commandline_->ParseFromString(
      reinterpret_cast<const char16_t*>(::GetCommandLineW()));
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  current_process_commandline_->InitFromArgv(argc, argv);
#else
#error Unsupported platform
#endif

  return true;
}

// static
void CommandLine::Reset() {
  DCHECK(current_process_commandline_);
  delete current_process_commandline_;
  current_process_commandline_ = nullptr;
}

// static
CommandLine* CommandLine::ForCurrentProcess() {
  DCHECK(current_process_commandline_);
  return current_process_commandline_;
}

// static
bool CommandLine::InitializedForCurrentProcess() {
  return !!current_process_commandline_;
}

#if defined(OS_WIN)
// static
CommandLine CommandLine::FromString(const std::u16string& command_line) {
  CommandLine cmd(NO_PROGRAM);
  cmd.ParseFromString(command_line);
  return cmd;
}
#endif

void CommandLine::InitFromArgv(int argc,
                               const CommandLine::CharType* const* argv) {
  StringVector new_argv;
  for (int i = 0; i < argc; ++i)
    new_argv.push_back(argv[i]);
  InitFromArgv(new_argv);
}

void CommandLine::InitFromArgv(const StringVector& argv) {
  argv_ = StringVector(1);
  switches_.clear();
  begin_args_ = 1;
  SetProgram(argv.empty() ? FilePath() : FilePath(argv[0]));
  AppendSwitchesAndArguments(this, argv, parse_switches_);
}

FilePath CommandLine::GetProgram() const {
  return FilePath(argv_[0]);
}

void CommandLine::SetProgram(const FilePath& program) {
#if defined(OS_WIN)
  TrimWhitespace(program.value(), TRIM_ALL, &argv_[0]);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  TrimWhitespaceASCII(program.value(), TRIM_ALL, &argv_[0]);
#else
#error Unsupported platform
#endif
}

bool CommandLine::HasSwitch(std::string_view switch_string) const {
  DCHECK_EQ(ToLowerASCII(switch_string), switch_string);
  return ContainsKey(switches_, switch_string);
}

bool CommandLine::HasSwitch(const char switch_constant[]) const {
  return HasSwitch(std::string_view(switch_constant));
}

std::string CommandLine::GetSwitchValueString(
    std::string_view switch_string) const {
  return StringTypeToUTF8(GetSwitchValueNative(switch_string));
}

FilePath CommandLine::GetSwitchValuePath(std::string_view switch_string) const {
  return FilePath(GetSwitchValueNative(switch_string));
}

CommandLine::StringType CommandLine::GetSwitchValueNative(
    std::string_view switch_string) const {
  DCHECK_EQ(ToLowerASCII(switch_string), switch_string);

  // There can be multiple matches, we want to find the last one.
  auto iter = switches_.upper_bound(switch_string);
  if (iter == switches_.begin())
    return StringType();

  // We want the item right before the upper bound, if it's a match.
  --iter;
  if (iter->first == switch_string)
    return iter->second;
  return StringType();
}

std::vector<std::string> CommandLine::GetSwitchValueStrings(
    std::string_view switch_string) const {
  std::vector<StringType> matches = GetSwitchValuesNative(switch_string);

  std::vector<std::string> result;
  result.reserve(matches.size());

  for (const StringType& cur : matches) {
    result.push_back(StringTypeToUTF8(cur));
  }
  return result;
}

std::vector<CommandLine::StringType> CommandLine::GetSwitchValuesNative(
    std::string_view switch_string) const {
  std::vector<StringType> result;

  auto [iter, end] = switches_.equal_range(switch_string);
  while (iter != end) {
    result.push_back(iter->second);
    ++iter;
  }

  return result;
}

void CommandLine::AppendSwitch(const std::string& switch_string) {
  AppendSwitchNative(switch_string, StringType());
}

void CommandLine::AppendSwitchPath(const std::string& switch_string,
                                   const FilePath& path) {
  AppendSwitchNative(switch_string, path.value());
}

void CommandLine::AppendSwitchNative(const std::string& switch_string,
                                     const CommandLine::StringType& value) {
#if defined(OS_WIN)
  const std::string switch_key = ToLowerASCII(switch_string);
  StringType combined_switch_string(ASCIIToUTF16(switch_key));
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  const std::string& switch_key = switch_string;
  StringType combined_switch_string(switch_key);
#endif

  size_t prefix_length = GetSwitchPrefixLength(combined_switch_string);
  switches_.insert(make_pair(switch_key.substr(prefix_length), value));

  // Preserve existing switch prefixes in |argv_|; only append one if necessary.
  if (prefix_length == 0)
    combined_switch_string = kSwitchPrefixes[0] + combined_switch_string;
  if (!value.empty())
    combined_switch_string += kSwitchValueSeparator + value;
  // Append the switch and update the switches/arguments divider |begin_args_|.
  argv_.insert(argv_.begin() + begin_args_++, combined_switch_string);
}

void CommandLine::AppendSwitch(const std::string& switch_string,
                               const std::string& value_string) {
  AppendSwitchNative(switch_string, UTF8ToStringType(value_string));
}

void CommandLine::CopySwitchesFrom(const CommandLine& source,
                                   const char* const switches[],
                                   size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (source.HasSwitch(switches[i]))
      AppendSwitchNative(switches[i], source.GetSwitchValueNative(switches[i]));
  }
}

CommandLine::StringVector CommandLine::GetArgs() const {
  // Gather all arguments after the last switch (may include kSwitchTerminator).
  StringVector args(argv_.begin() + begin_args_, argv_.end());
  // Erase only the first kSwitchTerminator (maybe "--" is a legitimate page?)
  StringVector::iterator switch_terminator =
      std::find(args.begin(), args.end(), kSwitchTerminator);
  if (switch_terminator != args.end())
    args.erase(switch_terminator);
  return args;
}

void CommandLine::AppendArg(const std::string& value) {
#if defined(OS_WIN)
  DCHECK(IsStringUTF8(value));
  AppendArgNative(UTF8ToUTF16(value));
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  AppendArgNative(value);
#else
#error Unsupported platform
#endif
}

void CommandLine::AppendArgPath(const FilePath& path) {
  AppendArgNative(path.value());
}

void CommandLine::AppendArgNative(const CommandLine::StringType& value) {
  argv_.push_back(value);
}

void CommandLine::AppendArguments(const CommandLine& other,
                                  bool include_program) {
  if (include_program)
    SetProgram(other.GetProgram());
  AppendSwitchesAndArguments(this, other.argv(), parse_switches_);
}

void CommandLine::PrependWrapper(const CommandLine::StringType& wrapper) {
  if (wrapper.empty())
    return;
  // Split the wrapper command based on whitespace (with quoting).
  using CommandLineTokenizer =
      StringTokenizerT<StringType, StringType::const_iterator>;
  CommandLineTokenizer tokenizer(wrapper, FILE_PATH_LITERAL(" "));
  tokenizer.set_quote_chars(FILE_PATH_LITERAL("'\""));
  std::vector<StringType> wrapper_argv;
  while (tokenizer.GetNext())
    wrapper_argv.emplace_back(tokenizer.token());

  // Prepend the wrapper and update the switches/arguments |begin_args_|.
  argv_.insert(argv_.begin(), wrapper_argv.begin(), wrapper_argv.end());
  begin_args_ += wrapper_argv.size();
}

#if defined(OS_WIN)
void CommandLine::ParseFromString(const std::u16string& command_line) {
  std::u16string command_line_string;
  TrimWhitespace(command_line, TRIM_ALL, &command_line_string);
  if (command_line_string.empty())
    return;

  int num_args = 0;
  char16_t** args = NULL;
  args = reinterpret_cast<char16_t**>(::CommandLineToArgvW(
      reinterpret_cast<LPCWSTR>(command_line_string.c_str()), &num_args));

  DPLOG_IF(FATAL, !args) << "CommandLineToArgvW failed on command line: "
                         << UTF16ToUTF8(command_line);
  InitFromArgv(num_args, args);
  LocalFree(args);
}
#endif

CommandLine::StringType CommandLine::GetCommandLineStringInternal(
    bool quote_placeholders) const {
  StringType string(argv_[0]);
#if defined(OS_WIN)
  string = QuoteForCommandLineToArgvW(string, quote_placeholders);
#endif
  StringType params(GetArgumentsStringInternal(quote_placeholders));
  if (!params.empty()) {
    string.append(StringType(FILE_PATH_LITERAL(" ")));
    string.append(params);
  }
  return string;
}

CommandLine::StringType CommandLine::GetArgumentsStringInternal(
    bool quote_placeholders) const {
  StringType params;
  // Append switches and arguments.
  bool parse_switches = parse_switches_;
  for (size_t i = 1; i < argv_.size(); ++i) {
    StringType arg = argv_[i];
    StringType switch_string;
    StringType switch_value;
    parse_switches &= arg != kSwitchTerminator;
    if (i > 1)
      params.append(StringType(FILE_PATH_LITERAL(" ")));
    if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
      params.append(switch_string);
      if (!switch_value.empty()) {
#if defined(OS_WIN)
        switch_value =
            QuoteForCommandLineToArgvW(switch_value, quote_placeholders);
#endif
        params.append(kSwitchValueSeparator + switch_value);
      }
    } else {
#if defined(OS_WIN)
      arg = QuoteForCommandLineToArgvW(arg, quote_placeholders);
#endif
      params.append(arg);
    }
  }
  return params;
}

}  // namespace base
