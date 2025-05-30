// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "substitution_pattern.h"

#include <stddef.h>

#include "string_number_conversions.h"
#include "build_settings.h"
#include "err.h"
#include "filesystem_utils.h"
#include "value.h"

SubstitutionPattern::Subrange::Subrange() : type(&SubstitutionLiteral) {}

SubstitutionPattern::Subrange::Subrange(const Substitution* t,
                                        const std::string& l)
    : type(t), literal(l) {}

SubstitutionPattern::Subrange::~Subrange() = default;

SubstitutionPattern::SubstitutionPattern() : origin_(nullptr) {}

SubstitutionPattern::SubstitutionPattern(const SubstitutionPattern& other) =
    default;

SubstitutionPattern::~SubstitutionPattern() = default;

bool SubstitutionPattern::Parse(const Value& value, Err* err) {
  if (!value.VerifyTypeIs(Value::STRING, err))
    return false;
  return Parse(value.string_value(), value.origin(), err);
}

bool SubstitutionPattern::Parse(const std::string& str,
                                const ParseNode* origin,
                                Err* err) {
  DCHECK(ranges_.empty());  // Should only be called once.

  size_t cur = 0;
  while (true) {
    size_t next = str.find("{{", cur);

    // Pick up everything from the previous spot to here as a literal.
    if (next == std::string::npos) {
      if (cur != str.size())
        ranges_.push_back(Subrange(&SubstitutionLiteral, str.substr(cur)));
      break;
    } else if (next > cur) {
      ranges_.push_back(
          Subrange(&SubstitutionLiteral, str.substr(cur, next - cur)));
    }

    // Find which specific pattern this corresponds to.
    bool found_match = false;
    for (const SubstitutionTypes* types : AllSubstitutions) {
      for (const Substitution* sub : *types) {
        const char* cur_pattern = sub->name;
        size_t cur_len = strlen(cur_pattern);
        if (str.compare(next, cur_len, cur_pattern) == 0) {
          ranges_.push_back(Subrange(sub));
          cur = next + cur_len;
          found_match = true;
          break;
        }
      }
    }

    // Expect all occurrences of {{ to resolve to a pattern.
    if (!found_match) {
      // Could make this error message more friendly if it comes up a lot. But
      // most people will not be writing substitution patterns and the code
      // to exactly indicate the error location is tricky.
      *err = Err(origin, "Unknown substitution pattern",
                 "Found a {{ at offset " + base::NumberToString(next) +
                     " and did not find a known substitution following it.");
      ranges_.clear();
      return false;
    }
  }

  origin_ = origin;

  // Fill required types vector.
  SubstitutionBits bits;
  FillRequiredTypes(&bits);
  bits.FillVector(&required_types_);
  return true;
}

// static
SubstitutionPattern SubstitutionPattern::MakeForTest(const char* str) {
  Err err;
  SubstitutionPattern pattern;
  CHECK(pattern.Parse(str, nullptr, &err)) << err.message() << "\n"
                                           << err.help_text();
  return pattern;
}

std::string SubstitutionPattern::AsString() const {
  std::string result;
  for (const auto& elem : ranges_) {
    if (elem.type == &SubstitutionLiteral)
      result.append(elem.literal);
    else
      result.append(elem.type->name);
  }
  return result;
}

void SubstitutionPattern::FillRequiredTypes(SubstitutionBits* bits) const {
  for (const auto& elem : ranges_) {
    if (elem.type != &SubstitutionLiteral)
      bits->used.insert(elem.type);
  }
}

bool SubstitutionPattern::IsInOutputDir(const BuildSettings* build_settings,
                                        Err* err) const {
  if (ranges_.empty()) {
    *err = Err(origin_, "This is empty but I was expecting an output file.");
    return false;
  }

  if (ranges_[0].type == &SubstitutionLiteral) {
    // If the first thing is a literal, it must start with the output dir.
    if (!EnsureStringIsInOutputDir(build_settings->build_dir(),
                                   ranges_[0].literal, origin_, err))
      return false;
  } else {
    // Otherwise, the first subrange must be a pattern that expands to
    // something in the output directory.
    if (!SubstitutionIsInOutputDir(ranges_[0].type)) {
      *err =
          Err(origin_, "File is not inside output directory.",
              "The given file should be in the output directory. Normally you\n"
              "would specify\n\"$target_out_dir/foo\" or "
              "\"{{source_gen_dir}}/foo\".");
      return false;
    }
  }

  return true;
}
