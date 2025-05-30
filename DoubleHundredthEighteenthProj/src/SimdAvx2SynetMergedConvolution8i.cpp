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
#include "SimdSynetMergedConvolution8i.h"
#include "SimdSynetConvolution8iCommon.h"
#include "SimdSynet.h"
#include "SimdMath.h"
#include "SimdBase.h"
#include "SimdAvx2.h"
#include "SimdCpu.h"

namespace Simd
{
#if defined(SIMD_AVX2_ENABLE) && defined(SIMD_SYNET_ENABLE) 
    namespace Avx2
    {
        using Convert8uTo32fPtr = Base::SynetMergedConvolution8i::Convert8uTo32fPtr;
        using Convert32fTo8uPtr = Base::SynetMergedConvolution8i::Convert32fTo8uPtr;

        //---------------------------------------------------------------------

        template<bool nofma> SIMD_INLINE void Cvt8uTo32f(const uint8_t* src, const float* scale, const float* shift, float * dst)
        {
            __m256 f32 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_loadl_epi64((__m128i*)(src))));
            _mm256_storeu_ps(dst, Fmadd<nofma>(f32, _mm256_loadu_ps(scale), _mm256_loadu_ps(shift)));
        }

        template<bool nofma> void Convert8uTo32f(const uint8_t* src, size_t maC, size_t yBeg, size_t yEnd, size_t width, size_t channels,
            const float* scale, const float* shift, float* dst, size_t bufH)
        {
            size_t dM = bufH - 1, cD = width* bufH;
            src += yBeg * width * channels;
            for (size_t y = yBeg; y < yEnd; ++y)
            {
                float* pd = dst + (y & dM) * width * F;
                for (size_t x = 0; x < width; ++x)
                {
                    for (size_t c = 0; c < maC; c += F)
                        Cvt8uTo32f<nofma>(src + c, scale + c, shift + c, pd + c * cD);
                    src += channels;
                    pd += F;
                }
            }
        }

        void Convert8uTo32f(const uint8_t* src, size_t maC, size_t yBeg, size_t yEnd, size_t width, size_t channels,
            const float* scale, const float* shift, float* dst, size_t bufH, SimdSynetCompatibilityType compatibility)
        {
            if (Base::FmaAvoid(compatibility))
                Convert8uTo32f<true>(src, maC, yBeg, yEnd, width, channels, scale, shift, dst, bufH);
            else
                Convert8uTo32f<false>(src, maC, yBeg, yEnd, width, channels, scale, shift, dst, bufH);
        }

        //---------------------------------------------------------------------

        void Convert32fTo8u(const float* src, size_t yBeg, size_t yEnd, size_t width, size_t channels,
            const float* scale, const float* shift, uint8_t* dst, size_t bufH, SimdSynetCompatibilityType compatibility)
        {
            size_t size = width * channels, mask = bufH - 1;
            size_t yInt = Simd::Max(yBeg, AlignLo(yEnd, bufH));
            if (yInt > yBeg)
                Avx2::SynetConvert32fTo8u(src + yBeg * size, 1, channels, yInt - yBeg, width, SimdTensorFormatNhwc, scale, shift, dst + (yBeg & mask) * size, compatibility);
            if (yEnd > yInt)
                Avx2::SynetConvert32fTo8u(src + yInt * size, 1, channels, yEnd - yInt, width, SimdTensorFormatNhwc, scale, shift, dst + (yInt & mask) * size, compatibility);
        }

        //---------------------------------------------------------------------

        SynetMergedConvolution8iCdc::SynetMergedConvolution8iCdc(const MergConvParam8i& p)
            : Sse41::SynetMergedConvolution8iCdc(p)
        {
            SetSize(F);
            _cvt32fTo8u = _s8u ? NULL : Convert32fTo8u;
            SetInput(_param.conv[0], _input);
            SetDepthwise(_param.conv[1], _depthwise);
            SetOutput(_param.conv[2], _output);
        }

        //---------------------------------------------------------------------

        SynetMergedConvolution8iCd::SynetMergedConvolution8iCd(const MergConvParam8i& p)
            : Sse41::SynetMergedConvolution8iCd(p)
        {
            SetSize(F);
            _cvt32fTo8u = _s8u ? NULL : Convert32fTo8u;
            SetInput(_param.conv[0], _input);
            SetDepthwise(_param.conv[1], _depthwise);
        }

        //---------------------------------------------------------------------

        SynetMergedConvolution8iDc::SynetMergedConvolution8iDc(const MergConvParam8i& p)
            : Sse41::SynetMergedConvolution8iDc(p)
        {
            SetSize(F);
            _cvt8uTo32f = _s8u ? (Convert8uTo32fPtr)Convert8uTo32f : NULL;
            SetDepthwise(_param.conv[0], _depthwise);
            SetOutput(_param.conv[1], _output);
        }

        //---------------------------------------------------------------------

        void* SynetMergedConvolution8iInit(size_t batch, const SimdConvolutionParameters* convs, size_t count, SimdSynetCompatibilityType compatibility)
        {
            MergConvParam8i param(batch, convs, count, compatibility);
            if (!param.Valid())
                return NULL;
            if (SynetMergedConvolution8iCdc::Preferable(param))
                return new Avx2::SynetMergedConvolution8iCdc(param);
            else if (SynetMergedConvolution8iCd::Preferable(param))
                return new Avx2::SynetMergedConvolution8iCd(param);
            else if (SynetMergedConvolution8iDc::Preferable(param))
                return new Avx2::SynetMergedConvolution8iDc(param);
            else
                return new Base::SynetMergedConvolution8i(param);
        }
    }
#endif
}
