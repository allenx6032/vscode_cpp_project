// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "json_value_converter.h"

namespace base {
namespace internal {

bool BasicValueConverter<int>::Convert(const base::Value& value,
                                       int* field) const {
  return value.GetAsInteger(field);
}

bool BasicValueConverter<std::string>::Convert(const base::Value& value,
                                               std::string* field) const {
  return value.GetAsString(field);
}

bool BasicValueConverter<std::u16string>::Convert(const base::Value& value,
                                                  std::u16string* field) const {
  return value.GetAsString(field);
}

// bool BasicValueConverter<double>::Convert(const base::Value& value,
//                                           double* field) const {
//   return value.GetAsFloat(field);
// }

bool BasicValueConverter<bool>::Convert(const base::Value& value,
                                        bool* field) const {
  return value.GetAsBoolean(field);
}

}  // namespace internal
}  // namespace base
