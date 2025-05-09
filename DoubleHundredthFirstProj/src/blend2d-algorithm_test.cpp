// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_test_p.h"
#if defined(BL_TEST)

#include "blend2d-algorithm_p.h"
#include "blend2d-math_p.h"

namespace bl {
namespace Tests {

template<typename T>
static void checkArrays(const T* a, const T* b, size_t size) noexcept {
  for (size_t i = 0; i < size; i++) {
    EXPECT_EQ(a[i], b[i]).message("Mismatch at %u", unsigned(i));
  }
}

UNIT(support_algorithm, BL_TEST_GROUP_SUPPORT_UTILITIES) {
  INFO("bl::lowerBound()");
  {
    static const int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12 };

    EXPECT_EQ(lowerBound(arr, 0, 0), 0u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), -11000), 0u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 0), 0u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 1), 1u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 2), 2u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 3), 3u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 4), 4u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 5), 5u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 6), 6u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 10), 10u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 11), 11u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 12), 11u);
    EXPECT_EQ(lowerBound(arr, BL_ARRAY_SIZE(arr), 11000), BL_ARRAY_SIZE(arr));
  }

  INFO("bl::quickSort() - Testing quickSort and insertionSort of predefined arrays");
  {
    constexpr size_t kArraySize = 11;

    int ref_[kArraySize] = { -4, -2, -1, 0, 1, 9, 12, 13, 14, 19, 22 };
    int arr1[kArraySize] = { 0, 1, -1, 19, 22, 14, -4, 9, 12, 13, -2 };
    int arr2[kArraySize];

    memcpy(arr2, arr1, kArraySize * sizeof(int));

    insertionSort(arr1, kArraySize);
    quickSort(arr2, kArraySize);
    checkArrays(arr1, ref_, kArraySize);
    checkArrays(arr2, ref_, kArraySize);
  }

  INFO("bl::quickSort() - Testing quickSort and insertionSort of artificial arrays");
  {
    constexpr size_t kArraySize = 200;

    int arr1[kArraySize];
    int arr2[kArraySize];
    int ref_[kArraySize];

    for (size_t size = 2; size < kArraySize; size++) {
      for (size_t i = 0; i < size; i++) {
        arr1[i] = int(size - 1 - i);
        arr2[i] = int(size - 1 - i);
        ref_[i] = int(i);
      }

      insertionSort(arr1, size);
      quickSort(arr2, size);
      checkArrays(arr1, ref_, size);
      checkArrays(arr2, ref_, size);
    }
  }

  INFO("bl::quickSort() - Testing quickSort and insertingSort with unstable compare function");
  {
    constexpr size_t kArraySize = 5;

    float arr1[kArraySize] = { 1.0f, 0.0f, 3.0f, -1.0f, Math::nan<float>() };
    float arr2[kArraySize] = { };

    memcpy(arr2, arr1, kArraySize * sizeof(float));

    // We don't test as it's undefined where the NaN would be.
    insertionSort(arr1, kArraySize);
    quickSort(arr2, kArraySize);
  }

  INFO("bl::binarySearch() - Testing binary search");
  {
    static const int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    for (size_t size = BL_ARRAY_SIZE(arr); size > 0; size--) {
      for (size_t i = 0; i < size; i++) {
        int value = arr[i];
        EXPECT_EQ(binarySearch(arr, size, value), i);
        EXPECT_EQ(binarySearchClosestFirst(arr, size, value), i);
        EXPECT_EQ(binarySearchClosestLast(arr, size, value), i);
      }
    }
  }
}

} // {Tests}
} // {bl}

#endif // BL_TEST
