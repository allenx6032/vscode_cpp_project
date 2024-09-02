// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ninja_group_target_writer.h"

#include "string_util.h"
#include "deps_iterator.h"
#include "output_file.h"
#include "string_utils.h"
#include "target.h"

NinjaGroupTargetWriter::NinjaGroupTargetWriter(const Target* target,
                                               std::ostream& out)
    : NinjaTargetWriter(target, out) {}

NinjaGroupTargetWriter::~NinjaGroupTargetWriter() = default;

void NinjaGroupTargetWriter::Run() {
  // A group rule just generates a stamp file with dependencies on each of
  // the deps and data_deps in the group.
  std::vector<OutputFile> output_files;
  std::vector<OutputFile> data_output_files;
  const auto& target_deps = resolved().GetTargetDeps(target_);

  for (const Target* dep : target_deps.linked_deps()) {
    if (dep->IsDataOnly()) {
      data_output_files.push_back(dep->dependency_output_file());
    } else {
      output_files.push_back(dep->dependency_output_file());
    }
  }

  for (const Target* data_dep : target_deps.data_deps())
    data_output_files.push_back(data_dep->dependency_output_file());

  WriteStampForTarget(output_files, data_output_files);
}
