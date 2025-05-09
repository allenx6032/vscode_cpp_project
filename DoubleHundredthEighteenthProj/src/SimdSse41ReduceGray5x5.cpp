/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2022 Yermalayeu Ihar.
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
#include "SimdMemory.h"
#include "SimdStore.h"

namespace Simd
{
#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {
        namespace
        {
            struct Buffer
            {
                Buffer(size_t width)
                {
                    _p = Allocate(sizeof(uint16_t)*(5 * width + A));
                    in0 = (uint16_t*)_p;
                    in1 = in0 + width;
                    out0 = in1 + width;
                    out1 = out0 + width;
                    dst = out1 + width + HA;
                }

                ~Buffer()
                {
                    Free(_p);
                }

                uint16_t * in0;
                uint16_t * in1;
                uint16_t * out0;
                uint16_t * out1;
                uint16_t * dst;
            private:
                void *_p;
            };
        }

        template <bool compensation> SIMD_INLINE __m128i DivideBy256(__m128i value);

        template <> SIMD_INLINE __m128i DivideBy256<true>(__m128i value)
        {
            return _mm_srli_epi16(_mm_add_epi16(value, K16_0080), 8);
        }

        template <> SIMD_INLINE __m128i DivideBy256<false>(__m128i value)
        {
            return _mm_srli_epi16(value, 8);
        }

        SIMD_INLINE __m128i LoadUnpacked(const void * src)
        {
            return _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)src), K_ZERO);
        }

        template<bool align> SIMD_INLINE void FirstRow5x5(__m128i src, Buffer & buffer, size_t offset)
        {
            Store<align>((__m128i*)(buffer.in0 + offset), src);
            Store<align>((__m128i*)(buffer.in1 + offset), _mm_mullo_epi16(src, K16_0005));
        }

        template<bool align> SIMD_INLINE void FirstRow5x5(const uint8_t * src, Buffer & buffer, size_t offset)
        {
            FirstRow5x5<align>(LoadUnpacked(src + offset), buffer, offset);
            offset += HA;
            FirstRow5x5<align>(LoadUnpacked(src + offset), buffer, offset);
        }

        template<bool align> SIMD_INLINE void MainRowY5x5(__m128i odd, __m128i even, Buffer & buffer, size_t offset)
        {
            __m128i cp = _mm_mullo_epi16(odd, K16_0004);
            __m128i c0 = Load<align>((__m128i*)(buffer.in0 + offset));
            __m128i c1 = Load<align>((__m128i*)(buffer.in1 + offset));
            Store<align>((__m128i*)(buffer.dst + offset), _mm_add_epi16(even, _mm_add_epi16(c1, _mm_add_epi16(cp, _mm_mullo_epi16(c0, K16_0006)))));
            Store<align>((__m128i*)(buffer.out1 + offset), _mm_add_epi16(c0, cp));
            Store<align>((__m128i*)(buffer.out0 + offset), even);
        }

        template<bool align> SIMD_INLINE void MainRowY5x5(const uint8_t *odd, const uint8_t *even, Buffer & buffer, size_t offset)
        {
            MainRowY5x5<align>(LoadUnpacked(odd + offset), LoadUnpacked(even + offset), buffer, offset);
            offset += HA;
            MainRowY5x5<align>(LoadUnpacked(odd + offset), LoadUnpacked(even + offset), buffer, offset);
        }

        template <bool align, bool compensation> SIMD_INLINE __m128i MainRowX5x5(uint16_t * dst)
        {
            __m128i t0 = _mm_loadu_si128((__m128i*)(dst - 2));
            __m128i t1 = _mm_loadu_si128((__m128i*)(dst - 1));
            __m128i t2 = Load<align>((__m128i*)dst);
            __m128i t3 = _mm_loadu_si128((__m128i*)(dst + 1));
            __m128i t4 = _mm_loadu_si128((__m128i*)(dst + 2));
            t2 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(t2, K16_0006), _mm_mullo_epi16(_mm_add_epi16(t1, t3), K16_0004)), _mm_add_epi16(t0, t4));
            return DivideBy256<compensation>(t2);
        }

        template <bool align, bool compensation> SIMD_INLINE void MainRowX5x5(Buffer & buffer, size_t offset, uint8_t *dst)
        {
            __m128i t0 = MainRowX5x5<align, compensation>(buffer.dst + offset);
            __m128i t1 = MainRowX5x5<align, compensation>(buffer.dst + offset + HA);
            t0 = _mm_packus_epi16(_mm_and_si128(_mm_packus_epi16(t0, t1), K16_00FF), K_ZERO);
            _mm_storel_epi64((__m128i*)dst, t0);
        }

        template <bool compensation> void ReduceGray5x5(
            const uint8_t* src, size_t srcWidth, size_t srcHeight, size_t srcStride,
            uint8_t* dst, size_t dstWidth, size_t dstHeight, size_t dstStride)
        {
            assert((srcWidth + 1) / 2 == dstWidth && (srcHeight + 1) / 2 == dstHeight && srcWidth >= A);

            size_t alignedWidth = Simd::AlignLo(srcWidth, A);
            size_t bufferDstTail = Simd::AlignHi(srcWidth - A, 2);

            Buffer buffer(Simd::AlignHi(srcWidth, A));

            for (size_t col = 0; col < alignedWidth; col += A)
                FirstRow5x5<true>(src, buffer, col);
            if (alignedWidth != srcWidth)
                FirstRow5x5<false>(src, buffer, srcWidth - A);
            src += srcStride;

            for (size_t row = 1; row <= srcHeight; row += 2, dst += dstStride, src += 2 * srcStride)
            {
                const uint8_t *odd = src - (row < srcHeight ? 0 : srcStride);
                const uint8_t *even = odd + (row < srcHeight - 1 ? srcStride : 0);

                for (size_t col = 0; col < alignedWidth; col += A)
                    MainRowY5x5<true>(odd, even, buffer, col);
                if (alignedWidth != srcWidth)
                    MainRowY5x5<false>(odd, even, buffer, srcWidth - A);

                Swap(buffer.in0, buffer.out0);
                Swap(buffer.in1, buffer.out1);

                buffer.dst[-2] = buffer.dst[0];
                buffer.dst[-1] = buffer.dst[0];
                buffer.dst[srcWidth] = buffer.dst[srcWidth - 1];
                buffer.dst[srcWidth + 1] = buffer.dst[srcWidth - 1];

                for (size_t srcCol = 0, dstCol = 0; srcCol < alignedWidth; srcCol += A, dstCol += HA)
                    MainRowX5x5<true, compensation>(buffer, srcCol, dst + dstCol);
                if (alignedWidth != srcWidth)
                    MainRowX5x5<false, compensation>(buffer, bufferDstTail, dst + dstWidth - HA);
            }
        }

        void ReduceGray5x5(const uint8_t *src, size_t srcWidth, size_t srcHeight, size_t srcStride,
            uint8_t *dst, size_t dstWidth, size_t dstHeight, size_t dstStride, int compensation)
        {
            if (compensation)
                ReduceGray5x5<true>(src, srcWidth, srcHeight, srcStride, dst, dstWidth, dstHeight, dstStride);
            else
                ReduceGray5x5<false>(src, srcWidth, srcHeight, srcStride, dst, dstWidth, dstHeight, dstStride);
        }
    }
#endif
}
