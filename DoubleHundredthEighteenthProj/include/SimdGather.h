/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2024 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#ifndef __SimdGather_h__
#define __SimdGather_h__

#include "SimdInit.h"

namespace Simd
{
#ifdef SIMD_SSE41_ENABLE
    namespace Sse41
    {
        template<int step> SIMD_INLINE __m128 Gather(const float* ptr)
        {
            SIMD_ALIGNED(16) float buf[F];
            buf[0] = ptr[0 * step];
            buf[1] = ptr[1 * step];
            buf[2] = ptr[2 * step];
            buf[3] = ptr[3 * step];
            return _mm_load_ps(buf);
        }

        template<int step> SIMD_INLINE __m128 Gather(const float* ptr, size_t size)
        {
            SIMD_ALIGNED(16) float buf[F];
            for (size_t i = 0; i < size; ++i)
                buf[i] = ptr[i * step];
            return _mm_load_ps(buf);
        }
    }
#endif

#ifdef SIMD_AVX2_ENABLE
    namespace Avx2
    {
        template<int step> SIMD_INLINE __m256 Gather(const float* ptr)
        {
            static const __m256i idx = _mm256_setr_epi32(0 * step, 1 * step, 
                2 * step, 3 * step, 4 * step, 5 * step, 6 * step, 7 * step);
            return _mm256_i32gather_ps(ptr, idx, 4);
        }

        template<int step> SIMD_INLINE __m256 Gather(const float* ptr, size_t size)
        {
            SIMD_ALIGNED(32) float buf[F];
            for (size_t i = 0; i < size; ++i)
                buf[i] = ptr[i * step];
            return _mm256_load_ps(buf);
        }
    }
#endif

#ifdef SIMD_AVX512BW_ENABLE
    namespace Avx512bw
    {
        const __m512i K32_GATHER_ANY = SIMD_MM512_SET1_EPI32(1);
        const __m512i K32_GATHER_3A = SIMD_MM512_SETR_EPI32(0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 0, 0, 0, 0, 0);
        const __m512i K32_GATHER_3B = SIMD_MM512_SETR_EPI32(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 7, 10, 13);

        template<int period> SIMD_INLINE __m512 Gather(const float * ptr)
        {
            return _mm512_i32gather_ps(K32_GATHER_ANY, ptr, sizeof(float)*period);
        }

        template<> SIMD_INLINE __m512 Gather<3>(const float * ptr)
        {
            __m512 s0 = _mm512_loadu_ps(ptr + 0 * F);
            __m512 s1 = _mm512_loadu_ps(ptr + 1 * F);
            __m512 s2 = _mm512_loadu_ps(ptr + 2 * F);
            return _mm512_mask_permutexvar_ps(_mm512_maskz_permutex2var_ps(0xFFFF, s0, K32_GATHER_3A, s1), 0xF800, K32_GATHER_3B, s2);
        }
    }
#endif
}
#endif
