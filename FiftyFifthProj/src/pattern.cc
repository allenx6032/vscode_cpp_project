// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "pattern.h"

#include "value.h"

const char kFilePattern_Help[] =
    R"*(File patterns

  File patterns are VERY limited regular expressions. They must match the
  entire input string to be counted as a match. In regular expression parlance,
  there is an implicit "^...$" surrounding your input. If you want to match a
  substring, you need to use wildcards at the beginning and end.

  There are only two special tokens understood by the pattern matcher.
  Everything else is a literal.

   - "*" Matches zero or more of any character. It does not depend on the
     preceding character (in regular expression parlance it is equivalent to
     ".*").

   - "\b" Matches a path boundary. This will match the beginning or end of a
     string, or a slash.

Pattern examples

  "*asdf*"
      Matches a string containing "asdf" anywhere.

  "asdf"
      Matches only the exact string "asdf".

  "*.cc"
      Matches strings ending in the literal ".cc".

  "\bwin/*"
      Matches "win/foo" and "foo/win/bar.cc" but not "iwin/foo".

)*";

namespace {

void ParsePattern(const std::string& s, std::vector<Pattern::Subrange>* out) {
  // Set when the last subrange is a literal so we can just append when we
  // find another literal.
  Pattern::Subrange* last_literal = nullptr;

  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '*') {
      // Don't allow two **.
      if (out->size() == 0 ||
          (*out)[out->size() - 1].type != Pattern::Subrange::ANYTHING)
        out->push_back(Pattern::Subrange(Pattern::Subrange::ANYTHING));
      last_literal = nullptr;
    } else if (s[i] == '\\') {
      if (i < s.size() - 1 && s[i + 1] == 'b') {
        // "\b" means path boundary.
        i++;
        out->push_back(Pattern::Subrange(Pattern::Subrange::PATH_BOUNDARY));
        last_literal = nullptr;
      } else {
        // Backslash + anything else means that literal char.
        if (!last_literal) {
          out->push_back(Pattern::Subrange(Pattern::Subrange::LITERAL));
          last_literal = &(*out)[out->size() - 1];
        }
        if (i < s.size() - 1) {
          i++;
          last_literal->literal.push_back(s[i]);
        } else {
          // Single backslash at end, use literal backslash.
          last_literal->literal.push_back('\\');
        }
      }
    } else {
      if (!last_literal) {
        out->push_back(Pattern::Subrange(Pattern::Subrange::LITERAL));
        last_literal = &(*out)[out->size() - 1];
      }
      last_literal->literal.push_back(s[i]);
    }
  }
}

}  // namespace

Pattern::Pattern(const std::string& s) {
  ParsePattern(s, &subranges_);
  is_suffix_ =
      (subranges_.size() == 2 && subranges_[0].type == Subrange::ANYTHING &&
       subranges_[1].type == Subrange::LITERAL);
}

Pattern::Pattern(const Pattern& other) = default;

Pattern::~Pattern() = default;

bool Pattern::MatchesString(const std::string& s) const {
  // Empty pattern matches only empty string.
  if (subranges_.empty())
    return s.empty();

  if (is_suffix_) {
    const std::string& suffix = subranges_[1].literal;
    if (suffix.size() > s.size())
      return false;  // Too short.
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
  }

  return RecursiveMatch(s, 0, 0, true);
}

// We assume the number of ranges is small so recursive is always reasonable.
// Could be optimized to only be recursive for *.
bool Pattern::RecursiveMatch(const std::string& s,
                             size_t begin_char,
                             size_t subrange_index,
                             bool allow_implicit_path_boundary) const {
  if (subrange_index >= subranges_.size()) {
    // Hit the end of our subranges, the text should also be at the end for a
    // match.
    return begin_char == s.size();
  }

  const Subrange& sr = subranges_[subrange_index];
  switch (sr.type) {
    case Subrange::LITERAL: {
      if (s.size() - begin_char < sr.literal.size())
        return false;  // Not enough room.
      if (s.compare(begin_char, sr.literal.size(), sr.literal) != 0)
        return false;  // Literal doesn't match.

      // Recursively check the next one.
      return RecursiveMatch(s, begin_char + sr.literal.size(),
                            subrange_index + 1, true);
    }

    case Subrange::PATH_BOUNDARY: {
      // When we can accept an implicit path boundary, we have to check both
      // a match of the literal and the implicit one.
      if (allow_implicit_path_boundary &&
          (begin_char == 0 || begin_char == s.size())) {
        // At implicit path boundary, see if the rest of the pattern matches.
        if (RecursiveMatch(s, begin_char, subrange_index + 1, false))
          return true;
      }

      // Check for a literal "/".
      if (begin_char < s.size() && s[begin_char] == '/') {
        // At explicit boundary, see if the rest of the pattern matches.
        if (RecursiveMatch(s, begin_char + 1, subrange_index + 1, true))
          return true;
      }
      return false;
    }

    case Subrange::ANYTHING: {
      if (subrange_index == subranges_.size() - 1)
        return true;  // * at the end, consider it matching.

      size_t min_next_size = sr.MinSize();

      // We don't care about exactly what matched as long as there was a match,
      // so we can do this front-to-back. If we needed the match, we would
      // normally want "*" to be greedy so would work backwards.
      for (size_t i = begin_char; i < s.size() - min_next_size; i++) {
        // Note: this could probably be faster by detecting the type of the
        // next match in advance and checking for a match in this loop rather
        // than doing a full recursive call for each character.
        if (RecursiveMatch(s, i, subrange_index + 1, true))
          return true;
      }
      return false;
    }

    default:
      NOTREACHED();
  }

  return false;
}

PatternList::PatternList() = default;

PatternList::PatternList(const PatternList& other) = default;

PatternList::~PatternList() = default;

void PatternList::Append(const Pattern& pattern) {
  patterns_.push_back(pattern);
}

void PatternList::SetFromValue(const Value& v, Err* err) {
  patterns_.clear();

  if (v.type() != Value::LIST) {
    *err = Err(v.origin(), "This value must be a list.");
    return;
  }

  const std::vector<Value>& list = v.list_value();
  for (const auto& elem : list) {
    if (!elem.VerifyTypeIs(Value::STRING, err))
      return;
    patterns_.push_back(Pattern(elem.string_value()));
  }
}

bool PatternList::MatchesString(const std::string& s) const {
  for (const auto& pattern : patterns_) {
    if (pattern.MatchesString(s))
      return true;
  }
  return false;
}

bool PatternList::MatchesValue(const Value& v) const {
  if (v.type() == Value::STRING)
    return MatchesString(v.string_value());
  return false;
}
