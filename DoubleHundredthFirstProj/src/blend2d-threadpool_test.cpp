// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_test_p.h"
#if defined(BL_TEST)

#include "blend2d-atomic_p.h"
#include "blend2d-conditionvariable_p.h"
#include "blend2d-mutex_p.h"
#include "blend2d-threadpool_p.h"

// bl::ThreadPool - Tests
// ======================

namespace bl {
namespace Tests {

struct ThreadTestData {
  uint32_t iter;
  uint32_t counter;
  uint32_t waiting;
  BLMutex mutex;
  BLConditionVariable condition;

  ThreadTestData() noexcept
    : iter(0),
      counter(0),
      waiting(false) {}
};

static void BL_CDECL test_thread_entry(BLThread* thread, void* data_) noexcept {
  ThreadTestData* data = static_cast<ThreadTestData*>(data_);
  uint32_t iter = blAtomicFetchStrong(&data->iter);

  INFO("[#%u] Thread %p running\n", iter, thread);

  if (blAtomicFetchSubStrong(&data->counter) == 1) {
    BLLockGuard<BLMutex> guard(data->mutex);
    if (data->waiting) {
      INFO("[#%u] Thread %p signaling to main thread\n", iter, thread);
      data->condition.signal();
    }
  }
}

UNIT(thread_pool, BL_TEST_GROUP_THREADING) {
  BLThreadPool* tp = blThreadPoolGlobal();
  ThreadTestData data;

  constexpr uint32_t kThreadCount = 4;
  BLThread* threads[kThreadCount];

  // Allocating more threads than an internal limit for must always fail.
  INFO("Trying to allocate very high number of threads that should fail");
  {
    BLResult reason;
    uint32_t n = tp->acquireThreads(nullptr, 1000000, BL_THREAD_POOL_ACQUIRE_FLAG_ALL_OR_NOTHING, &reason);

    EXPECT_EQ(n, 0u);
    EXPECT_EQ(reason, BL_ERROR_THREAD_POOL_EXHAUSTED);
  }

  INFO("Repeatedly acquiring / releasing %u threads with a simple task", kThreadCount);
  for (uint32_t i = 0; i < 10; i++) {
    blAtomicFetchAddStrong(&data.iter);

    INFO("[#%u] Acquiring %u threads from thread-pool", i, kThreadCount);
    BLResult reason;
    uint32_t n = tp->acquireThreads(threads, kThreadCount, 0, &reason);

    EXPECT_SUCCESS(reason);
    EXPECT_EQ(n, kThreadCount);

    blAtomicStoreRelaxed(&data.counter, n);
    INFO("[#%u] Running %u threads", i, n);
    for (BLThread* thread : threads) {
      BLResult result = thread->run(test_thread_entry, &data);
      EXPECT_SUCCESS(result);
    }

    INFO("[#%u] Waiting and releasing", i);
    {
      BLLockGuard<BLMutex> guard(data.mutex);
      data.waiting = true;
      while (blAtomicFetchStrong(&data.counter) != 0)
        data.condition.wait(data.mutex);
    }

    tp->releaseThreads(threads, n);
  }


  INFO("Cleaning up");
  tp->cleanup();

  INFO("Done");
}

} // {Tests}
} // {bl}

#endif // BL_TEST
