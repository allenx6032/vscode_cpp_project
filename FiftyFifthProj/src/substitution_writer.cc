// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "substitution_writer.h"

#include "build_settings.h"
#include "c_substitution_type.h"
#include "escape.h"
#include "filesystem_utils.h"
#include "output_file.h"
#include "rust_substitution_type.h"
#include "rust_tool.h"
#include "settings.h"
#include "source_file.h"
#include "string_utils.h"
#include "substitution_list.h"
#include "substitution_pattern.h"
#include "target.h"

namespace {

// Sets the given directory string to the destination, trimming any trailing
// slash from the directory (SourceDirs and OutputFiles representing
// directories will end in a trailing slash). If the directory is empty,
// it will be replaced with a ".".
void SetDirOrDotWithNoSlash(const std::string& dir, std::string* dest) {
  if (!dir.empty() && dir[dir.size() - 1] == '/')
    dest->assign(dir.data(), dir.size() - 1);
  else
    dest->assign(dir);

  if (dest->empty())
    dest->push_back('.');
}

}  // namespace

const char kSourceExpansion_Help[] =
    R"(How Source Expansion Works

  Source expansion is used for the action_foreach and copy target types to map
  source file names to output file names or arguments.

  To perform source expansion in the outputs, GN maps every entry in the
  sources to every entry in the outputs list, producing the cross product of
  all combinations, expanding placeholders (see below).

  Source expansion in the args works similarly, but performing the placeholder
  substitution produces a different set of arguments for each invocation of the
  script.

  If no placeholders are found, the outputs or args list will be treated as a
  static list of literal file names that do not depend on the sources.

  See "gn help copy" and "gn help action_foreach" for more on how this is
  applied.

Placeholders

  This section discusses only placeholders for actions. There are other
  placeholders used in the definition of tools. See "gn help tool" for those.

  {{source}}
      The name of the source file including directory (*). This will generally
      be used for specifying inputs to a script in the "args" variable.
        "//foo/bar/baz.txt" => "../../foo/bar/baz.txt"

  {{source_file_part}}
      The file part of the source including the extension.
        "//foo/bar/baz.txt" => "baz.txt"

  {{source_name_part}}
      The filename part of the source file with no directory or extension. This
      will generally be used for specifying a transformation from a source file
      to a destination file with the same name but different extension.
        "//foo/bar/baz.txt" => "baz"

  {{source_dir}}
      The directory (*) containing the source file with no trailing slash.
        "//foo/bar/baz.txt" => "../../foo/bar"

  {{source_root_relative_dir}}
      The path to the source file's directory relative to the source root, with
      no leading "//" or trailing slashes. If the path is system-absolute,
      (beginning in a single slash) this will just return the path with no
      trailing slash. This value will always be the same, regardless of whether
      it appears in the "outputs" or "args" section.
        "//foo/bar/baz.txt" => "foo/bar"

  {{source_gen_dir}}
      The generated file directory (*) corresponding to the source file's path.
      This will be different than the target's generated file directory if the
      source file is in a different directory than the BUILD.gn file.
        "//foo/bar/baz.txt" => "gen/foo/bar"

  {{source_out_dir}}
      The object file directory (*) corresponding to the source file's path,
      relative to the build directory. this us be different than the target's
      out directory if the source file is in a different directory than the
      build.gn file.
        "//foo/bar/baz.txt" => "obj/foo/bar"

  {{source_target_relative}}
      The path to the source file relative to the target's directory. This will
      generally be used for replicating the source directory layout in the
      output directory. This can only be used in actions and bundle_data
      targets. It is an error to use in process_file_template where there is no
      "target".
        "//foo/bar/baz.txt" => "baz.txt"

(*) Note on directories

  Paths containing directories (except the source_root_relative_dir) will be
  different depending on what context the expansion is evaluated in. Generally
  it should "just work" but it means you can't concatenate strings containing
  these values with reasonable results.

  Details: source expansions can be used in the "outputs" variable, the "args"
  variable, and in calls to "process_file_template". The "args" are passed to a
  script which is run from the build directory, so these directories will
  relative to the build directory for the script to find. In the other cases,
  the directories will be source- absolute (begin with a "//") because the
  results of those expansions will be handled by GN internally.

Examples

  Non-varying outputs:
    action("hardcoded_outputs") {
      sources = [ "input1.idl", "input2.idl" ]
      outputs = [ "$target_out_dir/output1.dat",
                  "$target_out_dir/output2.dat" ]
    }
  The outputs in this case will be the two literal files given.

  Varying outputs:
    action_foreach("varying_outputs") {
      sources = [ "input1.idl", "input2.idl" ]
      outputs = [ "{{source_gen_dir}}/{{source_name_part}}.h",
                  "{{source_gen_dir}}/{{source_name_part}}.cc" ]
    }
  Performing source expansion will result in the following output names:
    //out/Debug/obj/mydirectory/input1.h
    //out/Debug/obj/mydirectory/input1.cc
    //out/Debug/obj/mydirectory/input2.h
    //out/Debug/obj/mydirectory/input2.cc
)";

// static
void SubstitutionWriter::WriteWithNinjaVariables(
    const SubstitutionPattern& pattern,
    const EscapeOptions& escape_options,
    std::ostream& out) {
  // The result needs to be quoted as if it was one string, but the $ for
  // the inserted Ninja variables can't be escaped. So write to a buffer with
  // no quoting, and then quote the whole thing if necessary.
  EscapeOptions no_quoting(escape_options);
  no_quoting.inhibit_quoting = true;

  bool needs_quotes = false;
  std::string result;
  for (const auto& range : pattern.ranges()) {
    if (range.type == &SubstitutionLiteral) {
      result.append(EscapeString(range.literal, no_quoting, &needs_quotes));
    } else {
      result.append("${");
      result.append(range.type->ninja_name);
      result.append("}");
    }
  }

  if (needs_quotes && !escape_options.inhibit_quoting)
    out << "\"" << result << "\"";
  else
    out << result;
}

// static
void SubstitutionWriter::GetListAsSourceFiles(const SubstitutionList& list,
                                              std::vector<SourceFile>* output) {
  for (const auto& pattern : list.list()) {
    CHECK(pattern.ranges().size() == 1 &&
          pattern.ranges()[0].type == &SubstitutionLiteral)
        << "The substitution pattern \"" << pattern.AsString()
        << "\" was expected to be a literal with no {{substitutions}}.";
    const std::string& literal = pattern.ranges()[0].literal;
    CHECK(literal.size() >= 1 && literal[0] == '/')
        << "The result of the pattern \"" << pattern.AsString()
        << "\" was not an absolute path.";
    output->push_back(SourceFile(literal));
  }
}

// static
void SubstitutionWriter::GetListAsOutputFiles(const Settings* settings,
                                              const SubstitutionList& list,
                                              std::vector<OutputFile>* output) {
  std::vector<SourceFile> output_as_sources;
  GetListAsSourceFiles(list, &output_as_sources);
  for (const auto& file : output_as_sources)
    output->push_back(OutputFile(settings->build_settings(), file));
}

// static
SourceFile SubstitutionWriter::ApplyPatternToSource(
    const Target* target,
    const Settings* settings,
    const SubstitutionPattern& pattern,
    const SourceFile& source) {
  std::string result_value =
      ApplyPatternToSourceAsString(target, settings, pattern, source);
  CHECK(!result_value.empty() && result_value[0] == '/')
      << "The result of the pattern \"" << pattern.AsString()
      << "\" was not a path beginning in \"/\" or \"//\".";
  return SourceFile(std::move(result_value));
}

// static
std::string SubstitutionWriter::ApplyPatternToSourceAsString(
    const Target* target,
    const Settings* settings,
    const SubstitutionPattern& pattern,
    const SourceFile& source) {
  std::string result_value;
  for (const auto& subrange : pattern.ranges()) {
    if (subrange.type == &SubstitutionLiteral) {
      result_value.append(subrange.literal);
    } else {
      result_value.append(GetSourceSubstitution(target, settings, source,
                                                subrange.type, OUTPUT_ABSOLUTE,
                                                SourceDir()));
    }
  }
  return result_value;
}

// static
OutputFile SubstitutionWriter::ApplyPatternToSourceAsOutputFile(
    const Target* target,
    const Settings* settings,
    const SubstitutionPattern& pattern,
    const SourceFile& source) {
  SourceFile result_as_source =
      ApplyPatternToSource(target, settings, pattern, source);
  return OutputFile(settings->build_settings(), result_as_source);
}

// static
void SubstitutionWriter::ApplyListToSource(const Target* target,
                                           const Settings* settings,
                                           const SubstitutionList& list,
                                           const SourceFile& source,
                                           std::vector<SourceFile>* output) {
  for (const auto& item : list.list())
    output->push_back(ApplyPatternToSource(target, settings, item, source));
}

// static
void SubstitutionWriter::ApplyListToSourceAsString(
    const Target* target,
    const Settings* settings,
    const SubstitutionList& list,
    const SourceFile& source,
    std::vector<std::string>* output) {
  for (const auto& item : list.list())
    output->push_back(
        ApplyPatternToSourceAsString(target, settings, item, source));
}

// static
void SubstitutionWriter::ApplyListToSourceAsOutputFile(
    const Target* target,
    const Settings* settings,
    const SubstitutionList& list,
    const SourceFile& source,
    std::vector<OutputFile>* output) {
  for (const auto& item : list.list())
    output->push_back(
        ApplyPatternToSourceAsOutputFile(target, settings, item, source));
}

// static
void SubstitutionWriter::ApplyListToSources(
    const Target* target,
    const Settings* settings,
    const SubstitutionList& list,
    const std::vector<SourceFile>& sources,
    std::vector<SourceFile>* output) {
  output->clear();
  for (const auto& source : sources)
    ApplyListToSource(target, settings, list, source, output);
}

// static
void SubstitutionWriter::ApplyListToSourcesAsString(
    const Target* target,
    const Settings* settings,
    const SubstitutionList& list,
    const std::vector<SourceFile>& sources,
    std::vector<std::string>* output) {
  output->clear();
  for (const auto& source : sources)
    ApplyListToSourceAsString(target, settings, list, source, output);
}

// static
void SubstitutionWriter::ApplyListToSourcesAsOutputFile(
    const Target* target,
    const Settings* settings,
    const SubstitutionList& list,
    const std::vector<SourceFile>& sources,
    std::vector<OutputFile>* output) {
  output->clear();
  for (const auto& source : sources)
    ApplyListToSourceAsOutputFile(target, settings, list, source, output);
}

// static
void SubstitutionWriter::WriteNinjaVariablesForSource(
    const Target* target,
    const Settings* settings,
    const SourceFile& source,
    const std::vector<const Substitution*>& types,
    const EscapeOptions& escape_options,
    std::ostream& out) {
  for (const auto& type : types) {
    // Don't write SOURCE since that just maps to Ninja's $in variable, which
    // is implicit in the rule. RESPONSE_FILE_NAME is written separately
    // only when writing target rules since it can never be used in any
    // other context (like process_file_template).
    if (type != &SubstitutionSource && type != &SubstitutionRspFileName) {
      out << "  " << type->ninja_name << " = ";
      EscapeStringToStream(
          out,
          GetSourceSubstitution(target, settings, source, type, OUTPUT_RELATIVE,
                                settings->build_settings()->build_dir()),
          escape_options);
      out << std::endl;
    }
  }
}

// static
std::string SubstitutionWriter::GetSourceSubstitution(
    const Target* target,
    const Settings* settings,
    const SourceFile& source,
    const Substitution* type,
    OutputStyle output_style,
    const SourceDir& relative_to) {
  std::string to_rebase;
  if (type == &SubstitutionSource) {
    if (source.is_system_absolute())
      return source.value();
    to_rebase = source.value();
  } else if (type == &SubstitutionSourceNamePart) {
    return std::string(FindFilenameNoExtension(&source.value()));
  } else if (type == &SubstitutionSourceFilePart) {
    return source.GetName();
  } else if (type == &SubstitutionSourceDir) {
    if (source.is_system_absolute())
      return DirectoryWithNoLastSlash(source.GetDir());
    to_rebase = DirectoryWithNoLastSlash(source.GetDir());
  } else if (type == &SubstitutionSourceRootRelativeDir) {
    if (source.is_system_absolute())
      return DirectoryWithNoLastSlash(source.GetDir());
    return RebasePath(DirectoryWithNoLastSlash(source.GetDir()),
                      SourceDir("//"),
                      settings->build_settings()->root_path_utf8());
  } else if (type == &SubstitutionSourceGenDir) {
    to_rebase = DirectoryWithNoLastSlash(GetSubBuildDirAsSourceDir(
        BuildDirContext(settings), source.GetDir(), BuildDirType::GEN));
  } else if (type == &SubstitutionSourceOutDir) {
    to_rebase = DirectoryWithNoLastSlash(GetSubBuildDirAsSourceDir(
        BuildDirContext(settings), source.GetDir(), BuildDirType::OBJ));
  } else if (type == &SubstitutionSourceTargetRelative) {
    if (target) {
      return RebasePath(source.value(), target->label().dir(),
                        settings->build_settings()->root_path_utf8());
    }
    NOTREACHED() << "Cannot use substitution " << type->name
                 << " without target";
    return std::string();
  } else if (IsValidRustSubstitution(type)) {
    to_rebase = source.value();
  } else {
    NOTREACHED() << "Unsupported substitution for this function: "
                 << type->name;
    return std::string();
  }

  // If we get here, the result is a path that should be made relative or
  // absolute according to the output_style. Other cases (just file name or
  // extension extraction) will have been handled via early return above.
  if (output_style == OUTPUT_ABSOLUTE)
    return to_rebase;
  return RebasePath(to_rebase, relative_to,
                    settings->build_settings()->root_path_utf8());
}

// static
OutputFile SubstitutionWriter::ApplyPatternToTargetAsOutputFile(
    const Target* target,
    const Tool* tool,
    const SubstitutionPattern& pattern) {
  std::string result_value;
  for (const auto& subrange : pattern.ranges()) {
    if (subrange.type == &SubstitutionLiteral) {
      result_value.append(subrange.literal);
    } else {
      std::string subst;
      CHECK(GetTargetSubstitution(target, subrange.type, &subst));
      result_value.append(subst);
    }
  }
  return OutputFile(result_value);
}

// static
void SubstitutionWriter::ApplyListToTargetAsOutputFile(
    const Target* target,
    const Tool* tool,
    const SubstitutionList& list,
    std::vector<OutputFile>* output) {
  for (const auto& item : list.list())
    output->push_back(ApplyPatternToTargetAsOutputFile(target, tool, item));
}

// static
bool SubstitutionWriter::GetTargetSubstitution(const Target* target,
                                               const Substitution* type,
                                               std::string* result) {
  if (type == &SubstitutionLabel) {
    // Only include the toolchain for non-default toolchains.
    *result =
        target->label().GetUserVisibleName(!target->settings()->is_default());
  } else if (type == &SubstitutionLabelName) {
    *result = target->label().name();
  } else if (type == &SubstitutionLabelNoToolchain) {
    *result = target->label().GetUserVisibleName(false);
  } else if (type == &SubstitutionRootGenDir) {
    SetDirOrDotWithNoSlash(
        GetBuildDirAsOutputFile(BuildDirContext(target), BuildDirType::GEN)
            .value(),
        result);
  } else if (type == &SubstitutionRootOutDir) {
    SetDirOrDotWithNoSlash(
        target->settings()->toolchain_output_subdir().value(), result);
  } else if (type == &SubstitutionTargetGenDir) {
    SetDirOrDotWithNoSlash(
        GetBuildDirForTargetAsOutputFile(target, BuildDirType::GEN).value(),
        result);
  } else if (type == &SubstitutionTargetOutDir) {
    SetDirOrDotWithNoSlash(
        GetBuildDirForTargetAsOutputFile(target, BuildDirType::OBJ).value(),
        result);
  } else if (type == &SubstitutionTargetOutputName) {
    *result = target->GetComputedOutputName();
  } else {
    return false;
  }
  return true;
}

// static
std::string SubstitutionWriter::GetTargetSubstitution(
    const Target* target,
    const Substitution* type) {
  std::string result;
  GetTargetSubstitution(target, type, &result);
  return result;
}

// static
OutputFile SubstitutionWriter::ApplyPatternToCompilerAsOutputFile(
    const Target* target,
    const SourceFile& source,
    const SubstitutionPattern& pattern) {
  OutputFile result;
  for (const auto& subrange : pattern.ranges()) {
    if (subrange.type == &SubstitutionLiteral) {
      result.value().append(subrange.literal);
    } else {
      result.value().append(
          GetCompilerSubstitution(target, source, subrange.type));
    }
  }
  return result;
}

// static
void SubstitutionWriter::ApplyListToCompilerAsOutputFile(
    const Target* target,
    const SourceFile& source,
    const SubstitutionList& list,
    std::vector<OutputFile>* output) {
  for (const auto& item : list.list())
    output->push_back(ApplyPatternToCompilerAsOutputFile(target, source, item));
}

// static
std::string SubstitutionWriter::GetCompilerSubstitution(
    const Target* target,
    const SourceFile& source,
    const Substitution* type) {
  // First try the common tool ones.
  std::string result;
  if (GetTargetSubstitution(target, type, &result))
    return result;

  // Fall-through to the source ones.
  return GetSourceSubstitution(
      target, target->settings(), source, type, OUTPUT_RELATIVE,
      target->settings()->build_settings()->build_dir());
}

// static
OutputFile SubstitutionWriter::ApplyPatternToLinkerAsOutputFile(
    const Target* target,
    const Tool* tool,
    const SubstitutionPattern& pattern) {
  OutputFile result;
  for (const auto& subrange : pattern.ranges()) {
    if (subrange.type == &SubstitutionLiteral) {
      result.value().append(subrange.literal);
    } else {
      result.value().append(GetLinkerSubstitution(target, tool, subrange.type));
    }
  }
  return result;
}

// static
void SubstitutionWriter::ApplyListToLinkerAsOutputFile(
    const Target* target,
    const Tool* tool,
    const SubstitutionList& list,
    std::vector<OutputFile>* output) {
  for (const auto& item : list.list())
    output->push_back(ApplyPatternToLinkerAsOutputFile(target, tool, item));
}

// static
std::string SubstitutionWriter::GetLinkerSubstitution(
    const Target* target,
    const Tool* tool,
    const Substitution* type) {
  // First try the common tool ones.
  std::string result;
  if (GetTargetSubstitution(target, type, &result))
    return result;

  // Fall-through to the linker-specific ones.
  if (type == &SubstitutionOutputDir) {
    // Use the target's value if there is one (it will have no expansion
    // patterns since it can directly use GN variables to compute whatever
    // path it wants), or the tool's default (which will contain further
    // expansions).
    if (target->output_dir().is_null()) {
      return ApplyPatternToLinkerAsOutputFile(target, tool,
                                              tool->default_output_dir())
          .value();
    }
    SetDirOrDotWithNoSlash(
        RebasePath(target->output_dir().value(),
                   target->settings()->build_settings()->build_dir()),
        &result);
    return result;
  } else if (type == &SubstitutionOutputExtension) {
    // Use the extension provided on the target if specified, otherwise
    // fall back on the default. Note that the target's output extension
    // does not include the dot but the tool's does.
    if (!target->output_extension_set())
      return tool->default_output_extension();
    if (target->output_extension().empty())
      return std::string();  // Explicitly set to no extension.
    return std::string(".") + target->output_extension();
  } else if (type == &kRustSubstitutionCrateName) {
    // Only include the toolchain for non-default toolchains.
    return target->rust_values().crate_name();
  } else if (type == &CSubstitutionSwiftModuleName) {
    return target->swift_values().module_name();
  } else {
    NOTREACHED();
    return std::string();
  }
}
