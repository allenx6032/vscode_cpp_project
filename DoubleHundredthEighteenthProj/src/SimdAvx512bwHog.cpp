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
#include "SimdStore.h"
#include "SimdBase.h"
#include "SimdArray.h"
#include "SimdUnpack.h"

namespace Simd
{
#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {
        namespace
        {
            struct Buffer
            {
                const int size;
                __m512 * cos, *sin;
                __m512i * pos, *neg;
                int * index;
                float * value;

                Buffer(size_t width, size_t quantization)
                    : size((int)quantization / 2)
                {
                    width = AlignHi(width, A / sizeof(float));
                    _p = Allocate(width*(sizeof(int) + sizeof(float)) + (sizeof(__m512i) + sizeof(__m512)) * 2 * size);
                    index = (int*)_p - 1;
                    value = (float*)index + width;
                    cos = (__m512*)(value + width + 1);
                    sin = cos + size;
                    pos = (__m512i*)(sin + size);
                    neg = pos + size;
                    for (int i = 0; i < size; ++i)
                    {
                        cos[i] = _mm512_set1_ps((float)::cos(i*M_PI / size));
                        sin[i] = _mm512_set1_ps((float)::sin(i*M_PI / size));
                        pos[i] = _mm512_set1_epi32(i);
                        neg[i] = _mm512_set1_epi32(size + i);
                    }
                }

                ~Buffer()
                {
                    Free(_p);
                }

            private:
                void *_p;
            };
        }

        template <bool align> SIMD_INLINE void HogDirectionHistograms(const __m512 & dx, const __m512 & dy, Buffer & buffer, size_t col)
        {
            __m512 bestDot = _mm512_setzero_ps();
            __m512i bestIndex = _mm512_setzero_si512();
            for (int i = 0; i < buffer.size; ++i)
            {
                __m512 dot = _mm512_fmadd_ps(dx, buffer.cos[i], _mm512_mul_ps(dy, buffer.sin[i]));
                bestIndex = _mm512_mask_blend_epi32(_mm512_cmp_ps_mask(dot, bestDot, _CMP_GT_OS), bestIndex, buffer.pos[i]);
                bestDot = _mm512_max_ps(dot, bestDot);

                dot = _mm512_sub_ps(_mm512_setzero_ps(), dot);
                bestIndex = _mm512_mask_blend_epi32(_mm512_cmp_ps_mask(dot, bestDot, _CMP_GT_OS), bestIndex, buffer.neg[i]);
                bestDot = _mm512_max_ps(dot, bestDot);
            }
            Store<align>(buffer.index + col, bestIndex);
            Store<align>(buffer.value + col, _mm512_sqrt_ps(_mm512_fmadd_ps(dx, dx, _mm512_mul_ps(dy, dy))));
        }

        template <int part> SIMD_INLINE __m512 CovertDifference(const __m256i & a, const __m256i & b)
        {
            return _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(Avx2::SubUnpackedU8<part>(a, b)));
        }

        template <bool align> SIMD_INLINE void HogDirectionHistograms(const uint8_t * src, size_t stride, Buffer & buffer, size_t col)
        {
            const uint8_t * s = src + col;
            __m256i t = Avx2::LoadPermuted<false>((__m256i*)(s - stride));
            __m256i l = Avx2::LoadPermuted<false>((__m256i*)(s - 1));
            __m256i r = Avx2::LoadPermuted<false>((__m256i*)(s + 1));
            __m256i b = Avx2::LoadPermuted<false>((__m256i*)(s + stride));
            HogDirectionHistograms<align>(CovertDifference<0>(r, l), CovertDifference<0>(b, t), buffer, col + 0);
            HogDirectionHistograms<align>(CovertDifference<1>(r, l), CovertDifference<1>(b, t), buffer, col + F);
        }

        namespace Custom_8x8_18
        {
            struct Buffer
            {
                __m512i pos[5];
                __m512 cos[5], sin[5];
                __m128 kx[8], ky[8];

                int * index;
                float * value;
                __m128 * hist;
                size_t hs;

                Buffer(size_t width)
                {
                    width = AlignHi(width, A / sizeof(float));
                    hs = (width / 8 + 1) * 18 * sizeof(__m128);
                    _p = Allocate(width*(sizeof(int) + sizeof(float)) + hs);
                    index = (int*)_p - 1;
                    value = (float*)index + width;
                    hist = (__m128*)(value + width + 1);

                    for (int i = 0; i < 5; ++i)
                    {
                        cos[i] = _mm512_set1_ps((float)::cos(i*M_PI / 9));
                        sin[i] = _mm512_set1_ps((float)::sin(i*M_PI / 9));
                        pos[i] = _mm512_set1_epi32(i);
                    }
                    for (int i = 0; i < 8; ++i)
                    {
                        float k0 = float((15 - i * 2) / 16.0f);
                        float k1 = 1.0f - k0;
                        kx[i] = _mm_setr_ps(k0, k1, k0, k1);
                        ky[i] = _mm_setr_ps(k0, k0, k1, k1);
                    }
                    ClearHist();
                }

                ~Buffer()
                {
                    Free(_p);
                }

                void ClearHist()
                {
                    memset(hist, 0, hs);
                }

            private:
                void *_p;
            };

            const __m512i K32_9 = SIMD_MM512_SET1_EPI32(9);
            const __m512i K32_18 = SIMD_MM512_SET1_EPI32(18);

            template <bool align> SIMD_INLINE void HogDirectionHistograms(const __m512 & dx, const __m512 & dy, Buffer & buffer, size_t col)
            {
                __m512 _0 = _mm512_set1_ps(-0.0f);
                __m512 adx = _mm512_andnot_ps(_0, dx);
                __m512 ady = _mm512_andnot_ps(_0, dy);
                __m512 bestDot = _mm512_fmadd_ps(adx, buffer.cos[0], _mm512_mul_ps(ady, buffer.sin[0]));
                __m512i bestIndex = buffer.pos[0];
                for (int i = 1; i < 5; ++i)
                {
                    __m512 dot = _mm512_fmadd_ps(adx, buffer.cos[i], _mm512_mul_ps(ady, buffer.sin[i]));
                    bestIndex = _mm512_mask_blend_epi32(_mm512_cmp_ps_mask(dot, bestDot, _CMP_GT_OS), bestIndex, buffer.pos[i]);
                    bestDot = _mm512_max_ps(dot, bestDot);
                }
                bestIndex = _mm512_mask_sub_epi32(bestIndex, _mm512_cmp_ps_mask(dx, _0, _CMP_LT_OS), K32_9, bestIndex);

                __m512i corr = _mm512_maskz_set1_epi32(_mm512_cmp_ps_mask(adx, _mm512_setzero_ps(), _CMP_EQ_OS), 1);
                bestIndex = _mm512_mask_sub_epi32(bestIndex, _mm512_cmp_ps_mask(dy, _0, _CMP_LT_OS), K32_18, _mm512_add_epi32(bestIndex, corr));

                bestIndex = _mm512_mask_set1_epi32(bestIndex, _mm512_cmpeq_epi32_mask(bestIndex, K32_18), 0);

                Store<align>(buffer.index + col, bestIndex);
                Store<align>(buffer.value + col, _mm512_sqrt_ps(_mm512_fmadd_ps(adx, adx, _mm512_mul_ps(ady, ady))));
            }

            template <bool align> SIMD_INLINE void HogDirectionHistograms(const uint8_t * src, size_t stride, Buffer & buffer, size_t col)
            {
                const uint8_t * s = src + col;
                __m256i t = Avx2::LoadPermuted<false>((__m256i*)(s - stride));
                __m256i l = Avx2::LoadPermuted<false>((__m256i*)(s - 1));
                __m256i r = Avx2::LoadPermuted<false>((__m256i*)(s + 1));
                __m256i b = Avx2::LoadPermuted<false>((__m256i*)(s + stride));
                HogDirectionHistograms<align>(CovertDifference<0>(r, l), CovertDifference<0>(b, t), buffer, col + 0);
                HogDirectionHistograms<align>(CovertDifference<1>(r, l), CovertDifference<1>(b, t), buffer, col + F);
            }

            void AddRowToBuffer(const uint8_t * src, size_t stride, Buffer & buffer, size_t row, size_t width, size_t aligned)
            {
                const uint8_t * s = src + stride * row;
                for (size_t col = 1; col < aligned; col += HA)
                    HogDirectionHistograms<true>(s, stride, buffer, col);
                HogDirectionHistograms<false>(s, stride, buffer, width - 1 - HA);

                __m128 ky = buffer.ky[(row + 4) & 7];
                __m128 * hist = buffer.hist;
                size_t cellEnd = width / 8;

                for (size_t col = 1; col < 4; ++col)
                {
                    int index = buffer.index[col];
                    __m128 value = _mm_set1_ps(buffer.value[col]);
                    __m128 kx = buffer.kx[(col + 4) & 7];
                    hist[index] = _mm_fmadd_ps(_mm_mul_ps(ky, kx), value, hist[index]);
                }
                hist += 18;

                for (size_t cell = 1, col = 4; cell < cellEnd; ++cell)
                {
                    for (size_t i = 0; i < 8; ++i, ++col)
                    {
                        int index = buffer.index[col];
                        __m128 value = _mm_set1_ps(buffer.value[col]);
                        __m128 kx = buffer.kx[i];
                        hist[index] = _mm_fmadd_ps(_mm_mul_ps(ky, kx), value, hist[index]);
                    }
                    hist += 18;
                }

                for (size_t col = width - 4; col < width - 1; ++col)
                {
                    int index = buffer.index[col];
                    __m128 value = _mm_set1_ps(buffer.value[col]);
                    __m128 kx = buffer.kx[(col + 4) & 7];
                    hist[index] = _mm_fmadd_ps(_mm_mul_ps(ky, kx), value, hist[index]);
                }
            }

            void AddToHistogram(Buffer & buffer, size_t row, size_t width, size_t height, float * histograms)
            {
                typedef float f18_t[18];

                float * src = (float*)buffer.hist;
                f18_t * h0 = (f18_t*)histograms + row * width - width - 1;
                f18_t * h1 = h0 + width;

                if (row == 0)
                {
                    for (size_t i = 0; i < 18; ++i)
                        h1[1][i] += src[i * 4 + 3];
                    h1++;
                    src += 72;
                    for (size_t cell = 1; cell < width; ++cell)
                    {
                        for (size_t i = 0; i < 18; ++i)
                        {
                            h1[0][i] += src[i * 4 + 2];
                            h1[1][i] += src[i * 4 + 3];
                        }
                        h1++;
                        src += 72;
                    }
                    for (size_t i = 0; i < 18; ++i)
                        h1[0][i] += src[i * 4 + 2];
                }
                else if (row == height)
                {
                    for (size_t i = 0; i < 18; ++i)
                        h0[1][i] += src[i * 4 + 1];
                    h0++;
                    src += 72;
                    for (size_t cell = 1; cell < width; ++cell)
                    {
                        for (size_t i = 0; i < 18; ++i)
                        {
                            h0[0][i] += src[i * 4 + 0];
                            h0[1][i] += src[i * 4 + 1];
                        }
                        h0++;
                        src += 72;
                    }
                    for (size_t i = 0; i < 18; ++i)
                        h0[0][i] += src[i * 4 + 0];
                }
                else
                {
                    for (size_t i = 0; i < 18; ++i)
                    {
                        h0[1][i] += src[i * 4 + 1];
                        h1[1][i] += src[i * 4 + 3];
                    }
                    h0++;
                    h1++;
                    src += 72;
                    for (size_t cell = 1; cell < width; ++cell)
                    {
                        __m512 a0 = Load<true>(src + 0x00, src + 0x10, src + 0x20, src + 0x30);
                        __m512 a1 = Load<true>(src + 0x04, src + 0x14, src + 0x24, src + 0x34);
                        __m512 a2 = Load<true>(src + 0x08, src + 0x18, src + 0x28, src + 0x38);
                        __m512 a3 = Load<true>(src + 0x0C, src + 0x1C, src + 0x2C, src + 0x3C);
                        __m512 b0 = _mm512_unpacklo_ps(a0, a2);
                        __m512 b1 = _mm512_unpackhi_ps(a0, a2);
                        __m512 b2 = _mm512_unpacklo_ps(a1, a3);
                        __m512 b3 = _mm512_unpackhi_ps(a1, a3);
                        Store<false>(h0[0], _mm512_add_ps(Load<false>(h0[0]), _mm512_unpacklo_ps(b0, b2)));
                        Store<false>(h0[1], _mm512_add_ps(Load<false>(h0[1]), _mm512_unpackhi_ps(b0, b2)));
                        Store<false>(h1[0], _mm512_add_ps(Load<false>(h1[0]), _mm512_unpacklo_ps(b1, b3)));
                        Store<false>(h1[1], _mm512_add_ps(Load<false>(h1[1]), _mm512_unpackhi_ps(b1, b3)));
                        for (size_t i = 16; i < 18; ++i)
                        {
                            h0[0][i] += src[i * 4 + 0];
                            h0[1][i] += src[i * 4 + 1];
                            h1[0][i] += src[i * 4 + 2];
                            h1[1][i] += src[i * 4 + 3];
                        }
                        h0++;
                        h1++;
                        src += 72;
                    }
                    for (size_t i = 0; i < 18; ++i)
                    {
                        h0[0][i] += src[i * 4 + 0];
                        h1[0][i] += src[i * 4 + 2];
                    }
                }
                buffer.ClearHist();
            }

            void HogDirectionHistograms(const uint8_t * src, size_t stride, size_t width, size_t height, float * histograms)
            {
                const size_t quantization = 18;

                size_t sizeX = width / 8, sizeY = height / 8;

                memset(histograms, 0, quantization*sizeX*sizeY * sizeof(float));

                Buffer buffer(width);

                size_t aligned = AlignLo(width - 2, HA) + 1;

                for (size_t row = 1; row < 4; ++row)
                    AddRowToBuffer(src, stride, buffer, row, width, aligned);
                AddToHistogram(buffer, 0, sizeX, sizeY, histograms);
                for (size_t row = 4, cell = 1; row < height - 4; ++row)
                {
                    AddRowToBuffer(src, stride, buffer, row, width, aligned);
                    if ((row & 7) == 3)
                        AddToHistogram(buffer, cell++, sizeX, sizeY, histograms);
                }
                for (size_t row = height - 4; row < height - 1; ++row)
                    AddRowToBuffer(src, stride, buffer, row, width, aligned);
                AddToHistogram(buffer, sizeY, sizeX, sizeY, histograms);
            }
        }

        void HogDirectionHistograms(const uint8_t * src, size_t stride, size_t width, size_t height,
            size_t cellX, size_t cellY, size_t quantization, float * histograms)
        {
            assert(width%cellX == 0 && height%cellY == 0 && quantization % 2 == 0);
            assert(width >= HA + 2);

            if (cellX == 8 && cellY == 8 && quantization == 18)
                Custom_8x8_18::HogDirectionHistograms(src, stride, width, height, histograms);
            else
            {
                memset(histograms, 0, quantization*(width / cellX)*(height / cellY) * sizeof(float));

                Buffer buffer(width, quantization);

                size_t alignedWidth = AlignLo(width - 2, HA) + 1;

                for (size_t row = 1; row < height - 1; ++row)
                {
                    const uint8_t * s = src + stride * row;
                    for (size_t col = 1; col < alignedWidth; col += HA)
                        HogDirectionHistograms<true>(s, stride, buffer, col);
                    HogDirectionHistograms<false>(s, stride, buffer, width - 1 - HA);
                    Base::AddRowToHistograms(buffer.index, buffer.value, row, width, height, cellX, cellY, quantization, histograms);
                }
            }
        }

        class HogFeatureExtractor
        {
            static const size_t C = 8;
            static const size_t Q = 9;
            static const size_t Q2 = 18;

            typedef Array<int> Array32i;
            typedef Array<float> Array32f;

            size_t _sx, _sy, _hs;

            __m512i _pos[5];
            __m512 _cos[5], _sin[5];
            __m128 _kx[8], _ky[8];
            __m512i _Q, _Q2;

            Array32i _index;
            Array32f _value;
            Array32f _buffer;
            Array32f _histogram;
            Array32f _norm;

            void Init(size_t w, size_t h)
            {
                _sx = w / C;
                _hs = _sx + 2;
                _sy = h / C;
                for (int i = 0; i < 5; ++i)
                {
                    _cos[i] = _mm512_set1_ps((float)::cos(i*M_PI / Q));
                    _sin[i] = _mm512_set1_ps((float)::sin(i*M_PI / Q));
                    _pos[i] = _mm512_set1_epi32(i);
                }
                for (int i = 0; i < C; ++i)
                {
                    float k0 = float((15 - i * 2) / 16.0f);
                    float k1 = 1.0f - k0;
                    _kx[i] = _mm_setr_ps(k0, k1, k0, k1);
                    _ky[i] = _mm_setr_ps(k0, k0, k1, k1);
                }
                _Q = _mm512_set1_epi32(Q);
                _Q2 = _mm512_set1_epi32(Q2);

                _index.Resize(w);
                _value.Resize(w);
                _buffer.Resize((_sx + 1) * 4 * Q2);
                _histogram.Resize((_sx + 2)*(_sy + 2)*Q2);
                _norm.Resize((_sx + 2)*(_sy + 2));
            }

            template <bool align> SIMD_INLINE void GetHistogram(const __m512 & dx, const __m512 & dy, size_t col)
            {
                __m512 _0 = _mm512_set1_ps(-0.0f);
                __m512 adx = _mm512_andnot_ps(_0, dx);
                __m512 ady = _mm512_andnot_ps(_0, dy);
                __m512 bestDot = _mm512_fmadd_ps(adx, _cos[0], _mm512_mul_ps(ady, _sin[0]));
                __m512i bestIndex = _pos[0];
                for (int i = 1; i < 5; ++i)
                {
                    __m512 dot = _mm512_fmadd_ps(adx, _cos[i], _mm512_mul_ps(ady, _sin[i]));
                    bestIndex = _mm512_mask_blend_epi32(_mm512_cmp_ps_mask(dot, bestDot, _CMP_GT_OS), bestIndex, _pos[i]);
                    bestDot = _mm512_max_ps(dot, bestDot);
                }
                bestIndex = _mm512_mask_sub_epi32(bestIndex, _mm512_cmp_ps_mask(dx, _0, _CMP_LT_OS), _Q, bestIndex);

                __m512i corr = _mm512_maskz_set1_epi32(_mm512_cmp_ps_mask(adx, _mm512_setzero_ps(), _CMP_EQ_OS), 1);
                bestIndex = _mm512_mask_sub_epi32(bestIndex, _mm512_cmp_ps_mask(dy, _0, _CMP_LT_OS), _Q2, _mm512_add_epi32(bestIndex, corr));

                bestIndex = _mm512_mask_set1_epi32(bestIndex, _mm512_cmpeq_epi32_mask(bestIndex, _Q2), 0);

                Store<align>(_index.data + col, bestIndex);
                Store<align>(_value.data + col, _mm512_sqrt_ps(_mm512_fmadd_ps(adx, adx, _mm512_mul_ps(ady, ady))));
            }

            template <bool align> SIMD_INLINE void GetHistogram(const uint8_t * src, size_t stride, size_t col)
            {
                const uint8_t * s = src + col;
                __m256i t = Avx2::LoadPermuted<false>((__m256i*)(s - stride));
                __m256i l = Avx2::LoadPermuted<false>((__m256i*)(s - 1));
                __m256i r = Avx2::LoadPermuted<false>((__m256i*)(s + 1));
                __m256i b = Avx2::LoadPermuted<false>((__m256i*)(s + stride));
                GetHistogram<align>(CovertDifference<0>(r, l), CovertDifference<0>(b, t), col + 0);
                GetHistogram<align>(CovertDifference<1>(r, l), CovertDifference<1>(b, t), col + F);
            }

            void AddRowToBuffer(const uint8_t * src, size_t stride, size_t row, size_t width, size_t aligned)
            {
                const uint8_t * s = src + stride * row;
                GetHistogram<false>(s, stride, 1);
                for (size_t col = HA; col < aligned; col += HA)
                    GetHistogram<false>(s, stride, col);
                GetHistogram<false>(s, stride, width - 1 - HA);

                __m128 ky = _ky[(row + 4) & 7];
                __m128 * buffer = (__m128*)_buffer.data;
                for (size_t col = 1, n = C, i = 5; col < width - 1; i = 0, n = Simd::Min<size_t>(C, width - col - 1))
                {
                    for (; i < n; ++i, ++col)
                    {
                        int index = _index[col];
                        __m128 value = _mm_set1_ps(_value[col]);
                        buffer[index] = _mm_fmadd_ps(_mm_mul_ps(ky, _kx[i]), value, buffer[index]);
                    }
                    buffer += Q2;
                }
            }

            void AddToHistogram(size_t row, size_t width, size_t height)
            {
                typedef float f18_t[18];

                float * src = _buffer.data;
                f18_t * h0 = (f18_t*)_histogram.data + row * _hs;
                f18_t * h1 = h0 + _hs;

                for (size_t cell = 0; cell <= width; ++cell)
                {
                    __m512 a0 = Load<true>(src + 0x00, src + 0x10, src + 0x20, src + 0x30);
                    __m512 a1 = Load<true>(src + 0x04, src + 0x14, src + 0x24, src + 0x34);
                    __m512 a2 = Load<true>(src + 0x08, src + 0x18, src + 0x28, src + 0x38);
                    __m512 a3 = Load<true>(src + 0x0C, src + 0x1C, src + 0x2C, src + 0x3C);
                    __m512 b0 = _mm512_unpacklo_ps(a0, a2);
                    __m512 b1 = _mm512_unpackhi_ps(a0, a2);
                    __m512 b2 = _mm512_unpacklo_ps(a1, a3);
                    __m512 b3 = _mm512_unpackhi_ps(a1, a3);
                    Store<false>(h0[0], _mm512_add_ps(Load<false>(h0[0]), _mm512_unpacklo_ps(b0, b2)));
                    Store<false>(h0[1], _mm512_add_ps(Load<false>(h0[1]), _mm512_unpackhi_ps(b0, b2)));
                    Store<false>(h1[0], _mm512_add_ps(Load<false>(h1[0]), _mm512_unpacklo_ps(b1, b3)));
                    Store<false>(h1[1], _mm512_add_ps(Load<false>(h1[1]), _mm512_unpackhi_ps(b1, b3)));
#if defined(_MSC_VER)
                    for (size_t i = 16; i < 18; ++i)
                    {
                        h0[0][i] += src[i * 4 + 0];
                        h0[1][i] += src[i * 4 + 1];
                        h1[0][i] += src[i * 4 + 2];
                        h1[1][i] += src[i * 4 + 3];
                    }
#else
                    __m128 * ps = (__m128*)src;
                    __m128 s0 = _mm_add_ps(_mm_unpacklo_ps(ps[16], ps[17]), Sse41::Load(h0[0] + 16, h0[1] + 16));
                    __m128 s1 = _mm_add_ps(_mm_unpackhi_ps(ps[16], ps[17]), Sse41::Load(h1[0] + 16, h1[1] + 16));
                    Sse41::StoreHalf<0>(h0[0] + 16, s0);
                    Sse41::StoreHalf<1>(h0[1] + 16, s0);
                    Sse41::StoreHalf<0>(h1[0] + 16, s1);
                    Sse41::StoreHalf<1>(h1[1] + 16, s1);
#endif
                    h0++;
                    h1++;
                    src += 72;
                }
                _buffer.Clear();
            }

            void EstimateHistogram(const uint8_t * src, size_t stride, size_t width, size_t height)
            {
                _histogram.Clear();

                size_t aligned = AlignHi(width - 1, HA) - HA;

                _buffer.Clear();
                for (size_t row = 1; row < 4; ++row)
                    AddRowToBuffer(src, stride, row, width, aligned);
                AddToHistogram(0, _sx, _sy);
                for (size_t row = 4, cell = 1; row < height - 4; ++row)
                {
                    AddRowToBuffer(src, stride, row, width, aligned);
                    if ((row & 7) == 3)
                        AddToHistogram(cell++, _sx, _sy);
                }
                for (size_t row = height - 4; row < height - 1; ++row)
                    AddRowToBuffer(src, stride, row, width, aligned);
                AddToHistogram(_sy, _sx, _sy);
            }

            SIMD_INLINE float GetNorm(const float * src)
            {
                __m256 norm = _mm256_add_ps(_mm256_loadu_ps(src), _mm256_loadu_ps(src + Q));
                norm = _mm256_mul_ps(norm, norm);
                norm = _mm256_hadd_ps(norm, norm);
                norm = _mm256_hadd_ps(norm, norm);
                float buf[8];
                _mm256_storeu_ps(buf, norm);
                return buf[0] + buf[4] + Simd::Square(src[Q - 1] + src[Q2 - 1]);
            }

            void EstimateNorm()
            {
                _norm.Clear();
                for (size_t y = 0, i = 0; y < _sy; y++)
                {
                    const float * h = _histogram.data + ((y + 1)*_hs + 1)*Q2;
                    float * n = _norm.data + (y + 1)*_hs + 1;
                    for (size_t x = 0; x < _sx; x++, i++)
                        n[x] = GetNorm(h + x * Q2);
                }
            }

            void ExtractFeatures(float * features)
            {
                __m128 _02 = _mm_set1_ps(0.2f);
                __m128 _05 = _mm_set1_ps(0.5f);
                __m128 _02357 = _mm_set1_ps(0.2357f);
                __m128 eps = _mm_set1_ps(0.0001f);
                for (size_t y = 0; y < _sy; y++)
                {
                    float * ph = _histogram.data + ((y + 1)*_hs + 1)*Q2;
                    for (size_t x = 0; x < _sx; x++)
                    {
                        float * dst = features + (y*_sx + x) * 31;

                        float * p0 = _norm.data + y * _hs + x;
                        float * p1 = p0 + _hs;
                        float * p2 = p1 + _hs;

                        __m128 n = _mm_setr_ps(
                            p1[1] + p1[2] + p2[1] + p2[2],
                            p0[1] + p0[2] + p1[1] + p1[2],
                            p1[0] + p1[1] + p2[0] + p2[1],
                            p0[0] + p0[1] + p1[0] + p1[1]);

                        n = _mm_rsqrt_ps(_mm_add_ps(n, eps));

                        __m128 t = _mm_setzero_ps();

                        float * src = ph + x * Q2;
                        for (int o = 0; o < 16; o += 4)
                        {
                            __m128 s = _mm_loadu_ps(src);
                            __m128 h0 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<0>(s), n), _02);
                            __m128 h1 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<1>(s), n), _02);
                            __m128 h2 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<2>(s), n), _02);
                            __m128 h3 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<3>(s), n), _02);
                            t = _mm_add_ps(t, _mm_add_ps(_mm_add_ps(h0, h1), _mm_add_ps(h2, h3)));
                            _mm_storeu_ps(dst, _mm_mul_ps(_05, _mm_hadd_ps(_mm_hadd_ps(h0, h1), _mm_hadd_ps(h2, h3))));
                            dst += 4;
                            src += 4;
                        }
                        {
                            __m128 h0 = _mm_min_ps(_mm_mul_ps(_mm_set1_ps(*src++), n), _02);
                            __m128 h1 = _mm_min_ps(_mm_mul_ps(_mm_set1_ps(*src++), n), _02);
                            t = _mm_add_ps(t, _mm_add_ps(h0, h1));
                            __m128 h = _mm_hadd_ps(h0, h1);
                            _mm_storeu_ps(dst, _mm_mul_ps(_05, _mm_hadd_ps(h, h)));
                            dst += 2;
                        }

                        src = ph + x * Q2;
                        for (int o = 0; o < 8; o += 4)
                        {
                            __m128 s = _mm_add_ps(_mm_loadu_ps(src), _mm_loadu_ps(src + Q));
                            __m128 h0 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<0>(s), n), _02);
                            __m128 h1 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<1>(s), n), _02);
                            __m128 h2 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<2>(s), n), _02);
                            __m128 h3 = _mm_min_ps(_mm_mul_ps(Sse41::Broadcast<3>(s), n), _02);
                            _mm_storeu_ps(dst, _mm_mul_ps(_05, _mm_hadd_ps(_mm_hadd_ps(h0, h1), _mm_hadd_ps(h2, h3))));
                            dst += 4;
                            src += 4;
                        }
                        {
                            __m128 s = _mm_set1_ps(src[0] + src[Q]);
                            __m128 h = _mm_min_ps(_mm_mul_ps(s, n), _02);
                            h = _mm_dp_ps(_05, h, 0xF1);
                            _mm_store_ss(dst++, h);
                        }
                        _mm_storeu_ps(dst, _mm_mul_ps(t, _02357));
                    }
                }
            }

        public:

            void Run(const uint8_t * src, size_t stride, size_t width, size_t height, float * features)
            {
                Init(width, height);

                EstimateHistogram(src, stride, width, height);

                EstimateNorm();

                ExtractFeatures(features);
            }
        };

        void HogExtractFeatures(const uint8_t * src, size_t stride, size_t width, size_t height, float * features)
        {
            assert(width % 8 == 0 && height % 8 == 0 && width >= 16 && height >= 16);
            assert(width >= HA + 2);

            HogFeatureExtractor extractor;
            extractor.Run(src, stride, width, height, features);
        }

        SIMD_INLINE void HogDeinterleave(const float * src, size_t count, float ** dst, size_t offset, size_t i)
        {
            src += i;
            __m512 a0 = Load<false>(src + 0x0 * count, src + 0x4 * count, src + 0x8 * count, src + 0xC * count);
            __m512 a1 = Load<false>(src + 0x1 * count, src + 0x5 * count, src + 0x9 * count, src + 0xD * count);
            __m512 a2 = Load<false>(src + 0x2 * count, src + 0x6 * count, src + 0xA * count, src + 0xE * count);
            __m512 a3 = Load<false>(src + 0x3 * count, src + 0x7 * count, src + 0xB * count, src + 0xF * count);
            __m512 b0 = _mm512_unpacklo_ps(a0, a2);
            __m512 b1 = _mm512_unpackhi_ps(a0, a2);
            __m512 b2 = _mm512_unpacklo_ps(a1, a3);
            __m512 b3 = _mm512_unpackhi_ps(a1, a3);
            Store<false>(dst[i + 0] + offset, _mm512_unpacklo_ps(b0, b2));
            Store<false>(dst[i + 1] + offset, _mm512_unpackhi_ps(b0, b2));
            Store<false>(dst[i + 2] + offset, _mm512_unpacklo_ps(b1, b3));
            Store<false>(dst[i + 3] + offset, _mm512_unpackhi_ps(b1, b3));
        }

        void HogDeinterleave(const float * src, size_t srcStride, size_t width, size_t height, size_t count, float ** dst, size_t dstStride)
        {
            assert(width >= F && count >= Sse41::F);

            size_t alignedCount = AlignLo(count, Sse41::F);
            size_t alignedWidth = AlignLo(width, F);

            for (size_t row = 0; row < height; ++row)
            {
                size_t rowOffset = row * dstStride;
                for (size_t col = 0; col < alignedWidth; col += F)
                {
                    const float * s = src + count * col;
                    size_t offset = rowOffset + col;
                    for (size_t i = 0; i < alignedCount; i += Sse41::F)
                        HogDeinterleave(s, count, dst, offset, i);
                    if (alignedCount != count)
                        HogDeinterleave(s, count, dst, offset, count - Sse41::F);
                }
                if (alignedWidth != width)
                {
                    size_t col = width - F;
                    const float * s = src + count * col;
                    size_t offset = rowOffset + col;
                    for (size_t i = 0; i < alignedCount; i += Sse41::F)
                        HogDeinterleave(s, count, dst, offset, i);
                    if (alignedCount != count)
                        HogDeinterleave(s, count, dst, offset, count - Sse41::F);
                }
                src += srcStride;
            }
        }

        namespace HogSeparableFilter_Detail
        {
            template <int add, bool mask> SIMD_INLINE void Set(float * dst, const __m512 & value, __mmask16 tail = -1)
            {
                Store<false, mask>(dst, value, tail);
            }

            template <> SIMD_INLINE void Set<1, false>(float * dst, const __m512 & value, __mmask16 tail)
            {
                Store<false>(dst, _mm512_add_ps(Load<false>(dst), value));
            }

            template <> SIMD_INLINE void Set<1, true>(float * dst, const __m512 & value, __mmask16 tail)
            {
                Store<false, true>(dst, _mm512_add_ps((Load<false, true>(dst, tail)), value), tail);
            }
        }

        class HogSeparableFilter
        {
            size_t _w, _h, _s;
            Array32f _buffer;
            Array512f _filter;

            SIMD_INLINE void Init(size_t w, size_t h, size_t rs, size_t cs)
            {
                _w = w - rs + 1;
                _s = AlignHi(_w, F);
                _h = h - cs + 1;
                _buffer.Resize(_s*h);
            }

            template <bool align> SIMD_INLINE void FilterRows(const float * src, const __m512 * filter, size_t size, float * dst)
            {
                __m512 sum = _mm512_setzero_ps();
                for (size_t i = 0; i < size; ++i)
                    sum = _mm512_fmadd_ps(Load<false>(src + i), filter[i], sum);
                Store<align>(dst, sum);
            }

            void FilterRows(const float * src, size_t srcStride, size_t width, size_t height, const float * filter, size_t size, float * dst, size_t dstStride)
            {
                _filter.Resize(size);
                for (size_t i = 0; i < size; ++i)
                    _filter[i] = _mm512_set1_ps(filter[i]);

                size_t alignedWidth = AlignLo(width, F);

                for (size_t row = 0; row < height; ++row)
                {
                    for (size_t col = 0; col < alignedWidth; col += F)
                        FilterRows<true>(src + col, _filter.data, size, dst + col);
                    if (alignedWidth != width)
                        FilterRows<false>(src + width - F, _filter.data, size, dst + width - F);
                    src += srcStride;
                    dst += dstStride;
                }
            }

            template <bool align> SIMD_INLINE void FilterRows_10(const float * src, const __m512 * filter, float * dst)
            {
                __m512  src0 = Load<false>(src + 0);
                __m512  srcf = Load<false>(src + F);
                __m512 sum0 = _mm512_mul_ps(Alignr<0>(src0, srcf), filter[0]);
                __m512 sum1 = _mm512_mul_ps(Alignr<1>(src0, srcf), filter[1]);
                sum0 = _mm512_fmadd_ps(Alignr<2>(src0, srcf), filter[2], sum0);
                sum1 = _mm512_fmadd_ps(Alignr<3>(src0, srcf), filter[3], sum1);
                sum0 = _mm512_fmadd_ps(Alignr<4>(src0, srcf), filter[4], sum0);
                sum1 = _mm512_fmadd_ps(Alignr<5>(src0, srcf), filter[5], sum1);
                sum0 = _mm512_fmadd_ps(Alignr<6>(src0, srcf), filter[6], sum0);
                sum1 = _mm512_fmadd_ps(Alignr<7>(src0, srcf), filter[7], sum1);
                sum0 = _mm512_fmadd_ps(Alignr<8>(src0, srcf), filter[8], sum0);
                sum1 = _mm512_fmadd_ps(Alignr<9>(src0, srcf), filter[9], sum1);
                Store<align>(dst, _mm512_add_ps(sum0, sum1));
            }

            void FilterRows_10(const float * src, size_t srcStride, size_t width, size_t height, const float * filter, float * dst, size_t dstStride)
            {
                __m512 _filter[10];
                for (size_t i = 0; i < 10; ++i)
                    _filter[i] = _mm512_set1_ps(filter[i]);

                size_t alignedWidth = AlignLo(width, F);

                for (size_t row = 0; row < height; ++row)
                {
                    for (size_t col = 0; col < alignedWidth; col += F)
                        FilterRows_10<true>(src + col, _filter, dst + col);
                    if (alignedWidth != width)
                        FilterRows_10<false>(src + width - F, _filter, dst + width - F);
                    src += srcStride;
                    dst += dstStride;
                }
            }

            template <int add, bool mask> SIMD_INLINE void FilterCols(const float * src, size_t stride, const __m512 * filter, size_t size, float * dst, __mmask16 tail = -1)
            {
                __m512 sum = _mm512_setzero_ps();
                for (size_t i = 0; i < size; ++i, src += stride)
                    sum = _mm512_fmadd_ps((Load<true, mask>(src, tail)), filter[i], sum);
                HogSeparableFilter_Detail::Set<add, mask>(dst, sum, tail);
            }

            template <int add> void SIMD_INLINE FilterCols4x(const float * src, size_t stride, const __m512 * filter, size_t size, float * dst)
            {
                __m512 sums[4] = { _mm512_setzero_ps(), _mm512_setzero_ps(), _mm512_setzero_ps(), _mm512_setzero_ps() };
                for (size_t i = 0; i < size; ++i, src += stride)
                {
                    __m512 f = filter[i];
                    sums[0] = _mm512_fmadd_ps(Load<true>(src + 0 * F), f, sums[0]);
                    sums[1] = _mm512_fmadd_ps(Load<true>(src + 1 * F), f, sums[1]);
                    sums[2] = _mm512_fmadd_ps(Load<true>(src + 2 * F), f, sums[2]);
                    sums[3] = _mm512_fmadd_ps(Load<true>(src + 3 * F), f, sums[3]);
                }
                HogSeparableFilter_Detail::Set<add, false>(dst + 0 * F, sums[0]);
                HogSeparableFilter_Detail::Set<add, false>(dst + 1 * F, sums[1]);
                HogSeparableFilter_Detail::Set<add, false>(dst + 2 * F, sums[2]);
                HogSeparableFilter_Detail::Set<add, false>(dst + 3 * F, sums[3]);
            }

            template <int add> void FilterCols(const float * src, size_t srcStride, size_t width, size_t height, const float * filter, size_t size, float * dst, size_t dstStride)
            {
                _filter.Resize(size);
                for (size_t i = 0; i < size; ++i)
                    _filter[i] = _mm512_set1_ps(filter[i]);

                size_t fullAlignedWidth = AlignLo(width, QF);
                size_t alignedWidth = AlignLo(width, F);
                __mmask16 tailMask = TailMask16(width - alignedWidth);

                for (size_t row = 0; row < height; ++row)
                {
                    size_t col = 0;
                    for (; col < fullAlignedWidth; col += QF)
                        FilterCols4x<add>(src + col, srcStride, _filter.data, size, dst + col);
                    for (; col < alignedWidth; col += F)
                        FilterCols<add, false>(src + col, srcStride, _filter.data, size, dst + col);
                    if (col < width)
                        FilterCols<add, true>(src + col, srcStride, _filter.data, size, dst + col, tailMask);
                    src += srcStride;
                    dst += dstStride;
                }
            }

        public:

            void Run(const float * src, size_t srcStride, size_t width, size_t height,
                const float * rowFilter, size_t rowSize, const float * colFilter, size_t colSize, float * dst, size_t dstStride, int add)
            {
                Init(width, height, rowSize, colSize);

                if (colSize == 10)
                    FilterRows_10(src, srcStride, _w, height, rowFilter, _buffer.data, _s);
                else
                    FilterRows(src, srcStride, _w, height, rowFilter, rowSize, _buffer.data, _s);

                if (add)
                    FilterCols<1>(_buffer.data, _s, _w, _h, colFilter, colSize, dst, dstStride);
                else
                    FilterCols<0>(_buffer.data, _s, _w, _h, colFilter, colSize, dst, dstStride);
            }
        };

        void HogFilterSeparable(const float * src, size_t srcStride, size_t width, size_t height,
            const float * rowFilter, size_t rowSize, const float * colFilter, size_t colSize, float * dst, size_t dstStride, int add)
        {
            assert(width >= F + rowSize - 1 && height >= colSize - 1);

            HogSeparableFilter filter;
            filter.Run(src, srcStride, width, height, rowFilter, rowSize, colFilter, colSize, dst, dstStride, add);
        }
    }
#endif
}
