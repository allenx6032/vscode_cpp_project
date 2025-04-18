/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2021 Yermalayeu Ihar.
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
#include "SimdWinograd.h"
#include "SimdBase.h"
#include "SimdSet.h"

namespace Simd
{
#if defined(SIMD_NEON_ENABLE) && defined(SIMD_SYNET_ENABLE) 
    namespace Neon
    {
        SIMD_INLINE void WinogradKernel2x2Block2x2SetFilter(const float32x4_t src[4], float* dst, size_t stride)
        {
            Store<false>(dst + 0 * stride, src[0]);
            Store<false>(dst + 1 * stride, vaddq_f32(src[0], src[1]));
            Store<false>(dst + 2 * stride, src[1]);

            Store<false>(dst + 3 * stride, vaddq_f32(src[0], src[2]));
            Store<false>(dst + 4 * stride, vaddq_f32(vaddq_f32(src[0], src[1]), vaddq_f32(src[2], src[3])));
            Store<false>(dst + 5 * stride, vaddq_f32(src[1], src[3]));

            Store<false>(dst + 6 * stride, src[2]);
            Store<false>(dst + 7 * stride, vaddq_f32(src[2], src[3]));
            Store<false>(dst + 8 * stride, src[3]);
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetFilter4n(const float* src, float* dst, size_t stride)
        {
            float32x4_t _src[4];
            Load4(src + 0, 4, _src + 0);
            WinogradKernel2x2Block2x2SetFilter(_src, dst, stride);
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetFilter4t(const float* src, float* dst, size_t stride)
        {
            float32x4_t _src[4];
            _src[0] = Load<false>(src + 0 * stride);
            _src[1] = Load<false>(src + 1 * stride);
            _src[2] = Load<false>(src + 2 * stride);
            _src[3] = Load<false>(src + 3 * stride);
            WinogradKernel2x2Block2x2SetFilter(_src, dst, stride);
        }

        void WinogradKernel2x2Block2x2SetFilter(const float* src, size_t size, float* dst, SimdBool trans)
        {
            size_t size4 = AlignLo(size, 4), i = 0;
            if (trans)
            {
                for (; i < size4; i += 4)
                    WinogradKernel2x2Block2x2SetFilter4t(src + i, dst + i, size);
                for (; i < size; i += 1)
                    Base::WinogradKernel2x2Block2x2SetFilter1t(src + i, dst + i, size);
            }
            else
            {
                for (; i < size4; i += 4, src += 16, dst += 4)
                    WinogradKernel2x2Block2x2SetFilter4n(src, dst, size);
                for (; i < size; i += 1, src += 4, dst += 1)
                    Base::WinogradKernel2x2Block2x2SetFilter1n(src, dst, size);
            }
        }

        //-----------------------------------------------------------------------

        SIMD_INLINE void WinogradKernel2x2Block2x2SetInput4Store(const float32x4_t* src, float* dst, size_t stride)
        {
            Store<false>(dst + 0 * stride, vaddq_f32(vsubq_f32(src[0], src[1]), vsubq_f32(src[4], src[3])));
            Store<false>(dst + 1 * stride, vsubq_f32(src[1], src[4]));
            Store<false>(dst + 2 * stride, vaddq_f32(vsubq_f32(src[2], src[1]), vsubq_f32(src[4], src[5])));
            Store<false>(dst + 3 * stride, vsubq_f32(src[3], src[4]));
            Store<false>(dst + 4 * stride, src[4]);
            Store<false>(dst + 5 * stride, vsubq_f32(src[5], src[4]));
            Store<false>(dst + 6 * stride, vaddq_f32(vsubq_f32(src[4], src[3]), vsubq_f32(src[6], src[7])));
            Store<false>(dst + 7 * stride, vsubq_f32(src[7], src[4]));
            Store<false>(dst + 8 * stride, vaddq_f32(vsubq_f32(src[4], src[5]), vsubq_f32(src[8], src[7])));
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetInput4t(const float* src, size_t srcS, size_t srcC, float32x4_t dst[9])
        {
            dst[0] = Load<false>(src + 0 * srcS + 0 * srcC);
            dst[1] = Load<false>(src + 0 * srcS + 1 * srcC);
            dst[2] = Load<false>(src + 0 * srcS + 2 * srcC);
            dst[3] = Load<false>(src + 1 * srcS + 0 * srcC);
            dst[4] = Load<false>(src + 1 * srcS + 1 * srcC);
            dst[5] = Load<false>(src + 1 * srcS + 2 * srcC);
            dst[6] = Load<false>(src + 2 * srcS + 0 * srcC);
            dst[7] = Load<false>(src + 2 * srcS + 1 * srcC);
            dst[8] = Load<false>(src + 2 * srcS + 2 * srcC);
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetInput4t(const float* src, size_t srcW, size_t srcC, float* dst, size_t dstStride)
        {
            size_t srcS = srcW * srcC;
            size_t srcCF = AlignLo(srcC, F);
            for (size_t c = 0; c < srcCF; c += F)
            {
                float32x4_t tmp[9];
                WinogradKernel2x2Block2x2SetInput4t(src + c, srcS, srcC, tmp);
                WinogradKernel2x2Block2x2SetInput4Store(tmp, dst + c, dstStride);
            }
            if (srcCF < srcC)
            {
                float32x4_t tmp[9];
                WinogradKernel2x2Block2x2SetInput4t(src + srcC - F, srcS, srcC, tmp);
                WinogradKernel2x2Block2x2SetInput4Store(tmp, dst + srcC - F, dstStride);
            }
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetInput4t(const float* src, size_t srcS, size_t srcC, size_t rowB, size_t rowE, size_t colB, size_t colE, float32x4_t dst[9])
        {
            for (size_t i = 0; i < 9; ++i)
                dst[i] = vdupq_n_f32(0.0f);
            for (size_t row = rowB; row < rowE; ++row)
                for (size_t col = colB; col < colE; ++col)
                    dst[row * 3 + col] = Load<false>(src + row * srcS + col * srcC);
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetInput4t(const float* src, size_t srcW, size_t srcC, size_t rowB, size_t rowE, size_t colB, size_t colE, float* dst, size_t dstStride)
        {
            size_t srcS = srcW * srcC;
            size_t srcCF = AlignLo(srcC, F);
            for (size_t c = 0; c < srcCF; c += F)
            {
                float32x4_t tmp[9];
                WinogradKernel2x2Block2x2SetInput4t(src + c, srcS, srcC, rowB, rowE, colB, colE, tmp);
                WinogradKernel2x2Block2x2SetInput4Store(tmp, dst + c, dstStride);
            }
            if (srcCF < srcC)
            {
                float32x4_t tmp[9];
                WinogradKernel2x2Block2x2SetInput4t(src + srcC - F, srcS, srcC, rowB, rowE, colB, colE, tmp);
                WinogradKernel2x2Block2x2SetInput4Store(tmp, dst + srcC - F, dstStride);
            }
        }

        void WinogradKernel2x2Block2x2SetInput(const float* src, size_t srcChannels, size_t srcHeight, size_t srcWidth,
            size_t padY, size_t padX, size_t padH, size_t padW, float* dst, size_t dstStride, SimdBool trans)
        {
            assert(padY == padX && padW == padH && (padY + padH == 0 || padY + padH == 1));
            if (trans ? (srcChannels < F) : true)
            {
                Base::WinogradKernel2x2Block2x2SetInput(src, srcChannels, srcHeight, srcWidth, padY, padX, padH, padW, dst, dstStride, trans);
                return;
            }
            size_t dstH = srcHeight - 1 + padY + padH;
            size_t dstW = srcWidth - 1 + padX + padW;
            size_t dstH2 = AlignLo(dstH, 2);
            size_t dstW2 = AlignLo(dstW, 2);
            size_t noseW = Simd::Min<size_t>(3, dstW + 1);
            size_t noseH = Simd::Min<size_t>(3, dstH + 1);
            size_t startY = padY ? 2 : 0;
            size_t startX = padX ? 2 : 0;
            if (padY || padH)
            {
                if (dstH == dstH2)
                    dstH2 -= 2;
                if (dstW == dstW2)
                    dstW2 -= 2;
                if (padY)
                    src -= (srcWidth + 1) * (trans ? srcChannels : 1);
            }
            size_t tailW = dstW - dstW2 + (padW ? 0 : 1);
            size_t tailH = dstH - dstH2 + (padH ? 0 : 1);
            size_t row = 0, col = 0;
            if (padY)
            {
                if (padX)
                    WinogradKernel2x2Block2x2SetInput4t(src, srcWidth, srcChannels, 1, noseH, 1, noseW, dst, dstStride), dst += srcChannels;
                for (col = startX; col < dstW2; col += 2)
                    WinogradKernel2x2Block2x2SetInput4t(src + col * srcChannels, srcWidth, srcChannels, 1, noseH, 0, 3, dst, dstStride), dst += srcChannels;
                if (col < dstW)
                    WinogradKernel2x2Block2x2SetInput4t(src + col * srcChannels, srcWidth, srcChannels, 1, noseH, 0, tailW, dst, dstStride), dst += srcChannels;
            }
            for (row = startY; row < dstH2; row += 2)
            {
                if (padX)
                    WinogradKernel2x2Block2x2SetInput4t(src + row * srcWidth * srcChannels, srcWidth, srcChannels, 0, 3, 1, noseW, dst, dstStride), dst += srcChannels;
                for (col = startX; col < dstW2; col += 2)
                    WinogradKernel2x2Block2x2SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, dst, dstStride), dst += srcChannels;
                if (col < dstW)
                    WinogradKernel2x2Block2x2SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, 0, 3, 0, tailW, dst, dstStride), dst += srcChannels;
            }
            if (row < dstH)
            {
                if (padX)
                    WinogradKernel2x2Block2x2SetInput4t(src + row * srcWidth * srcChannels, srcWidth, srcChannels, 0, tailH, 1, noseW, dst, dstStride), dst += srcChannels;
                for (col = startX; col < dstW2; col += 2)
                    WinogradKernel2x2Block2x2SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, 0, tailH, 0, 3, dst, dstStride), dst += srcChannels;
                if (col < dstW)
                    WinogradKernel2x2Block2x2SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, 0, tailH, 0, tailW, dst, dstStride), dst += srcChannels;
            }
        }

        //-----------------------------------------------------------------------

        SIMD_INLINE void WinogradKernel2x2Block2x2SetOutputLoad9(const float* src, size_t stride, float32x4_t* dst)
        {
            float32x4_t s[9];
            s[0] = Load<false>(src + 0 * stride);
            s[1] = Load<false>(src + 1 * stride);
            s[2] = Load<false>(src + 2 * stride);
            s[3] = Load<false>(src + 3 * stride);
            s[4] = Load<false>(src + 4 * stride);
            s[5] = Load<false>(src + 5 * stride);
            s[6] = Load<false>(src + 6 * stride);
            s[7] = Load<false>(src + 7 * stride);
            s[8] = Load<false>(src + 8 * stride);
            dst[0] = vaddq_f32(vaddq_f32(s[0], s[1]), vaddq_f32(s[3], s[4]));
            dst[1] = vaddq_f32(vaddq_f32(s[1], s[2]), vaddq_f32(s[4], s[5]));
            dst[2] = vaddq_f32(vaddq_f32(s[3], s[4]), vaddq_f32(s[6], s[7]));
            dst[3] = vaddq_f32(vaddq_f32(s[4], s[5]), vaddq_f32(s[7], s[8]));
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetOutputStore4(const float32x4_t src[4], float* dst, size_t dstS, size_t dstC)
        {
            Store<false>(dst + 0 * dstS + 0 * dstC, src[0]);
            Store<false>(dst + 0 * dstS + 1 * dstC, src[1]);
            Store<false>(dst + 1 * dstS + 0 * dstC, src[2]);
            Store<false>(dst + 1 * dstS + 1 * dstC, src[3]);
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetOutput4t(const float* src, size_t srcStride, float* dst, size_t dstW, size_t dstC)
        {
            size_t dstS = dstW * dstC, dstCF = AlignLo(dstC, F);
            for (size_t d = 0; d < dstCF; d += F)
            {
                float32x4_t tmp[4];
                WinogradKernel2x2Block2x2SetOutputLoad9(src + d, srcStride, tmp);
                WinogradKernel2x2Block2x2SetOutputStore4(tmp, dst + d, dstS, dstC);
            }
            if (dstCF < dstC)
            {
                float32x4_t tmp[4];
                WinogradKernel2x2Block2x2SetOutputLoad9(src + dstC - F, srcStride, tmp);
                WinogradKernel2x2Block2x2SetOutputStore4(tmp, dst + dstC - F, dstS, dstC);
            }
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetOutputStore4(const float32x4_t src[4], float* dst, size_t dstS, size_t dstC, size_t rowE, size_t colE)
        {
            for (size_t row = 0; row < rowE; ++row)
                for (size_t col = 0; col < colE; ++col)
                    Store<false>(dst + row * dstS + col * dstC, src[row * 2 + col]);
        }

        SIMD_INLINE void WinogradKernel2x2Block2x2SetOutput4t(const float* src, size_t srcStride, float* dst, size_t dstW, size_t dstC, size_t rowE, size_t colE)
        {
            size_t dstS = dstW * dstC, dstCF = AlignLo(dstC, F);
            for (size_t d = 0; d < dstCF; d += F)
            {
                float32x4_t tmp[4];
                WinogradKernel2x2Block2x2SetOutputLoad9(src + d, srcStride, tmp);
                WinogradKernel2x2Block2x2SetOutputStore4(tmp, dst + d, dstS, dstC, rowE, colE);
            }
            if (dstCF < dstC)
            {
                float32x4_t tmp[4];
                WinogradKernel2x2Block2x2SetOutputLoad9(src + dstC - F, srcStride, tmp);
                WinogradKernel2x2Block2x2SetOutputStore4(tmp, dst + dstC - F, dstS, dstC, rowE, colE);
            }
        }

        void WinogradKernel2x2Block2x2SetOutput(const float* src, size_t srcStride, float* dst, size_t dstChannels, size_t dstHeight, size_t dstWidth, SimdBool trans)
        {
            if (trans ? (dstChannels < F) : true)
            {
                Base::WinogradKernel2x2Block2x2SetOutput(src, srcStride, dst, dstChannels, dstHeight, dstWidth, trans);
                return;
            }
            size_t tileH = (dstHeight + 1) / 2;
            size_t tileW = (dstWidth + 1) / 2;
            size_t dstH2 = AlignLo(dstHeight, 2);
            size_t dstW2 = AlignLo(dstWidth, 2);
            size_t row, col;
            for (row = 0; row < dstH2; row += 2)
            {
                for (col = 0; col < dstW2; col += 2)
                    WinogradKernel2x2Block2x2SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels), src += dstChannels;
                if (col < dstWidth)
                    WinogradKernel2x2Block2x2SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels, 2, dstWidth - col), src += dstChannels;
            }
            if (row < dstHeight)
            {
                for (col = 0; col < dstW2; col += 2)
                    WinogradKernel2x2Block2x2SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels, dstHeight - row, 2), src += dstChannels;
                if (col < dstWidth)
                    WinogradKernel2x2Block2x2SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels, dstHeight - row, dstWidth - col), src += dstChannels;
            }
        }
        
        //-----------------------------------------------------------------------

        SIMD_INLINE void WinogradKernel2x2Block4x4SetFilterRow(const float32x4_t* t, float* dst, size_t stride)
        {
            const float32x4_t r2 = vdupq_n_f32(1.0f / 2.0f);
            const float32x4_t r3 = vdupq_n_f32(1.0f / 3.0f);
            const float32x4_t r6 = vdupq_n_f32(1.0f / 6.0f);
            const float32x4_t mr2 = vdupq_n_f32(-1.0f / 2.0f);

            Store<false>(dst + 0 * stride, vmulq_f32(r2, t[0]));
            Store<false>(dst + 1 * stride, vmulq_f32(mr2, vaddq_f32(t[0], t[1])));
            Store<false>(dst + 2 * stride, vmulq_f32(r6, vsubq_f32(t[1], t[0])));
            Store<false>(dst + 3 * stride, vaddq_f32(vmulq_f32(r6, t[0]), vmulq_f32(r3, t[1])));
            Store<false>(dst + 4 * stride, t[1]);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetFilter(const float32x4_t src[4], float* dst, size_t stride)
        {
            const float32x4_t r2 = vdupq_n_f32(1.0f / 2.0f);
            const float32x4_t r3 = vdupq_n_f32(1.0f / 3.0f);
            const float32x4_t r6 = vdupq_n_f32(1.0f / 6.0f);
            const float32x4_t mr2 = vdupq_n_f32(-1.0f / 2.0f);

            float32x4_t t[2];
            t[0] = vmulq_f32(r2, src[0]);
            t[1] = vmulq_f32(r2, src[1]);
            WinogradKernel2x2Block4x4SetFilterRow(t, dst + 0 * stride, stride);

            t[0] = vmulq_f32(mr2, vaddq_f32(src[0], src[2]));
            t[1] = vmulq_f32(mr2, vaddq_f32(src[1], src[3]));
            WinogradKernel2x2Block4x4SetFilterRow(t, dst + 5 * stride, stride);

            t[0] = vmulq_f32(r6, vsubq_f32(src[2], src[0]));
            t[1] = vmulq_f32(r6, vsubq_f32(src[3], src[1]));
            WinogradKernel2x2Block4x4SetFilterRow(t, dst + 10 * stride, stride);

            t[0] = vaddq_f32(vmulq_f32(r6, src[0]), vmulq_f32(r3, src[2]));
            t[1] = vaddq_f32(vmulq_f32(r6, src[1]), vmulq_f32(r3, src[3]));
            WinogradKernel2x2Block4x4SetFilterRow(t, dst + 15 * stride, stride);

            t[0] = src[2];
            t[1] = src[3];
            WinogradKernel2x2Block4x4SetFilterRow(t, dst + 20 * stride, stride);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetFilter4n(const float* src, float* dst, size_t stride)
        {
            float32x4_t _src[4];
            Load4(src + 0, 4, _src + 0);
            WinogradKernel2x2Block4x4SetFilter(_src, dst, stride);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetFilter4t(const float* src, float* dst, size_t stride)
        {
            float32x4_t _src[4];
            _src[0] = Load<false>(src + 0 * stride);
            _src[1] = Load<false>(src + 1 * stride);
            _src[2] = Load<false>(src + 2 * stride);
            _src[3] = Load<false>(src + 3 * stride);
            WinogradKernel2x2Block4x4SetFilter(_src, dst, stride);
        }

        void WinogradKernel2x2Block4x4SetFilter(const float* src, size_t size, float* dst, SimdBool trans)
        {
            size_t size4 = AlignLo(size, 4), i = 0;
            if (trans)
            {
                for (; i < size4; i += 4)
                    WinogradKernel2x2Block4x4SetFilter4t(src + i, dst + i, size);
                for (; i < size; i += 1)
                    Base::WinogradKernel2x2Block4x4SetFilter1t(src + i, dst + i, size);
            }
            else
            {
                for (; i < size4; i += 4, src += 16, dst += 4)
                    WinogradKernel2x2Block4x4SetFilter4n(src, dst, size);
                for (; i < size; i += 1, src += 4, dst += 1)
                    Base::WinogradKernel2x2Block4x4SetFilter1n(src, dst, size);
            }
        }

        //-----------------------------------------------------------------------

        SIMD_INLINE void WinogradKernel2x2Block4x4SetInputStoreRow(const float32x4_t tmp[5], float* dst, size_t stride)
        {
            const float32x4_t _2 = vdupq_n_f32(2.0f);
            const float32x4_t _3 = vdupq_n_f32(3.0f);
            Store<false>(dst + 0 * stride, vaddq_f32(vsubq_f32(vmulq_f32(_2, tmp[0]), tmp[1]), vsubq_f32(tmp[3], vmulq_f32(_2, tmp[2]))));
            Store<false>(dst + 1 * stride, vsubq_f32(tmp[3], vaddq_f32(vmulq_f32(_2, tmp[1]), tmp[2])));
            Store<false>(dst + 2 * stride, vaddq_f32(vsubq_f32(vmulq_f32(_2, tmp[1]), vmulq_f32(_3, tmp[2])), tmp[3]));
            Store<false>(dst + 3 * stride, vsubq_f32(tmp[3], tmp[1]));
            Store<false>(dst + 4 * stride, vaddq_f32(vsubq_f32(vmulq_f32(_2, tmp[1]), tmp[2]), vsubq_f32(tmp[4], vmulq_f32(_2, tmp[3]))));
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetInputStore(const float32x4_t* src, float* dst, size_t stride)
        {
            const float32x4_t _2 = vdupq_n_f32(2.0f);
            const float32x4_t _3 = vdupq_n_f32(3.0f);
            float32x4_t tmp[5];
            tmp[0] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[0]), src[5]), vsubq_f32(src[15], vmulq_f32(_2, src[10])));
            tmp[1] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[1]), src[6]), vsubq_f32(src[16], vmulq_f32(_2, src[11])));
            tmp[2] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[2]), src[7]), vsubq_f32(src[17], vmulq_f32(_2, src[12])));
            tmp[3] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[3]), src[8]), vsubq_f32(src[18], vmulq_f32(_2, src[13])));
            tmp[4] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[4]), src[9]), vsubq_f32(src[19], vmulq_f32(_2, src[14])));
            WinogradKernel2x2Block4x4SetInputStoreRow(tmp, dst + 0 * stride, stride);

            tmp[0] = vsubq_f32(src[15], vaddq_f32(vmulq_f32(_2, src[5]), src[10]));
            tmp[1] = vsubq_f32(src[16], vaddq_f32(vmulq_f32(_2, src[6]), src[11]));
            tmp[2] = vsubq_f32(src[17], vaddq_f32(vmulq_f32(_2, src[7]), src[12]));
            tmp[3] = vsubq_f32(src[18], vaddq_f32(vmulq_f32(_2, src[8]), src[13]));
            tmp[4] = vsubq_f32(src[19], vaddq_f32(vmulq_f32(_2, src[9]), src[14]));
            WinogradKernel2x2Block4x4SetInputStoreRow(tmp, dst + 5 * stride, stride);

            tmp[0] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[5]), vmulq_f32(_3, src[10])), src[15]);
            tmp[1] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[6]), vmulq_f32(_3, src[11])), src[16]);
            tmp[2] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[7]), vmulq_f32(_3, src[12])), src[17]);
            tmp[3] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[8]), vmulq_f32(_3, src[13])), src[18]);
            tmp[4] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[9]), vmulq_f32(_3, src[14])), src[19]);
            WinogradKernel2x2Block4x4SetInputStoreRow(tmp, dst + 10 * stride, stride);

            tmp[0] = vsubq_f32(src[15], src[5]);
            tmp[1] = vsubq_f32(src[16], src[6]);
            tmp[2] = vsubq_f32(src[17], src[7]);
            tmp[3] = vsubq_f32(src[18], src[8]);
            tmp[4] = vsubq_f32(src[19], src[9]);
            WinogradKernel2x2Block4x4SetInputStoreRow(tmp, dst + 15 * stride, stride);

            tmp[0] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[5]), src[10]), vsubq_f32(src[20], vmulq_f32(_2, src[15])));
            tmp[1] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[6]), src[11]), vsubq_f32(src[21], vmulq_f32(_2, src[16])));
            tmp[2] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[7]), src[12]), vsubq_f32(src[22], vmulq_f32(_2, src[17])));
            tmp[3] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[8]), src[13]), vsubq_f32(src[23], vmulq_f32(_2, src[18])));
            tmp[4] = vaddq_f32(vsubq_f32(vmulq_f32(_2, src[9]), src[14]), vsubq_f32(src[24], vmulq_f32(_2, src[19])));
            WinogradKernel2x2Block4x4SetInputStoreRow(tmp, dst + 20 * stride, stride);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetInput4t(const float* src, size_t srcS, size_t srcC, float32x4_t dst[25])
        {
            dst[0] = Load<false>(src + 0 * srcS + 0 * srcC);
            dst[1] = Load<false>(src + 0 * srcS + 1 * srcC);
            dst[2] = Load<false>(src + 0 * srcS + 2 * srcC);
            dst[3] = Load<false>(src + 0 * srcS + 3 * srcC);
            dst[4] = Load<false>(src + 0 * srcS + 4 * srcC);
            dst[5] = Load<false>(src + 1 * srcS + 0 * srcC);
            dst[6] = Load<false>(src + 1 * srcS + 1 * srcC);
            dst[7] = Load<false>(src + 1 * srcS + 2 * srcC);
            dst[8] = Load<false>(src + 1 * srcS + 3 * srcC);
            dst[9] = Load<false>(src + 1 * srcS + 4 * srcC);
            dst[10] = Load<false>(src + 2 * srcS + 0 * srcC);
            dst[11] = Load<false>(src + 2 * srcS + 1 * srcC);
            dst[12] = Load<false>(src + 2 * srcS + 2 * srcC);
            dst[13] = Load<false>(src + 2 * srcS + 3 * srcC);
            dst[14] = Load<false>(src + 2 * srcS + 4 * srcC);
            dst[15] = Load<false>(src + 3 * srcS + 0 * srcC);
            dst[16] = Load<false>(src + 3 * srcS + 1 * srcC);
            dst[17] = Load<false>(src + 3 * srcS + 2 * srcC);
            dst[18] = Load<false>(src + 3 * srcS + 3 * srcC);
            dst[19] = Load<false>(src + 3 * srcS + 4 * srcC);
            dst[20] = Load<false>(src + 4 * srcS + 0 * srcC);
            dst[21] = Load<false>(src + 4 * srcS + 1 * srcC);
            dst[22] = Load<false>(src + 4 * srcS + 2 * srcC);
            dst[23] = Load<false>(src + 4 * srcS + 3 * srcC);
            dst[24] = Load<false>(src + 4 * srcS + 4 * srcC);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetInput4t(const float* src, size_t srcW, size_t srcC, float* dst, size_t dstStride)
        {
            size_t srcS = srcW * srcC;
            size_t srcCF = AlignLo(srcC, F);
            for (size_t c = 0; c < srcCF; c += F)
            {
                float32x4_t tmp[25];
                WinogradKernel2x2Block4x4SetInput4t(src + c, srcS, srcC, tmp);
                WinogradKernel2x2Block4x4SetInputStore(tmp, dst + c, dstStride);
            }
            if (srcCF < srcC)
            {
                float32x4_t tmp[25];
                WinogradKernel2x2Block4x4SetInput4t(src + srcC - F, srcS, srcC, tmp);
                WinogradKernel2x2Block4x4SetInputStore(tmp, dst + srcC - F, dstStride);
            }
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetInput4t(const float* src, size_t srcS, size_t srcC, size_t rowB, size_t rowE, size_t colB, size_t colE, float32x4_t dst[25])
        {
            for (size_t i = 0; i < 25; ++i)
                dst[i] = vdupq_n_f32(0.0f);
            for (size_t row = rowB; row < rowE; ++row)
                for (size_t col = colB; col < colE; ++col)
                    dst[row * 5 + col] = Load<false>(src + row * srcS + col * srcC);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetInput4t(const float* src, size_t srcW, size_t srcC, size_t rowB, size_t rowE, size_t colB, size_t colE, float* dst, size_t dstStride)
        {
            size_t srcS = srcW * srcC;
            size_t srcCF = AlignLo(srcC, F);
            for (size_t c = 0; c < srcCF; c += F)
            {
                float32x4_t tmp[25];
                WinogradKernel2x2Block4x4SetInput4t(src + c, srcS, srcC, rowB, rowE, colB, colE, tmp);
                WinogradKernel2x2Block4x4SetInputStore(tmp, dst + c, dstStride);
            }
            if (srcCF < srcC)
            {
                float32x4_t tmp[25];
                WinogradKernel2x2Block4x4SetInput4t(src + srcC - F, srcS, srcC, rowB, rowE, colB, colE, tmp);
                WinogradKernel2x2Block4x4SetInputStore(tmp, dst + srcC - F, dstStride);
            }
        }

        void WinogradKernel2x2Block4x4SetInput(const float* src, size_t srcChannels, size_t srcHeight, size_t srcWidth,
            size_t padY, size_t padX, size_t padH, size_t padW, float* dst, size_t dstStride, SimdBool trans)
        {
            assert(padY == padX && padW == padH && (padY + padH == 0 || padY + padH == 1));
            if (trans ? (srcChannels < F) : true)
            {
                Base::WinogradKernel2x2Block4x4SetInput(src, srcChannels, srcHeight, srcWidth, padY, padX, padH, padW, dst, dstStride, trans);
                return;
            }
            size_t dstH = srcHeight - 1 + padY + padH;
            size_t dstW = srcWidth - 1 + padX + padW;
            size_t dstH4 = AlignLo(dstH, 4);
            size_t dstW4 = AlignLo(dstW, 4);
            size_t noseW = Simd::Min<size_t>(5, dstW + 1);
            size_t noseH = Simd::Min<size_t>(5, dstH + 1);
            size_t startY = padY ? 4 : 0;
            size_t startX = padX ? 4 : 0;
            if (padY || padH)
            {
                if (dstH == dstH4)
                    dstH4 -= 4;
                if (dstW == dstW4)
                    dstW4 -= 4;
                if (padY)
                    src -= (srcWidth + 1) * (trans ? srcChannels : 1);
            }
            size_t tailW = dstW - dstW4 + (padW ? 0 : 1);
            size_t tailH = dstH - dstH4 + (padH ? 0 : 1);
            size_t row = 0, col = 0;
            if (padY)
            {
                if (padX)
                    WinogradKernel2x2Block4x4SetInput4t(src, srcWidth, srcChannels, 1, noseH, 1, noseW, dst, dstStride), dst += srcChannels;
                for (col = startX; col < dstW4; col += 4)
                    WinogradKernel2x2Block4x4SetInput4t(src + col * srcChannels, srcWidth, srcChannels, 1, noseH, 0, 5, dst, dstStride), dst += srcChannels;
                if (col < dstW)
                    WinogradKernel2x2Block4x4SetInput4t(src + col * srcChannels, srcWidth, srcChannels, 1, noseH, 0, tailW, dst, dstStride), dst += srcChannels;
            }
            for (row = startY; row < dstH4; row += 4)
            {
                if (padX)
                    WinogradKernel2x2Block4x4SetInput4t(src + row * srcWidth * srcChannels, srcWidth, srcChannels, 0, 5, 1, noseW, dst, dstStride), dst += srcChannels;
                for (col = startX; col < dstW4; col += 4)
                    WinogradKernel2x2Block4x4SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, dst, dstStride), dst += srcChannels;
                if (col < dstW)
                    WinogradKernel2x2Block4x4SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, 0, 5, 0, tailW, dst, dstStride), dst += srcChannels;
            }
            if (row < dstH)
            {
                if (padX)
                    WinogradKernel2x2Block4x4SetInput4t(src + row * srcWidth * srcChannels, srcWidth, srcChannels, 0, tailH, 1, noseW, dst, dstStride), dst += srcChannels;
                for (col = startX; col < dstW4; col += 4)
                    WinogradKernel2x2Block4x4SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, 0, tailH, 0, 5, dst, dstStride), dst += srcChannels;
                if (col < dstW)
                    WinogradKernel2x2Block4x4SetInput4t(src + (row * srcWidth + col) * srcChannels, srcWidth, srcChannels, 0, tailH, 0, tailW, dst, dstStride), dst += srcChannels;
            }
        }

        //-----------------------------------------------------------------------

        SIMD_INLINE void WinogradKernel2x2Block4x4SetOutputGetRow(const float32x4_t* s, float32x4_t* d)
        {
            const float32x4_t _2 = vdupq_n_f32(2.0f);
            const float32x4_t _4 = vdupq_n_f32(4.0f);
            const float32x4_t _8 = vdupq_n_f32(8.0f);
            d[0] = vaddq_f32(vaddq_f32(s[0], s[1]), vaddq_f32(s[2], s[3]));
            d[1] = vaddq_f32(vsubq_f32(s[1], s[2]), vmulq_f32(_2, s[3]));
            d[2] = vaddq_f32(vaddq_f32(s[1], s[2]), vmulq_f32(_4, s[3]));
            d[3] = vaddq_f32(vsubq_f32(s[1], s[2]), vaddq_f32(vmulq_f32(_8, s[3]), s[4]));
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetOutputLoad25(const float* src, size_t stride, float32x4_t* dst)
        {
            float32x4_t s[25];
            s[0] = Load<false>(src + 0 * stride);
            s[1] = Load<false>(src + 1 * stride);
            s[2] = Load<false>(src + 2 * stride);
            s[3] = Load<false>(src + 3 * stride);
            s[4] = Load<false>(src + 4 * stride);
            s[5] = Load<false>(src + 5 * stride);
            s[6] = Load<false>(src + 6 * stride);
            s[7] = Load<false>(src + 7 * stride);
            s[8] = Load<false>(src + 8 * stride);
            s[9] = Load<false>(src + 9 * stride);
            s[10] = Load<false>(src + 10 * stride);
            s[11] = Load<false>(src + 11 * stride);
            s[12] = Load<false>(src + 12 * stride);
            s[13] = Load<false>(src + 13 * stride);
            s[14] = Load<false>(src + 14 * stride);
            s[15] = Load<false>(src + 15 * stride);
            s[16] = Load<false>(src + 16 * stride);
            s[17] = Load<false>(src + 17 * stride);
            s[18] = Load<false>(src + 18 * stride);
            s[19] = Load<false>(src + 19 * stride);
            s[20] = Load<false>(src + 20 * stride);
            s[21] = Load<false>(src + 21 * stride);
            s[22] = Load<false>(src + 22 * stride);
            s[23] = Load<false>(src + 23 * stride);
            s[24] = Load<false>(src + 24 * stride);

            const float32x4_t _2 = vdupq_n_f32(2.0f);
            const float32x4_t _4 = vdupq_n_f32(4.0f);
            const float32x4_t _8 = vdupq_n_f32(8.0f);
            float32x4_t t[5];
            t[0] = vaddq_f32(vaddq_f32(s[0], s[5]), vaddq_f32(s[10], s[15]));
            t[1] = vaddq_f32(vaddq_f32(s[1], s[6]), vaddq_f32(s[11], s[16]));
            t[2] = vaddq_f32(vaddq_f32(s[2], s[7]), vaddq_f32(s[12], s[17]));
            t[3] = vaddq_f32(vaddq_f32(s[3], s[8]), vaddq_f32(s[13], s[18]));
            t[4] = vaddq_f32(vaddq_f32(s[4], s[9]), vaddq_f32(s[14], s[19]));
            WinogradKernel2x2Block4x4SetOutputGetRow(t, dst + 0);

            t[0] = vaddq_f32(vsubq_f32(s[5], s[10]), vmulq_f32(_2, s[15]));
            t[1] = vaddq_f32(vsubq_f32(s[6], s[11]), vmulq_f32(_2, s[16]));
            t[2] = vaddq_f32(vsubq_f32(s[7], s[12]), vmulq_f32(_2, s[17]));
            t[3] = vaddq_f32(vsubq_f32(s[8], s[13]), vmulq_f32(_2, s[18]));
            t[4] = vaddq_f32(vsubq_f32(s[9], s[14]), vmulq_f32(_2, s[19]));
            WinogradKernel2x2Block4x4SetOutputGetRow(t, dst + 4);

            t[0] = vaddq_f32(vaddq_f32(s[5], s[10]), vmulq_f32(_4, s[15]));
            t[1] = vaddq_f32(vaddq_f32(s[6], s[11]), vmulq_f32(_4, s[16]));
            t[2] = vaddq_f32(vaddq_f32(s[7], s[12]), vmulq_f32(_4, s[17]));
            t[3] = vaddq_f32(vaddq_f32(s[8], s[13]), vmulq_f32(_4, s[18]));
            t[4] = vaddq_f32(vaddq_f32(s[9], s[14]), vmulq_f32(_4, s[19]));
            WinogradKernel2x2Block4x4SetOutputGetRow(t, dst + 8);

            t[0] = vaddq_f32(vsubq_f32(s[5], s[10]), vaddq_f32(vmulq_f32(_8, s[15]), s[20]));
            t[1] = vaddq_f32(vsubq_f32(s[6], s[11]), vaddq_f32(vmulq_f32(_8, s[16]), s[21]));
            t[2] = vaddq_f32(vsubq_f32(s[7], s[12]), vaddq_f32(vmulq_f32(_8, s[17]), s[22]));
            t[3] = vaddq_f32(vsubq_f32(s[8], s[13]), vaddq_f32(vmulq_f32(_8, s[18]), s[23]));
            t[4] = vaddq_f32(vsubq_f32(s[9], s[14]), vaddq_f32(vmulq_f32(_8, s[19]), s[24]));
            WinogradKernel2x2Block4x4SetOutputGetRow(t, dst + 12);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetOutputStore16(const float32x4_t src[16], float* dst, size_t dstS, size_t dstC)
        {
            Store<false>(dst + 0 * dstS + 0 * dstC, src[0]);
            Store<false>(dst + 0 * dstS + 1 * dstC, src[1]);
            Store<false>(dst + 0 * dstS + 2 * dstC, src[2]);
            Store<false>(dst + 0 * dstS + 3 * dstC, src[3]);
            Store<false>(dst + 1 * dstS + 0 * dstC, src[4]);
            Store<false>(dst + 1 * dstS + 1 * dstC, src[5]);
            Store<false>(dst + 1 * dstS + 2 * dstC, src[6]);
            Store<false>(dst + 1 * dstS + 3 * dstC, src[7]);
            Store<false>(dst + 2 * dstS + 0 * dstC, src[8]);
            Store<false>(dst + 2 * dstS + 1 * dstC, src[9]);
            Store<false>(dst + 2 * dstS + 2 * dstC, src[10]);
            Store<false>(dst + 2 * dstS + 3 * dstC, src[11]);
            Store<false>(dst + 3 * dstS + 0 * dstC, src[12]);
            Store<false>(dst + 3 * dstS + 1 * dstC, src[13]);
            Store<false>(dst + 3 * dstS + 2 * dstC, src[14]);
            Store<false>(dst + 3 * dstS + 3 * dstC, src[15]);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetOutput4t(const float* src, size_t srcStride, float* dst, size_t dstW, size_t dstC)
        {
            size_t dstS = dstW * dstC, dstCF = AlignLo(dstC, F);
            for (size_t d = 0; d < dstCF; d += F)
            {
                float32x4_t tmp[16];
                WinogradKernel2x2Block4x4SetOutputLoad25(src + d, srcStride, tmp);
                WinogradKernel2x2Block4x4SetOutputStore16(tmp, dst + d, dstS, dstC);
            }
            if (dstCF < dstC)
            {
                float32x4_t tmp[16];
                WinogradKernel2x2Block4x4SetOutputLoad25(src + dstC - F, srcStride, tmp);
                WinogradKernel2x2Block4x4SetOutputStore16(tmp, dst + dstC - F, dstS, dstC);
            }
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetOutputStore16(const float32x4_t src[16], float* dst, size_t dstS, size_t dstC, size_t rowE, size_t colE)
        {
            for (size_t row = 0; row < rowE; ++row)
                for (size_t col = 0; col < colE; ++col)
                    Store<false>(dst + row * dstS + col * dstC, src[row * 4 + col]);
        }

        SIMD_INLINE void WinogradKernel2x2Block4x4SetOutput4t(const float* src, size_t srcStride, float* dst, size_t dstW, size_t dstC, size_t rowE, size_t colE)
        {
            size_t dstS = dstW * dstC, dstCF = AlignLo(dstC, F);
            for (size_t d = 0; d < dstCF; d += F)
            {
                float32x4_t tmp[16];
                WinogradKernel2x2Block4x4SetOutputLoad25(src + d, srcStride, tmp);
                WinogradKernel2x2Block4x4SetOutputStore16(tmp, dst + d, dstS, dstC, rowE, colE);
            }
            if (dstCF < dstC)
            {
                float32x4_t tmp[16];
                WinogradKernel2x2Block4x4SetOutputLoad25(src + dstC - F, srcStride, tmp);
                WinogradKernel2x2Block4x4SetOutputStore16(tmp, dst + dstC - F, dstS, dstC, rowE, colE);
            }
        }

        void WinogradKernel2x2Block4x4SetOutput(const float* src, size_t srcStride, float* dst, size_t dstChannels, size_t dstHeight, size_t dstWidth, SimdBool trans)
        {
            if (trans ? (dstChannels < F) : true)
            {
                Base::WinogradKernel2x2Block4x4SetOutput(src, srcStride, dst, dstChannels, dstHeight, dstWidth, trans);
                return;
            }
            size_t tileH = (dstHeight + 3) / 4;
            size_t tileW = (dstWidth + 3) / 4;
            size_t dstH4 = AlignLo(dstHeight, 4);
            size_t dstW4 = AlignLo(dstWidth, 4);
            size_t row, col;
            for (row = 0; row < dstH4; row += 4)
            {
                for (col = 0; col < dstW4; col += 4)
                    WinogradKernel2x2Block4x4SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels), src += dstChannels;
                if (col < dstWidth)
                    WinogradKernel2x2Block4x4SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels, 4, dstWidth - col), src += dstChannels;
            }
            if (row < dstHeight)
            {
                for (col = 0; col < dstW4; col += 4)
                    WinogradKernel2x2Block4x4SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels, dstHeight - row, 4), src += dstChannels;
                if (col < dstWidth)
                    WinogradKernel2x2Block4x4SetOutput4t(src, srcStride, dst + (row * dstWidth + col) * dstChannels, dstWidth, dstChannels, dstHeight - row, dstWidth - col), src += dstChannels;
            }
        }
    }
#endif// SIMD_NEON_ENABLE
}
