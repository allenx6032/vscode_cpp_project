﻿/*
 *  Copyright 2015 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv_rotate.h"
#include "libyuv_rotate_row.h"

#include "libyuv_basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

#define TANY(NAMEANY, TPOS_SIMD, MASK)                                         \
    void NAMEANY(const uint8* src, int src_stride,                             \
                 uint8* dst, int dst_stride, int width) {                      \
      int r = width & MASK;                                                    \
      int n = width - r;                                                       \
      if (n > 0) {                                                             \
        TPOS_SIMD(src, src_stride, dst, dst_stride, n);                        \
      }                                                                        \
      TransposeWx8_C(src + n, src_stride, dst + n * dst_stride, dst_stride, r);\
    }

#ifdef HAS_TRANSPOSEWX8_NEON
TANY(TransposeWx8_Any_NEON, TransposeWx8_NEON, 7)
#endif
#ifdef HAS_TRANSPOSEWX8_SSSE3
TANY(TransposeWx8_Any_SSSE3, TransposeWx8_SSSE3, 7)
#endif
#ifdef HAS_TRANSPOSEWX8_FAST_SSSE3
TANY(TransposeWx8_Fast_Any_SSSE3, TransposeWx8_Fast_SSSE3, 15)
#endif
#ifdef HAS_TRANSPOSEWX8_MIPS_DSPR2
TANY(TransposeWx8_Any_MIPS_DSPR2, TransposeWx8_MIPS_DSPR2, 7)
#endif
#undef TANY

#define TUVANY(NAMEANY, TPOS_SIMD, MASK)                                       \
    void NAMEANY(const uint8* src, int src_stride,                             \
                uint8* dst_a, int dst_stride_a,                                \
                uint8* dst_b, int dst_stride_b, int width) {                   \
      int r = width & MASK;                                                    \
      int n = width - r;                                                       \
      if (n > 0) {                                                             \
        TPOS_SIMD(src, src_stride, dst_a, dst_stride_a, dst_b, dst_stride_b,   \
                  n);                                                          \
      }                                                                        \
      TransposeUVWx8_C(src + n * 2, src_stride,                                \
                       dst_a + n * dst_stride_a, dst_stride_a,                 \
                       dst_b + n * dst_stride_b, dst_stride_b, r);             \
    }

#ifdef HAS_TRANSPOSEUVWX8_NEON
TUVANY(TransposeUVWx8_Any_NEON, TransposeUVWx8_NEON, 7)
#endif
#ifdef HAS_TRANSPOSEUVWX8_SSE2
TUVANY(TransposeUVWx8_Any_SSE2, TransposeUVWx8_SSE2, 7)
#endif
#ifdef HAS_TRANSPOSEUVWX8_MIPS_DSPR2
TUVANY(TransposeUVWx8_Any_MIPS_DSPR2, TransposeUVWx8_MIPS_DSPR2, 7)
#endif
#undef TUVANY

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif





