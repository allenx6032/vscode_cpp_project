// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "command_line.h"
#include "args.h"
#include "commands.h"
#include "err.h"
#include "functions.h"
#include "input_conversion.h"
#include "label.h"
#include "label_pattern.h"
#include "metadata.h"
#include "ninja_build_writer.h"
#include "output_conversion.h"
#include "parser.h"
#include "pattern.h"
#include "runtime_deps.h"
#include "setup.h"
#include "standard_out.h"
#include "string_utils.h"
#include "substitution_writer.h"
#include "switches.h"
#include "target.h"
#include "variables.h"

namespace commands {

namespace {

// Some names exist in multiple sections, these prefixes are used for the
// internal links to disambiguate when writing markdown.
const char kCommandLinkPrefix[] = "cmd_";
const char kFunctionLinkPrefix[] = "func_";
const char kVariableLinkPrefix[] = "var_";

void PrintToplevelHelp() {
  PrintSectionHelp("Commands", "<command>", "commands");
  for (const auto& cmd : commands::GetCommands())
    PrintShortHelp(cmd.second.help_short, kCommandLinkPrefix + cmd.first);

  // Target declarations.
  PrintSectionHelp("Target declarations", "<function>", "targets");
  for (const auto& func : functions::GetFunctions()) {
    if (func.second.is_target)
      PrintShortHelp(func.second.help_short, kFunctionLinkPrefix + func.first);
  }

  // Functions.
  PrintSectionHelp("Buildfile functions", "<function>", "functions");
  for (const auto& func : functions::GetFunctions()) {
    if (!func.second.is_target)
      PrintShortHelp(func.second.help_short, kFunctionLinkPrefix + func.first);
  }

  // Built-in variables.
  PrintSectionHelp("Built-in predefined variables", "<variable>",
                   "predefined_variables");
  for (const auto& builtin : variables::GetBuiltinVariables()) {
    PrintShortHelp(builtin.second.help_short,
                   kVariableLinkPrefix + builtin.first);
  }

  // Target variables.
  PrintSectionHelp("Variables you set in targets", "<variable>",
                   "target_variables");
  for (const auto& target : variables::GetTargetVariables()) {
    PrintShortHelp(target.second.help_short,
                   kVariableLinkPrefix + target.first);
  }

  PrintSectionHelp("Other help topics", "", "other");
  PrintShortHelp("all: Print all the help at once");
  PrintShortHelp("buildargs: How build arguments work.", "buildargs");
  PrintShortHelp("dotfile: Info about the toplevel .gn file.", "dotfile");
  PrintShortHelp("execution: Build graph and execution overview.", "execution");
  PrintShortHelp("grammar: Language and grammar for GN build files.",
                 "grammar");
  PrintShortHelp(
      "input_conversion: Processing input from exec_script and read_file.",
      "io_conversion");
  PrintShortHelp("file_pattern: Matching more than one file.", "file_pattern");
  PrintShortHelp("label_pattern: Matching more than one label.",
                 "label_pattern");
  PrintShortHelp("labels: About labels.", "labels");
  PrintShortHelp("metadata_collection: About metadata and its collection.",
                 "metadata_collection");
  PrintShortHelp("ninja_rules: How Ninja build rules are named.",
                 "ninja_rules");
  PrintShortHelp("nogncheck: Annotating includes for checking.", "nogncheck");
  PrintShortHelp(
      "output_conversion: Specifies how to transform a value to output.",
      "io_conversion");
  PrintShortHelp("runtime_deps: How runtime dependency computation works.",
                 "runtime_deps");
  PrintShortHelp("source_expansion: Map sources to outputs for scripts.",
                 "source_expansion");
  PrintShortHelp("switches: Show available command-line switches.",
                 "switch_list");
}

void PrintSwitchHelp() {
  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
  bool is_markdown = cmdline->HasSwitch(switches::kMarkdown);

  // This uses "switch_list" for the tag because Markdown seems to generate
  // implicit tags for headings that match the strings, and some headings are
  // labeled "switches".
  PrintLongHelp(R"(Available global switches

  Do "gn help --the_switch_you_want_help_on" for more. Individual commands may
  take command-specific switches not listed here. See the help on your specific
  command for more.
)",
                "switch_list");

  if (is_markdown)
    OutputString("```\n", DECORATION_NONE);

  for (const auto& s : switches::GetSwitches())
    PrintShortHelp(s.second.short_help);

  if (is_markdown)
    OutputString("```\n", DECORATION_NONE);

  OutputString("\n");
}

void PrintAllHelp() {
  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
  bool is_markdown = cmdline->HasSwitch(switches::kMarkdown);

  if (is_markdown) {
    OutputString("# GN Reference\n\n");
    OutputString(
        "*This page is automatically generated from* "
        "`gn help --markdown all`.\n\n");

    // Generate our own table of contents so that we have more control
    // over what's in and out.
    OutputString("## Contents\n\n");
  }

  PrintToplevelHelp();

  OutputString("\n");

  if (is_markdown) {
    OutputString("## <a name=\"commands\"></a>Commands\n\n", DECORATION_NONE,
                 NO_ESCAPING);
  }
  for (const auto& c : commands::GetCommands())
    PrintLongHelp(c.second.help, kCommandLinkPrefix + c.first);

  if (is_markdown) {
    OutputString("## <a name=\"targets\"></a>Target declarations\n\n",
                 DECORATION_NONE, NO_ESCAPING);
  }
  for (const auto& f : functions::GetFunctions()) {
    if (f.second.is_target)
      PrintLongHelp(f.second.help, kFunctionLinkPrefix + f.first);
  }

  if (is_markdown) {
    OutputString("## <a name=\"functions\"></a>Buildfile functions\n\n",
                 DECORATION_NONE, NO_ESCAPING);
  }
  for (const auto& f : functions::GetFunctions()) {
    if (!f.second.is_target)
      PrintLongHelp(f.second.help, kFunctionLinkPrefix + f.first);
  }

  if (is_markdown) {
    OutputString(
        "## <a name=\"predefined_variables\"></a>"
        "Built-in predefined variables\n\n",
        DECORATION_NONE, NO_ESCAPING);
  }
  for (const auto& v : variables::GetBuiltinVariables())
    PrintLongHelp(v.second.help, kVariableLinkPrefix + v.first);

  if (is_markdown) {
    OutputString(
        "## <a name=\"target_variables\"></a>"
        "Variables you set in targets\n\n",
        DECORATION_NONE, NO_ESCAPING);
  }
  for (const auto& v : variables::GetTargetVariables())
    PrintLongHelp(v.second.help, kVariableLinkPrefix + v.first);

  if (is_markdown) {
    OutputString("## <a name=\"other\"></a>Other help topics\n\n",
                 DECORATION_NONE, NO_ESCAPING);
  }
  PrintLongHelp(kBuildArgs_Help, "buildargs");
  PrintLongHelp(kDotfile_Help, "dotfile");
  PrintLongHelp(kExecution_Help, "execution");
  PrintLongHelp(kGrammar_Help, "grammar");
  PrintLongHelp(kInputOutputConversion_Help, "io_conversion");
  PrintLongHelp(kFilePattern_Help, "file_pattern");
  PrintLongHelp(kLabelPattern_Help, "label_pattern");
  PrintLongHelp(kLabels_Help, "labels");
  PrintLongHelp(kMetadata_Help, "metadata_collection");
  PrintLongHelp(kNinjaRules_Help, "ninja_rules");
  PrintLongHelp(kNoGnCheck_Help, "nogncheck");
  PrintLongHelp(kRuntimeDeps_Help, "runtime_deps");
  PrintLongHelp(kSourceExpansion_Help, "source_expansion");

  PrintSwitchHelp();
}

// Prints help on the given switch. There should be no leading hyphens. Returns
// true if the switch was found and help was printed. False means the switch is
// unknown.
bool PrintHelpOnSwitch(const std::string& what) {
  const switches::SwitchInfoMap& all = switches::GetSwitches();
  switches::SwitchInfoMap::const_iterator found =
      all.find(std::string_view(what));
  if (found == all.end())
    return false;
  PrintLongHelp(found->second.long_help);
  return true;
}

// Special-case help for ambiguous "args" case.
void PrintArgsHelp() {
  PrintLongHelp(
      "The string \"args\" is both a command and a variable for action "
      "targets.\nShowing help for both...\n\n");
  PrintLongHelp(commands::kArgs_Help);
  PrintLongHelp(
      "\n----------------------------------------------------------------------"
      "---------\n\n");
  PrintLongHelp(variables::kArgs_Help);
}

}  // namespace

const char kHelp[] = "help";
const char kHelp_HelpShort[] = "help: Does what you think.";
const char kHelp_Help[] =
    R"(gn help <anything>

  Yo dawg, I heard you like help on your help so I put help on the help in the
  help.

  You can also use "all" as the parameter to get all help at once.

Switches

  --markdown
      Format output in markdown syntax.

Example

  gn help --markdown all
      Dump all help to stdout in markdown format.
)";

int RunHelp(const std::vector<std::string>& args) {
  std::string what;
  if (args.size() == 0) {
    // If no argument is specified, check for switches to allow things like
    // "gn help --args" for help on the args switch.
    const base::CommandLine::SwitchMap& switches =
        base::CommandLine::ForCurrentProcess()->GetSwitches();
    if (switches.empty()) {
      // Still nothing, show help overview.
      PrintToplevelHelp();
      return 0;
    }

    // Switch help needs to be done separately. The CommandLine will strip the
    // switch separators so --args will come out as "args" which is then
    // ambiguous with the variable named "args".
    if (!PrintHelpOnSwitch(switches.begin()->first))
      PrintToplevelHelp();
    return 0;
  } else {
    what = args[0];
  }

  std::vector<std::string_view> all_help_topics;

  // Special-case ambiguous topics.
  if (what == "args") {
    PrintArgsHelp();
    return 0;
  }

  // Check commands.
  const commands::CommandInfoMap& command_map = commands::GetCommands();
  auto found_command = command_map.find(what);
  if (found_command != command_map.end()) {
    PrintLongHelp(found_command->second.help);
    return 0;
  }
  for (const auto& entry : command_map)
    all_help_topics.push_back(entry.first);

  // Check functions.
  const functions::FunctionInfoMap& function_map = functions::GetFunctions();
  auto found_function = function_map.find(what);
  if (found_function != function_map.end())
    PrintLongHelp(found_function->second.help);
  for (const auto& entry : function_map)
    all_help_topics.push_back(entry.first);

  // Builtin variables.
  const variables::VariableInfoMap& builtin_vars =
      variables::GetBuiltinVariables();
  auto found_builtin_var = builtin_vars.find(what);
  if (found_builtin_var != builtin_vars.end())
    PrintLongHelp(found_builtin_var->second.help);
  for (const auto& entry : builtin_vars)
    all_help_topics.push_back(entry.first);

  // Target variables.
  const variables::VariableInfoMap& target_vars =
      variables::GetTargetVariables();
  auto found_target_var = target_vars.find(what);
  if (found_target_var != target_vars.end())
    PrintLongHelp(found_target_var->second.help);
  for (const auto& entry : target_vars)
    all_help_topics.push_back(entry.first);

  if (found_function != function_map.end() ||
      found_builtin_var != builtin_vars.end() ||
      found_target_var != target_vars.end())
    return 0;

  // Random other topics.
  std::map<std::string, void (*)()> random_topics;
  random_topics["all"] = PrintAllHelp;
  random_topics["execution"] = []() { PrintLongHelp(kExecution_Help); };
  random_topics["buildargs"] = []() { PrintLongHelp(kBuildArgs_Help); };
  random_topics["dotfile"] = []() { PrintLongHelp(kDotfile_Help); };
  random_topics["grammar"] = []() { PrintLongHelp(kGrammar_Help); };
  random_topics["io_conversion"] = []() {
    PrintLongHelp(kInputOutputConversion_Help);
  };
  random_topics["file_pattern"] = []() { PrintLongHelp(kFilePattern_Help); };
  random_topics["label_pattern"] = []() { PrintLongHelp(kLabelPattern_Help); };
  random_topics["labels"] = []() { PrintLongHelp(kLabels_Help); };
  random_topics["metadata_collection"] = []() {
    PrintLongHelp(kMetadata_Help);
  };
  random_topics["ninja_rules"] = []() { PrintLongHelp(kNinjaRules_Help); };
  random_topics["nogncheck"] = []() { PrintLongHelp(kNoGnCheck_Help); };
  random_topics["runtime_deps"] = []() { PrintLongHelp(kRuntimeDeps_Help); };
  random_topics["source_expansion"] = []() {
    PrintLongHelp(kSourceExpansion_Help);
  };
  random_topics["switches"] = PrintSwitchHelp;
  auto found_random_topic = random_topics.find(what);
  if (found_random_topic != random_topics.end()) {
    found_random_topic->second();
    return 0;
  }
  for (const auto& entry : random_topics)
    all_help_topics.push_back(entry.first);

  // No help on this.
  Err(Location(), "No help on \"" + what + "\".").PrintToStdout();
  std::string_view suggestion = SpellcheckString(what, all_help_topics);
  if (suggestion.empty()) {
    OutputString("Run `gn help` for a list of available topics.\n",
                 DECORATION_NONE);
  } else {
    OutputString("Did you mean `gn help " + std::string(suggestion) + "`?\n",
                 DECORATION_NONE);
  }
  return 1;
}

}  // namespace commands
