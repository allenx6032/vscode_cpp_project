// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef BLEND2D_THREADING_ATOMIC_P_H_INCLUDED
#define BLEND2D_THREADING_ATOMIC_P_H_INCLUDED

#include "blend2d-api-internal_p.h"

//! \cond INTERNAL
//! \addtogroup blend2d_internal
//! \{

//! \name Threading - Atomic Operations
//! \{

static BL_INLINE void blAtomicThreadFence(std::memory_order order = std::memory_order_seq_cst) noexcept {
  std::atomic_thread_fence(order);
}

template<typename T>
static BL_INLINE T blAtomicFetchRelaxed(const T* p) noexcept {
  using RawT = BLInternal::UIntByType<T>;
  return T(((const std::atomic<RawT>*)p)->load(std::memory_order_relaxed));
}

template<typename T>
static BL_INLINE T blAtomicFetchStrong(const T* p) noexcept {
  using RawT = BLInternal::UIntByType<T>;
  return T(((const std::atomic<RawT>*)p)->load(std::memory_order_acquire));
}

template<typename T, typename V>
static BL_INLINE void blAtomicStoreRelaxed(T* p, V value) noexcept {
  using RawT = BLInternal::UIntByType<T>;
  return ((std::atomic<RawT>*)p)->store(RawT(T(value)), std::memory_order_relaxed);
}

template<typename T, typename V>
static BL_INLINE void blAtomicStoreStrong(T* p, V value) noexcept {
  using RawT = BLInternal::UIntByType<T>;
  return ((std::atomic<RawT>*)p)->store(RawT(T(value)), std::memory_order_release);
}

template<typename T>
static BL_INLINE T blAtomicFetchOrRelaxed(T* x, T value) noexcept {
  return ((std::atomic<T>*)x)->fetch_or(value, std::memory_order_relaxed);
}

template<typename T>
static BL_INLINE T blAtomicFetchOrStrong(T* x, T value) noexcept {
  return ((std::atomic<T>*)x)->fetch_or(value, std::memory_order_acq_rel);
}

template<typename T>
static BL_INLINE T blAtomicFetchOrSeqCst(T* x, T value) noexcept {
  return ((std::atomic<T>*)x)->fetch_or(value, std::memory_order_seq_cst);
}

template<typename T>
static BL_INLINE T blAtomicFetchAndRelaxed(T* x, T value) noexcept {
  return ((std::atomic<T>*)x)->fetch_and(value, std::memory_order_relaxed);
}

template<typename T>
static BL_INLINE T blAtomicFetchAndStrong(T* x, T value) noexcept {
  return ((std::atomic<T>*)x)->fetch_and(value, std::memory_order_acq_rel);
}

template<typename T>
static BL_INLINE T blAtomicFetchAndSeqCst(T* x, T value) noexcept {
  return ((std::atomic<T>*)x)->fetch_and(value, std::memory_order_seq_cst);
}

template<typename T>
static BL_INLINE bool blAtomicCompareExchange(T* ptr, T* expected, T desired) noexcept {
  return std::atomic_compare_exchange_strong(((std::atomic<T>*)ptr), expected, desired);
}

//! \}

//! \}
//! \endcond

#endif // BLEND2D_THREADING_ATOMIC_P_H_INCLUDED
