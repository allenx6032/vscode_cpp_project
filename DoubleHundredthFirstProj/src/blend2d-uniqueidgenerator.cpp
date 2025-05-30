// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_p.h"
#include "blend2d-atomic_p.h"
#include "blend2d-mutex_p.h"
#include "blend2d-uniqueidgenerator_p.h"

namespace BLUniqueIdGenerator {

#if BL_TARGET_HAS_ATOMIC_64B

// UniqueIdGenerator - 64-Bit Atomics Implementation
// =================================================

struct alignas(BL_CACHE_LINE_SIZE) GlobalState {
  std::atomic<uint64_t> index;

  BL_INLINE uint64_t fetchAdd(uint32_t n) noexcept {
    return index.fetch_add(uint64_t(n));
  }
};

#else

// UniqueIdGenerator - 32-Bit Atomics Implementation
// =================================================

struct alignas(BL_CACHE_LINE_SIZE) GlobalState {
  std::atomic<uint32_t> _hi;
  std::atomic<uint32_t> _lo;

  BL_INLINE uint64_t fetchAdd(uint32_t n) noexcept {
    // This implementation doesn't always return an incrementing value as it's not the point. The requirement is to
    // never return the same value, so it sacrifices one bit in `_lo` counter that would tell us to increment `_hi`
    // counter and try again.
    const uint32_t kThresholdLo32 = 0x80000000u;

    for (;;) {
      uint32_t hiValue = _hi.load();
      uint32_t loValue = _lo.fetch_add(n);

      // This MUST support even cases when the thread executing this function right now is terminated. When we reach
      // the threshold we increment `_hi`, which would contain a new HIGH value that will be used immediately, then
      // we remove the threshold mark from LOW value and try to get a new LOW and HIGH values to return.
      if (BL_UNLIKELY(loValue & kThresholdLo32)) {
        _hi++;

        // If the thread is interrupted here we only incremented the HIGH value. In this case another thread that might
        // call `next()` would end up right here trying to clear `kThresholdLo32` from LOW value as well, which is fine.
        _lo.fetch_and(uint32_t(~kThresholdLo32));
        continue;
      }

      return (uint64_t(hiValue) << 32) | loValue;
    }
  }
};

#endif

// UniqueIdGenerator - Globals
// ===========================

static GlobalState globalState[uint32_t(Domain::kMaxValue) + 1];

#if !defined(BL_BUILD_NO_TLS)

// UniqueIdGenerator - API - TLS Support
// =====================================

// Unique ID generator with TLS support caches each `kLocalCacheCount` count of IDs by using a thread local storage
// and only requests next `kLocalCacheCount` IDs when the local cache was exhausted. This makes the common path
// (using TLS) order of magnitude faster than going through atomics.
static constexpr uint32_t kLocalCacheCount = 256;
static thread_local uint64_t tlsIdState[uint32_t(Domain::kMaxValue) + 1];

BLUniqueId generateId(Domain domain) noexcept {
  uint32_t domainIndex = uint32_t(domain);
  if ((tlsIdState[domainIndex] & (kLocalCacheCount - 1)) == 0)
    tlsIdState[domainIndex] = globalState[domainIndex].fetchAdd(kLocalCacheCount);

  return ++tlsIdState[domainIndex];
}

#else

// UniqueIdGenerator - API - No TLS Support
// ========================================

BLUniqueId generateId(Domain domain) noexcept {
  uint32_t domainIndex = uint32_t(domain);
  return globalState[domainIndex].fetchAdd(1) + 1;
}

#endif

} // {BLUniqueIdGenerator}
