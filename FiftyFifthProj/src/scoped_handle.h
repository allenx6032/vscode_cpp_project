// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_SCOPED_HANDLE_H_
#define BASE_WIN_SCOPED_HANDLE_H_

#include <windows.h>

#include "gtest_prod_util.h"
#include "logging.h"

// TODO(rvargas): remove this with the rest of the verifier.
#if defined(COMPILER_MSVC)
#include <intrin.h>
#define BASE_WIN_GET_CALLER _ReturnAddress()
#elif defined(COMPILER_GCC)
#define BASE_WIN_GET_CALLER \
  __builtin_extract_return_addr(\ __builtin_return_address(0))
#endif

namespace base {
namespace win {

// Generic wrapper for raw handles that takes care of closing handles
// automatically. The class interface follows the style of
// the ScopedFILE class with two additions:
//   - IsValid() method can tolerate multiple invalid handle values such as NULL
//     and INVALID_HANDLE_VALUE (-1) for Win32 handles.
//   - Set() (and the constructors and assignment operators that call it)
//     preserve the Windows LastError code. This ensures that GetLastError() can
//     be called after stashing a handle in a GenericScopedHandle object. Doing
//     this explicitly is necessary because of bug 528394 and VC++ 2015.
template <class Traits, class Verifier>
class GenericScopedHandle {
 public:
  typedef typename Traits::Handle Handle;

  GenericScopedHandle() : handle_(Traits::NullHandle()) {}

  explicit GenericScopedHandle(Handle handle) : handle_(Traits::NullHandle()) {
    Set(handle);
  }

  GenericScopedHandle(GenericScopedHandle&& other)
      : handle_(Traits::NullHandle()) {
    Set(other.Take());
  }

  ~GenericScopedHandle() { Close(); }

  bool IsValid() const { return Traits::IsHandleValid(handle_); }

  GenericScopedHandle& operator=(GenericScopedHandle&& other) {
    DCHECK_NE(this, &other);
    Set(other.Take());
    return *this;
  }

  void Set(Handle handle) {
    if (handle_ != handle) {
      // Preserve old LastError to avoid bug 528394.
      auto last_error = ::GetLastError();
      Close();

      if (Traits::IsHandleValid(handle)) {
        handle_ = handle;
      }
      ::SetLastError(last_error);
    }
  }

  Handle Get() const { return handle_; }

  // Transfers ownership away from this object.
  Handle Take() {
    Handle temp = handle_;
    handle_ = Traits::NullHandle();
    return temp;
  }

  // Explicitly closes the owned handle.
  void Close() {
    if (Traits::IsHandleValid(handle_)) {
      Traits::CloseHandle(handle_);
      handle_ = Traits::NullHandle();
    }
  }

 private:
  FRIEND_TEST_ALL_PREFIXES(ScopedHandleTest, ActiveVerifierWrongOwner);
  FRIEND_TEST_ALL_PREFIXES(ScopedHandleTest, ActiveVerifierUntrackedHandle);
  Handle handle_;

  GenericScopedHandle(const GenericScopedHandle&) = delete;
  GenericScopedHandle& operator=(const GenericScopedHandle&) = delete;
};

#undef BASE_WIN_GET_CALLER

// The traits class for Win32 handles that can be closed via CloseHandle() API.
class HandleTraits {
 public:
  typedef HANDLE Handle;

  // Closes the handle.
  static bool CloseHandle(HANDLE handle);

  // Returns true if the handle value is valid.
  static bool IsHandleValid(HANDLE handle) {
    return handle != NULL && handle != INVALID_HANDLE_VALUE;
  }

  // Returns NULL handle value.
  static HANDLE NullHandle() { return NULL; }

 private:
  HandleTraits() = delete;
  HandleTraits(const HandleTraits&) = delete;
  HandleTraits& operator=(const HandleTraits&) = delete;
};

// Do-nothing verifier.
class DummyVerifierTraits {
 public:
  typedef HANDLE Handle;

  static void StartTracking(HANDLE handle,
                            const void* owner,
                            const void* pc1,
                            const void* pc2) {}
  static void StopTracking(HANDLE handle,
                           const void* owner,
                           const void* pc1,
                           const void* pc2) {}

 private:
  DummyVerifierTraits() = delete;
  DummyVerifierTraits(const DummyVerifierTraits&) = delete;
  DummyVerifierTraits& operator=(const DummyVerifierTraits&) = delete;
};

// Performs actual run-time tracking.
class VerifierTraits {
 public:
  typedef HANDLE Handle;

  static void StartTracking(HANDLE handle,
                            const void* owner,
                            const void* pc1,
                            const void* pc2);
  static void StopTracking(HANDLE handle,
                           const void* owner,
                           const void* pc1,
                           const void* pc2);

 private:
  VerifierTraits() = delete;
  VerifierTraits(const VerifierTraits&) = delete;
  VerifierTraits& operator=(const VerifierTraits&) = delete;
};

typedef GenericScopedHandle<HandleTraits, VerifierTraits> ScopedHandle;

// This function may be called by the embedder to disable the use of
// VerifierTraits at runtime. It has no effect if DummyVerifierTraits is used
// for ScopedHandle.
void DisableHandleVerifier();

// This should be called whenever the OS is closing a handle, if extended
// verification of improper handle closing is desired. If |handle| is being
// tracked by the handle verifier and ScopedHandle is not the one closing it,
// a CHECK is generated.
void OnHandleBeingClosed(HANDLE handle);
}  // namespace win
}  // namespace base

#endif  // BASE_WIN_SCOPED_HANDLE_H_
