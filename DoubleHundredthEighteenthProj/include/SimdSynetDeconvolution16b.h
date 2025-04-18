/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2025 Yermalayeu Ihar.
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
#ifndef __SimdSynetDeconvolution16b_h__
#define __SimdSynetDeconvolution16b_h__

#include "SimdArray.h"
#include "SimdPerformance.h"
#include "SimdRuntime.h"
#include "SimdGemm.h"
#include "SimdSynetConvParam.h"

namespace Simd
{
    class SynetDeconvolution16b : public Deletable
    {
    public:
        SynetDeconvolution16b(const DeconvParam& p);

        const DeconvParam& Param() const
        {
            return _param;
        }

        virtual String Ext() const = 0;
        virtual String Desc() const = 0;

        virtual size_t ExternalBufferSize() const
        {
            return 1;
        }

        virtual size_t InternalBufferSize() const
        {
            return _buffer.RawSize() + _weight.RawSize() +
                _bias.RawSize() + _params.RawSize();
        }

        virtual void SetParams(const float* weight, const float* bias, const float* params) = 0;

        virtual void Forward(const uint8_t* src, uint8_t* buf, uint8_t* dst) = 0;

        uint8_t* Buffer(uint8_t* buffer)
        {
            if (buffer)
                return buffer;
            else
            {
                _buffer.Resize(ExternalBufferSize());
                return _buffer.data;
            }
        }

#if defined(SIMD_PERFORMANCE_STATISTIC) && (defined(NDEBUG) || defined(SIMD_PERF_STAT_IN_DEBUG))
        Base::PerformanceMeasurer* Perf(const char* func);
#endif

        const char* Info() const
        {
            _info = Desc();
            return _info.c_str();
        }

    protected:

        DeconvParam _param;
        Array8u _buffer;
#if defined(SIMD_PERFORMANCE_STATISTIC) && (defined(NDEBUG) || defined(SIMD_PERF_STAT_IN_DEBUG))
        Base::PerformanceMeasurer* _perf;
#endif
        mutable String _info;
        Array16u _weight;
        Array32f _bias, _params;
        bool _src16b, _dst16b, _is1x1;
        size_t _elemS, _elemD, _stepS, _stepD;

        void SetBias(const float* bias, size_t align);
        void SetParams(const float* params, size_t align);
    };

    //-------------------------------------------------------------------------------------------------

    namespace Base
    {
        class SynetDeconvolution16bGemm : public SynetDeconvolution16b
        {
        public:
            SynetDeconvolution16bGemm(const DeconvParam& p);
            virtual String Ext() const { return "Base"; }
            virtual String Desc() const { return Ext() + "::Gemm"; }
            virtual size_t ExternalBufferSize() const;
            virtual void SetParams(const float* weight, const float* bias, const float* params);
            virtual void Forward(const uint8_t* src, uint8_t* buf, uint8_t* dst);

        protected:
            void ColToImg(const float* src, float* dst);
            void RowToImg(const float* src, float* dst);

            void GemmNN(size_t M, size_t N, size_t K, const uint16_t* A, size_t lda, const uint16_t* B, size_t ldb, float* C, size_t ldc);

            size_t _M, _N, _K, _ldW, _ldS, _ldD, _grW, _grS, _grD, _batch, _sizeS, _sizeB, _sizeD;
        };

        class SynetDeconvolution16bNhwcGemm : public SynetDeconvolution16b
        {
        public:
            SynetDeconvolution16bNhwcGemm(const DeconvParam& p);
            virtual String Ext() const { return "Base"; }
            virtual String Desc() const { return Ext() + "::NhwcGemm"; }
            virtual size_t ExternalBufferSize() const;
            virtual void SetParams(const float* weight, const float* bias, const float* params);
            virtual void Forward(const uint8_t* src, uint8_t* buf, uint8_t* dst);

            struct AlgParam
            {
                size_t M, N, K;
                size_t F, microM, microN, microK;
                size_t macroH, macroM, macroN, macroK;
                size_t bufM, bufN, bufK, elem, preH;
            };

            static bool Preferable(const DeconvParam& p);

            typedef void(*ConvertPtr)(const uint8_t* src, const DeconvParam& p, const AlgParam& a, size_t yBeg, size_t yEnd, uint16_t* dst);
            typedef void(*GemmPtr)(const uint16_t* src, const DeconvParam& p, const AlgParam& a, size_t M, size_t N, size_t K, int zero, const uint16_t* wgt, float* dst);
            typedef void(*ToImgPtr)(const float* src, const DeconvParam& p, const AlgParam& a, size_t dstC, size_t yBeg, size_t yEnd, float* dst);
            typedef void(*BiasActPtr)(const float* src, const DeconvParam& p, const AlgParam& a, size_t dstC, size_t yBeg, size_t yEnd, const float* bias, const float* params, uint8_t* dst);

        protected:
            void SetAlgParam(size_t F, size_t microN, size_t microM, size_t microK, size_t L1, size_t L2, size_t L3);
            void ForwardCommon(const uint8_t* src, uint16_t* bufS, float* bufB, float* bufD, uint8_t* dst);
            void GemmCommon(const uint16_t* src, float* dst);
            void ForwardSmallNK(const uint8_t* src, uint16_t* bufS, float* bufB, float* bufD, uint8_t* dst);

            AlgParam _alg;
            ConvertPtr _convert;
            GemmPtr _gemm;
            ToImgPtr _toImg;
            BiasActPtr _biasAct;
        };

        //-------------------------------------------------------------------------------------------------

        void * SynetDeconvolution16bInit(size_t batch, const SimdConvolutionParameters * conv, SimdSynetCompatibilityType compatibility);
    }

#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {
        class SynetDeconvolution16bNhwcGemm : public Base::SynetDeconvolution16bNhwcGemm
        {
        public:
            SynetDeconvolution16bNhwcGemm(const DeconvParam& p);
            virtual String Ext() const { return "Sse41"; }

            static bool Preferable(const DeconvParam& p);
        };

        //-------------------------------------------------------------------------------------------------

        void* SynetDeconvolution16bInit(size_t batch, const SimdConvolutionParameters* conv, SimdSynetCompatibilityType compatibility);
    }
#endif

#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {
        class SynetDeconvolution16bNhwcGemm : public Sse41::SynetDeconvolution16bNhwcGemm
        {
        public:
            SynetDeconvolution16bNhwcGemm(const DeconvParam& p);
            virtual String Ext() const { return "Avx2"; }
        };

        //-------------------------------------------------------------------------------------------------

        void* SynetDeconvolution16bInit(size_t batch, const SimdConvolutionParameters* conv, SimdSynetCompatibilityType compatibility);
    }
#endif

#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {
        class SynetDeconvolution16bNhwcGemm : public Avx2::SynetDeconvolution16bNhwcGemm
        {
        public:
            SynetDeconvolution16bNhwcGemm(const DeconvParam& p);
            virtual String Ext() const { return "Avx512bw"; }
        };

        //-------------------------------------------------------------------------------------------------

        void* SynetDeconvolution16bInit(size_t batch, const SimdConvolutionParameters* conv, SimdSynetCompatibilityType compatibility);
    }
#endif

#if (defined(SIMD_AMXBF16_ENABLE) || (defined(SIMD_AVX512BW_ENABLE) && defined(SIMD_AMX_EMULATE)))
    namespace AmxBf16
    {
        class SynetDeconvolution16bNhwcGemm : public Avx512bw::SynetDeconvolution16bNhwcGemm
        {
        public:
            SynetDeconvolution16bNhwcGemm(const DeconvParam& p);
            virtual String Ext() const { return "AmxBf16"; }
        };

        //-------------------------------------------------------------------------------------------------

        void* SynetDeconvolution16bInit(size_t batch, const SimdConvolutionParameters * conv, SimdSynetCompatibilityType compatibility);
    }
#endif
}

#endif
