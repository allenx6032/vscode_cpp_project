// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_p.h"
#include "blend2d-intops_p.h"
#include "blend2d-scopedallocator_p.h"

namespace bl {

// bl::ScopedAllocator - Alloc
// ===========================

void* ScopedAllocator::alloc(size_t size, size_t alignment) noexcept {
  // First try to allocate from the local memory pool.
  uint8_t* p = IntOps::alignUp(poolPtr, alignment);
  size_t remain = size_t(IntOps::usubSaturate((uintptr_t)poolEnd, (uintptr_t)p));

  if (remain >= size) {
    poolPtr = p + size;
    return p;
  }

  // Bail to malloc if local pool was either not provided or didn't have the required capacity.
  size_t sizeWithOverhead = size + sizeof(Link) + (alignment - 1);
  p = static_cast<uint8_t*>(malloc(sizeWithOverhead));

  if (p == nullptr)
    return nullptr;

  reinterpret_cast<Link*>(p)->next = links;
  links = reinterpret_cast<Link*>(p);

  return IntOps::alignUp(p + sizeof(Link), alignment);
}

// ScopedAllocator - Reset
// =======================

void ScopedAllocator::reset() noexcept {
  Link* link = links;
  while (link) {
    Link* next = link->next;
    free(link);
    link = next;
  }

  links = nullptr;
  poolPtr = poolMem;
}

} // {bl}
