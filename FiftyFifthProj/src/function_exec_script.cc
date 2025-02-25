// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "command_line.h"
#include "file_util.h"
#include "logging.h"
#include "string_number_conversions.h"
#include "utf_string_conversions.h"
#include "err.h"
#include "exec_process.h"
#include "filesystem_utils.h"
#include "functions.h"
#include "input_conversion.h"
#include "input_file.h"
#include "parse_tree.h"
#include "scheduler.h"
#include "trace.h"
#include "value.h"
#include "build_config.h"
#include "ticks.h"

namespace functions {

namespace {

bool CheckExecScriptPermissions(const BuildSettings* build_settings,
                                const FunctionCallNode* function,
                                Err* err) {
  const SourceFileSet* whitelist = build_settings->exec_script_whitelist();
  if (!whitelist)
    return true;  // No whitelist specified, don't check.

  LocationRange function_range = function->GetRange();
  if (!function_range.begin().file())
    return true;  // No file, might be some internal thing, implicitly pass.

  if (whitelist->find(function_range.begin().file()->name()) !=
      whitelist->end())
    return true;  // Whitelisted, this is OK.

  // Disallowed case.
  *err = Err(
      function, "Disallowed exec_script call.",
      "The use of exec_script use is restricted in this build. exec_script\n"
      "is discouraged because it can slow down the GN run and is easily\n"
      "abused.\n"
      "\n"
      "Generally nontrivial work should be done as build steps rather than\n"
      "when GN is run. For example, if you need to compute a nontrivial\n"
      "preprocessor define, it will be better to have an action target\n"
      "generate a header containing the define rather than blocking the GN\n"
      "run to compute the value.\n"
      "\n"
      "The allowed callers of exec_script is maintained in the \"//.gn\" file\n"
      "if you need to modify the whitelist.");
  return false;
}

}  // namespace

const char kExecScript[] = "exec_script";
const char kExecScript_HelpShort[] =
    "exec_script: Synchronously run a script and return the output.";
const char kExecScript_Help[] =
    R"(exec_script: Synchronously run a script and return the output.

  exec_script(filename,
              arguments = [],
              input_conversion = "",
              file_dependencies = [])

  Runs the given script, returning the stdout of the script. The build
  generation will fail if the script does not exist or returns a nonzero exit
  code.

  The current directory when executing the script will be the root build
  directory. If you are passing file names, you will want to use the
  rebase_path() function to make file names relative to this path (see "gn help
  rebase_path").

  The default script interpreter is Python ("python" on POSIX, "python.exe" or
  "python.bat" on Windows). This can be configured by the script_executable
  variable, see "gn help dotfile".

Arguments:

  filename:
      File name of script to execute. Non-absolute names will be treated as
      relative to the current build file.

  arguments:
      A list of strings to be passed to the script as arguments. May be
      unspecified or the empty list which means no arguments.

  input_conversion:
      Controls how the file is read and parsed. See `gn help io_conversion`.

      If unspecified, defaults to the empty string which causes the script
      result to be discarded. exec script will return None.

  dependencies:
      (Optional) A list of files that this script reads or otherwise depends
      on. These dependencies will be added to the build result such that if any
      of them change, the build will be regenerated and the script will be
      re-run.

      The script itself will be an implicit dependency so you do not need to
      list it.

Example

  all_lines = exec_script(
      "myscript.py", [some_input], "list lines",
      [ rebase_path("data_file.txt", root_build_dir) ])

  # This example just calls the script with no arguments and discards the
  # result.
  exec_script("//foo/bar/myscript.py")
)";

Value RunExecScript(Scope* scope,
                    const FunctionCallNode* function,
                    const std::vector<Value>& args,
                    Err* err) {
  if (args.size() < 1 || args.size() > 4) {
    *err = Err(function->function(), "Wrong number of arguments to exec_script",
               "I expected between one and four arguments.");
    return Value();
  }

  const Settings* settings = scope->settings();
  const BuildSettings* build_settings = settings->build_settings();
  const SourceDir& cur_dir = scope->GetSourceDir();

  if (!CheckExecScriptPermissions(build_settings, function, err))
    return Value();

  // Find the script to run.
  std::string script_source_path = cur_dir.ResolveRelativeAs(
      true, args[0], err,
      scope->settings()->build_settings()->root_path_utf8());
  if (err->has_error())
    return Value();
  base::FilePath script_path =
      build_settings->GetFullPath(script_source_path, true);
  if (!build_settings->secondary_source_path().empty() &&
      !base::PathExists(script_path)) {
    // Fall back to secondary source root when the file doesn't exist.
    script_path =
        build_settings->GetFullPathSecondary(script_source_path, true);
  }

  ScopedTrace trace(TraceItem::TRACE_SCRIPT_EXECUTE, script_source_path);
  trace.SetToolchain(settings->toolchain_label());

  // Add all dependencies of this script, including the script itself, to the
  // build deps.
  g_scheduler->AddGenDependency(script_path);
  if (args.size() == 4) {
    const Value& deps_value = args[3];
    if (!deps_value.VerifyTypeIs(Value::LIST, err))
      return Value();

    for (const auto& dep : deps_value.list_value()) {
      if (!dep.VerifyTypeIs(Value::STRING, err))
        return Value();
      g_scheduler->AddGenDependency(build_settings->GetFullPath(
          cur_dir.ResolveRelativeAs(
              true, dep, err,
              scope->settings()->build_settings()->root_path_utf8()),
          true));
      if (err->has_error())
        return Value();
    }
  }

  // Make the command line.
  base::CommandLine cmdline(base::CommandLine::NO_PROGRAM);

  // CommandLine tries to interpret arguments by default.  Disable that so
  // that the arguments will be passed through exactly as specified.
  cmdline.SetParseSwitches(false);

  // If an interpreter path is set, initialize it as the first entry and
  // pass script_path as the first argument. Otherwise, set the
  // program to script_path directly.
  const base::FilePath& interpreter_path = build_settings->python_path();
  if (!interpreter_path.empty()) {
    cmdline.SetProgram(interpreter_path);
    cmdline.AppendArgPath(script_path);
  } else {
    cmdline.SetProgram(script_path);
  }

  if (args.size() >= 2) {
    // Optional command-line arguments to the script.
    const Value& script_args = args[1];
    if (!script_args.VerifyTypeIs(Value::LIST, err))
      return Value();
    for (const auto& arg : script_args.list_value()) {
      if (!arg.VerifyTypeIs(Value::STRING, err))
        return Value();
      cmdline.AppendArg(arg.string_value());
    }
  }

  // Log command line for debugging help.
  trace.SetCommandLine(cmdline);
  Ticks begin_exec = 0;
  if (g_scheduler->verbose_logging()) {
#if defined(OS_WIN)
    g_scheduler->Log("Executing",
                     base::UTF16ToUTF8(cmdline.GetCommandLineString()));
#else
    g_scheduler->Log("Executing", cmdline.GetCommandLineString());
#endif
    begin_exec = TicksNow();
  }

  base::FilePath startup_dir =
      build_settings->GetFullPath(build_settings->build_dir());
  // The first time a build is run, no targets will have been written so the
  // build output directory won't exist. We need to make sure it does before
  // running any scripts with this as its startup directory, although it will
  // be relatively rare that the directory won't exist by the time we get here.
  //
  // If this shows up on benchmarks, we can cache whether we've done this
  // or not and skip creating the directory.
  base::CreateDirectory(startup_dir);

  // Execute the process.
  // TODO(brettw) set the environment block.
  std::string output;
  std::string stderr_output;
  int exit_code = 0;
  {
    if (!internal::ExecProcess(cmdline, startup_dir, &output, &stderr_output,
                               &exit_code)) {
      *err = Err(function->function(), "Could not execute interpreter.",
                 "I was trying to execute \"" +
                     FilePathToUTF8(interpreter_path) + "\".");
      return Value();
    }
  }
  if (g_scheduler->verbose_logging()) {
    g_scheduler->Log(
        "Executing",
        script_source_path + " took " +
            base::Int64ToString(
                TicksDelta(TicksNow(), begin_exec).InMilliseconds()) +
            "ms");
  }

  if (exit_code != 0) {
    std::string msg =
        "Current dir: " + FilePathToUTF8(startup_dir) +
        "\nCommand: " + FilePathToUTF8(cmdline.GetCommandLineString()) +
        "\nReturned " + base::IntToString(exit_code);
    if (!output.empty())
      msg += " and printed out:\n\n" + output;
    else
      msg += ".";
    if (!stderr_output.empty())
      msg += "\nstderr:\n\n" + stderr_output;

    *err =
        Err(function->function(), "Script returned non-zero exit code.", msg);
    return Value();
  }

  // Default to None value for the input conversion if unspecified.
  return ConvertInputToValue(scope->settings(), output, function,
                             args.size() >= 3 ? args[2] : Value(), err);
}

}  // namespace functions
