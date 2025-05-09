// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "settings.h"

#include "logging.h"
#include "filesystem_utils.h"
#include "build_config.h"

Settings::Settings(const BuildSettings* build_settings,
                   const std::string& output_subdir_name)
    : build_settings_(build_settings), base_config_(this) {
  if (output_subdir_name.empty()) {
    toolchain_output_dir_ = build_settings->build_dir();
  } else {
    // We guarantee this ends in a slash.
    DCHECK(output_subdir_name[output_subdir_name.size() - 1] == '/');
    toolchain_output_subdir_.value().append(output_subdir_name);

    DCHECK(!build_settings->build_dir().is_null());
    toolchain_output_dir_ = SourceDir(build_settings->build_dir().value() +
                                      toolchain_output_subdir_.value());
  }
  // The output dir will be null in some tests and when invoked to parsed
  // one-off data without doing generation.
  if (!toolchain_output_dir_.is_null())
    toolchain_gen_dir_ = SourceDir(toolchain_output_dir_.value() + "gen/");
}

bool Settings::ShouldShowToolchain(
    std::initializer_list<const Label*> labels) const {
  for (auto label : labels) {
    if (label->GetToolchainLabel() != default_toolchain_label_)
      return true;
  }
  return false;
}
