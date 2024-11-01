// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FILES_PLATFORM_FILE_H_
#define BASE_FILES_PLATFORM_FILE_H_

#include "scoped_file.h"
#include "build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#include "scoped_handle.h"
#endif

// This file defines platform-independent types for dealing with
// platform-dependent files. If possible, use the higher-level base::File class
// rather than these primitives.

namespace base {

#if defined(OS_WIN)

using PlatformFile = HANDLE;
using ScopedPlatformFile = ::base::win::ScopedHandle;

// It would be nice to make this constexpr but INVALID_HANDLE_VALUE is a
// ((void*)(-1)) which Clang rejects since reinterpret_cast is technically
// disallowed in constexpr. Visual Studio accepts this, however.
const PlatformFile kInvalidPlatformFile = INVALID_HANDLE_VALUE;

#elif defined(OS_POSIX) || defined(OS_FUCHSIA)

using PlatformFile = int;
using ScopedPlatformFile = ::base::ScopedFD;

constexpr PlatformFile kInvalidPlatformFile = -1;

#endif

}  // namespace base

#endif  // BASE_FILES_PLATFORM_FILE_H_
