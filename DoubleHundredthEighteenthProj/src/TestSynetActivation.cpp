/*
* Tests for Simd Library (http://ermig1979.github.io/Simd).
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
#include "TestCompare.h"
#include "TestPerformance.h"
#include "TestTensor.h"
#include "TestRandom.h"

namespace Test
{
#if defined(SIMD_SYNET_ENABLE)
    namespace
    {
        struct FuncElu32f
        {
            typedef void(*FuncPtr)(const float * src, size_t size, const float * alpha, float * dst);

            FuncPtr func;
            String desc;

            FuncElu32f(const FuncPtr & f, const String & d) : func(f), desc(d) {}

            void Call(const Tensor32f & src, const float & alpha, Tensor32f & dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func(src.Data(), src.Size(), &alpha, dst.Data());
            }
        };
    }

#define FUNC_ELU32F(func) FuncElu32f(func, #func)

    bool SynetElu32fAutoTest(size_t size, const FuncElu32f & f1, const FuncElu32f & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        Tensor32f src(ToShape(size));
        Tensor32f dst1(ToShape(size));
        Tensor32f dst2(ToShape(size));

        FillRandom(src, -10.0, 10.0);
        float alpha = 1.1f;

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, alpha, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, alpha, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, DifferenceBoth);

        return result;
    }

    bool SynetElu32fAutoTest(const FuncElu32f & f1, const FuncElu32f & f2)
    {
        bool result = true;

        result = result && SynetElu32fAutoTest(W * H, f1, f2);
        result = result && SynetElu32fAutoTest(W * H - O , f1, f2);

        return result;
    }

    bool SynetElu32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetElu32fAutoTest(FUNC_ELU32F(Simd::Base::SynetElu32f), FUNC_ELU32F(SimdSynetElu32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetElu32fAutoTest(FUNC_ELU32F(Simd::Sse41::SynetElu32f), FUNC_ELU32F(SimdSynetElu32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetElu32fAutoTest(FUNC_ELU32F(Simd::Avx2::SynetElu32f), FUNC_ELU32F(SimdSynetElu32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetElu32fAutoTest(FUNC_ELU32F(Simd::Avx512bw::SynetElu32f), FUNC_ELU32F(SimdSynetElu32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetElu32fAutoTest(FUNC_ELU32F(Simd::Neon::SynetElu32f), FUNC_ELU32F(SimdSynetElu32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncGelu32f
        {
            typedef void(*FuncPtr)(const float* src, size_t size, float* dst);

            FuncPtr func;
            String desc;

            FuncGelu32f(const FuncPtr& f, const String& d) : func(f), desc(d) {}

            void Call(const Tensor32f& src, Tensor32f& dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func(src.Data(), src.Size(), dst.Data());
            }
        };
    }

#define FUNC_GELU32F(func) FuncGelu32f(func, #func)

    bool SynetGelu32fAutoTest(size_t size, const FuncGelu32f& f1, const FuncGelu32f& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        Tensor32f src(ToShape(size));
        Tensor32f dst1(ToShape(size));
        Tensor32f dst2(ToShape(size));

        FillRandom(src, -10.0, 10.0);

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, DifferenceBoth);

        return result;
    }

    bool SynetGelu32fAutoTest(const FuncGelu32f& f1, const FuncGelu32f& f2)
    {
        bool result = true;

        result = result && SynetGelu32fAutoTest(W * H, f1, f2);
        result = result && SynetGelu32fAutoTest(W * H - O, f1, f2);

        return result;
    }

    bool SynetGelu32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetGelu32fAutoTest(FUNC_GELU32F(Simd::Base::SynetGelu32f), FUNC_GELU32F(SimdSynetGelu32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetGelu32fAutoTest(FUNC_GELU32F(Simd::Sse41::SynetGelu32f), FUNC_GELU32F(SimdSynetGelu32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetGelu32fAutoTest(FUNC_GELU32F(Simd::Avx2::SynetGelu32f), FUNC_GELU32F(SimdSynetGelu32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetGelu32fAutoTest(FUNC_GELU32F(Simd::Avx512bw::SynetGelu32f), FUNC_GELU32F(SimdSynetGelu32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetGelu32fAutoTest(FUNC_GELU32F(Simd::Neon::SynetGelu32f), FUNC_GELU32F(SimdSynetGelu32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncHardSigmoid32f
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* scale, const float* shift, float* dst);

            FuncPtr func;
            String desc;

            FuncHardSigmoid32f(const FuncPtr& f, const String& d) : func(f), desc(d) {}

            void Call(const Tensor32f& src, const float& scale, const float& shift, Tensor32f& dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func(src.Data(), src.Size(), &scale, &shift, dst.Data());
            }
        };
    }

#define FUNC_HARDSIGMOID32F(func) FuncHardSigmoid32f(func, #func)

    bool SynetHardSigmoid32fAutoTest(size_t size, const FuncHardSigmoid32f& f1, const FuncHardSigmoid32f& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        Tensor32f src(ToShape(size));
        Tensor32f dst1(ToShape(size));
        Tensor32f dst2(ToShape(size));

        FillRandom(src, -10.0, 10.0);
        float scale = 1.0f / 6.0f;
        float shift = 1.0f / 2.0f;

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, scale, shift, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, scale, shift, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, DifferenceBoth);

        return result;
    }

    bool SynetHardSigmoid32fAutoTest(const FuncHardSigmoid32f& f1, const FuncHardSigmoid32f& f2)
    {
        bool result = true;

        result = result && SynetHardSigmoid32fAutoTest(W * H, f1, f2);
        result = result && SynetHardSigmoid32fAutoTest(W * H - O, f1, f2);

        return result;
    }

    bool SynetHardSigmoid32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetHardSigmoid32fAutoTest(FUNC_HARDSIGMOID32F(Simd::Base::SynetHardSigmoid32f), FUNC_HARDSIGMOID32F(SimdSynetHardSigmoid32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetHardSigmoid32fAutoTest(FUNC_HARDSIGMOID32F(Simd::Sse41::SynetHardSigmoid32f), FUNC_HARDSIGMOID32F(SimdSynetHardSigmoid32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetHardSigmoid32fAutoTest(FUNC_HARDSIGMOID32F(Simd::Avx2::SynetHardSigmoid32f), FUNC_HARDSIGMOID32F(SimdSynetHardSigmoid32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetHardSigmoid32fAutoTest(FUNC_HARDSIGMOID32F(Simd::Avx512bw::SynetHardSigmoid32f), FUNC_HARDSIGMOID32F(SimdSynetHardSigmoid32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetHardSigmoid32fAutoTest(FUNC_HARDSIGMOID32F(Simd::Neon::SynetHardSigmoid32f), FUNC_HARDSIGMOID32F(SimdSynetHardSigmoid32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncHswish32f
        {
            typedef void(*FuncPtr)(const float * src, size_t size, const float * shift, const float * scale, float * dst);

            FuncPtr func;
            String desc;

            FuncHswish32f(const FuncPtr & f, const String & d) : func(f), desc(d) {}

            void Call(const Tensor32f & src, const float & shift, const float & scale, Tensor32f & dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func(src.Data(), src.Size(), &shift, &scale, dst.Data());
            }
        };
    }

#define FUNC_HSWISH32F(func) FuncHswish32f(func, #func)

    bool SynetHswish32fAutoTest(size_t size, const FuncHswish32f & f1, const FuncHswish32f & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        Tensor32f src(ToShape(size));
        Tensor32f dst1(ToShape(size));
        Tensor32f dst2(ToShape(size));

        FillRandom(src, -10.0, 10.0);
        float shift = 3.0f;
        float scale = 1.0f / 6.0f;

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, shift, scale, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, shift, scale, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, DifferenceBoth);

        return result;
    }

    bool SynetHswish32fAutoTest(const FuncHswish32f & f1, const FuncHswish32f & f2)
    {
        bool result = true;

        result = result && SynetHswish32fAutoTest(W * H, f1, f2);
        result = result && SynetHswish32fAutoTest(W * H - O, f1, f2);

        return result;
    }

    bool SynetHswish32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetHswish32fAutoTest(FUNC_HSWISH32F(Simd::Base::SynetHswish32f), FUNC_HSWISH32F(SimdSynetHswish32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetHswish32fAutoTest(FUNC_HSWISH32F(Simd::Sse41::SynetHswish32f), FUNC_HSWISH32F(SimdSynetHswish32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetHswish32fAutoTest(FUNC_HSWISH32F(Simd::Avx2::SynetHswish32f), FUNC_HSWISH32F(SimdSynetHswish32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetHswish32fAutoTest(FUNC_HSWISH32F(Simd::Avx512bw::SynetHswish32f), FUNC_HSWISH32F(SimdSynetHswish32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetHswish32fAutoTest(FUNC_HSWISH32F(Simd::Neon::SynetHswish32f), FUNC_HSWISH32F(SimdSynetHswish32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncMish32f
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* threshold, float* dst);

            FuncPtr func;
            String desc;

            FuncMish32f(const FuncPtr& f, const String& d) : func(f), desc(d) {}

            void Call(const Tensor32f& src, const float& threshold, Tensor32f& dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func(src.Data(), src.Size(), &threshold, dst.Data());
            }
        };
    }

#define FUNC_MISH32F(func) FuncMish32f(func, #func)

    bool SynetMish32fAutoTest(size_t size, const FuncMish32f& f1, const FuncMish32f& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        Tensor32f src(ToShape(size));
        Tensor32f dst1(ToShape(size));
        Tensor32f dst2(ToShape(size));

        FillRandom(src, -10.0, 10.0);
        float threshold = 20.0f;

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, threshold, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, threshold, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, DifferenceBoth);

        return result;
    }

    bool SynetMish32fAutoTest(const FuncMish32f& f1, const FuncMish32f& f2)
    {
        bool result = true;

        result = result && SynetMish32fAutoTest(W * H, f1, f2);
        result = result && SynetMish32fAutoTest(W * H - O, f1, f2);

        return result;
    }

    bool SynetMish32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetMish32fAutoTest(FUNC_MISH32F(Simd::Base::SynetMish32f), FUNC_MISH32F(SimdSynetMish32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetMish32fAutoTest(FUNC_MISH32F(Simd::Sse41::SynetMish32f), FUNC_MISH32F(SimdSynetMish32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetMish32fAutoTest(FUNC_MISH32F(Simd::Avx2::SynetMish32f), FUNC_MISH32F(SimdSynetMish32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetMish32fAutoTest(FUNC_MISH32F(Simd::Avx512bw::SynetMish32f), FUNC_MISH32F(SimdSynetMish32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetMish32fAutoTest(FUNC_MISH32F(Simd::Neon::SynetMish32f), FUNC_MISH32F(SimdSynetMish32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncPLF
        {
            typedef void(*FuncPtr)(const float* src, const float* slope, size_t channels, size_t spatial, float* dst, SimdTensorFormatType format);

            FuncPtr func;
            String desc;

            FuncPLF(const FuncPtr& f, const String& d) : func(f), desc(d) {}

            void Update(SimdTensorFormatType format)
            {
                desc = desc + "[" + ToString(format) + "]";
            }

            void Call(const Tensor32f& src, const Tensor32f& slope, size_t channels, size_t spatial, SimdTensorFormatType format, Tensor32f& dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func(src.Data(), slope.Data(), channels, spatial, dst.Data(), format);
            }
        };
    }

#define FUNC_PLF(function) FuncPLF(function, #function)

    bool SynetPreluLayerForwardAutoTest(size_t channels, size_t spatial, SimdTensorFormatType format, FuncPLF f1, FuncPLF f2)
    {
        bool result = true;

        f1.Update(format);
        f2.Update(format);

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << channels << ", " << spatial << "].");

        Tensor32f src(ToShape(channels, spatial, format));
        Tensor32f slope(ToShape(channels, format));
        Tensor32f dst1(ToShape(channels, spatial, format));
        Tensor32f dst2(ToShape(channels, spatial, format));

        FillRandom(src.Data(), slope.Size(), -10.0, 10.0);

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, slope, channels, spatial, format, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, slope, channels, spatial, format, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, DifferenceBoth);

        return result;
    }

    bool SynetPreluLayerForwardAutoTest(const FuncPLF& f1, const FuncPLF& f2)
    {
        bool result = true;

        for (SimdTensorFormatType format = SimdTensorFormatNchw; format <= SimdTensorFormatNhwc && result; format = (SimdTensorFormatType)((int)format + 1))
        {
            result = result && SynetPreluLayerForwardAutoTest(H, W, format, f1, f2);
            result = result && SynetPreluLayerForwardAutoTest(H - O, W + O, format, f1, f2);
        }

        return result;
    }

    bool SynetPreluLayerForwardAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetPreluLayerForwardAutoTest(FUNC_PLF(Simd::Base::SynetPreluLayerForward), FUNC_PLF(SimdSynetPreluLayerForward));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetPreluLayerForwardAutoTest(FUNC_PLF(Simd::Sse41::SynetPreluLayerForward), FUNC_PLF(SimdSynetPreluLayerForward));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetPreluLayerForwardAutoTest(FUNC_PLF(Simd::Avx2::SynetPreluLayerForward), FUNC_PLF(SimdSynetPreluLayerForward));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetPreluLayerForwardAutoTest(FUNC_PLF(Simd::Avx512bw::SynetPreluLayerForward), FUNC_PLF(SimdSynetPreluLayerForward));
#endif

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetPreluLayerForwardAutoTest(FUNC_PLF(Simd::Neon::SynetPreluLayerForward), FUNC_PLF(SimdSynetPreluLayerForward));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncRE32f
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* slope, float* dst);

            FuncPtr func;
            String description;

            FuncRE32f(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, float slope, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func((float*)src.data, src.width, &slope, (float*)dst.data);
            }
        };
    }
#define FUNC_RE32F(function) FuncRE32f(function, #function)

    bool SynetRelu32fAutoTest(int size, const FuncRE32f& f1, const FuncRE32f& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(size));
        const float lo = -10.0f, hi = 10.0f, slope = 1.1f;
        FillRandom32f(src, lo, hi);

        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(size));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(size));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, slope, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, slope, dst2));

        result = Compare(dst1, dst2, EPS, true, 32, false);

        return result;
    }

    bool SynetRelu32fAutoTest(const FuncRE32f& f1, const FuncRE32f& f2)
    {
        bool result = true;

        result = result && SynetRelu32fAutoTest(W * H, f1, f2);
        result = result && SynetRelu32fAutoTest(W * H + O, f1, f2);

        return result;
    }

    bool SynetRelu32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetRelu32fAutoTest(FUNC_RE32F(Simd::Base::SynetRelu32f), FUNC_RE32F(SimdSynetRelu32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetRelu32fAutoTest(FUNC_RE32F(Simd::Sse41::SynetRelu32f), FUNC_RE32F(SimdSynetRelu32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetRelu32fAutoTest(FUNC_RE32F(Simd::Avx2::SynetRelu32f), FUNC_RE32F(SimdSynetRelu32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetRelu32fAutoTest(FUNC_RE32F(Simd::Avx512bw::SynetRelu32f), FUNC_RE32F(SimdSynetRelu32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetRelu32fAutoTest(FUNC_RE32F(Simd::Neon::SynetRelu32f), FUNC_RE32F(SimdSynetRelu32f));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncRE16b
        {
            typedef void(*FuncPtr)(const uint16_t* src, size_t size, const float* slope, uint16_t* dst);

            FuncPtr func;
            String description;

            FuncRE16b(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const Tensor16u& src, float slope, Tensor16u& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.Data(), src.Size(), &slope, dst.Data());
            }
        };
    }
#define FUNC_RE16B(function) FuncRE16b(function, #function)

    bool SynetRelu16bAutoTest(int size, const FuncRE16b& f1, const FuncRE16b& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        const float lo = -10.0f, hi = 10.0f, slope = 1.1f;
        Shape shape = Shp(size);
        Tensor32f src32f(shape), dst32f1(shape), dst32f2(shape);
        FillRandom(src32f.Data(), src32f.Size(), lo, hi);

        Tensor16u src16b(shape), dst16b1(shape), dst16b2(shape);
        SimdFloat32ToBFloat16(src32f.Data(), src32f.Size(), src16b.Data());

        Fill(dst16b1.Data(), dst16b1.Size(), uint16_t(1));
        Fill(dst16b2.Data(), dst16b2.Size(), uint16_t(2));

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src16b, slope, dst16b1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src16b, slope, dst16b2));

        SimdBFloat16ToFloat32(dst16b1.Data(), dst16b1.Size(), dst32f1.Data());
        SimdBFloat16ToFloat32(dst16b2.Data(), dst16b2.Size(), dst32f2.Data());

        float eps = EPS;
        result = result && Compare(dst32f1, dst32f2, eps, true, 64, DifferenceBoth);

        return result;
    }

    bool SynetRelu16bAutoTest(const FuncRE16b& f1, const FuncRE16b& f2)
    {
        bool result = true;

        result = result && SynetRelu16bAutoTest(W * H, f1, f2);
        result = result && SynetRelu16bAutoTest(W * H + O, f1, f2);

        return result;
    }

    bool SynetRelu16bAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetRelu16bAutoTest(FUNC_RE16B(Simd::Base::SynetRelu16b), FUNC_RE16B(SimdSynetRelu16b));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetRelu16bAutoTest(FUNC_RE16B(Simd::Sse41::SynetRelu16b), FUNC_RE16B(SimdSynetRelu16b));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetRelu16bAutoTest(FUNC_RE16B(Simd::Avx2::SynetRelu16b), FUNC_RE16B(SimdSynetRelu16b));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetRelu16bAutoTest(FUNC_RE16B(Simd::Avx512bw::SynetRelu16b), FUNC_RE16B(SimdSynetRelu16b));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncRR
        {
            typedef void(*FuncPtr)(const float * src, size_t size, const float * lower, const float * upper, float * dst);

            FuncPtr func;
            String desc;

            FuncRR(const FuncPtr & f, const String & d) : func(f), desc(d) {}

            void Call(const View & src, float lower, float upper, View & dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func((float*)src.data, src.width, &lower, &upper, (float*)dst.data);
            }
        };
    }

#define FUNC_RR(function) FuncRR(function, #function)

    bool SynetRestrictRange32fAutoTest(size_t size, const FuncRR & f1, const FuncRR & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));

        const float lower = -1.0f, upper = 1.0f;
        FillRandom32f(src, 2.0f*lower, 2.0f*upper);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, lower, upper, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, lower, upper, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, false);

        return result;
    }

    bool SynetRestrictRange32fAutoTest(const FuncRR & f1, const FuncRR & f2)
    {
        bool result = true;

        result = result && SynetRestrictRange32fAutoTest(H*W, f1, f2);
        result = result && SynetRestrictRange32fAutoTest(H*W + O, f1, f2);

        return result;
    }

    bool SynetRestrictRange32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetRestrictRange32fAutoTest(FUNC_RR(Simd::Base::SynetRestrictRange32f), FUNC_RR(SimdSynetRestrictRange32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetRestrictRange32fAutoTest(FUNC_RR(Simd::Sse41::SynetRestrictRange32f), FUNC_RR(SimdSynetRestrictRange32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetRestrictRange32fAutoTest(FUNC_RR(Simd::Avx2::SynetRestrictRange32f), FUNC_RR(SimdSynetRestrictRange32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetRestrictRange32fAutoTest(FUNC_RR(Simd::Avx512bw::SynetRestrictRange32f), FUNC_RR(SimdSynetRestrictRange32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetRestrictRange32fAutoTest(FUNC_RR(Simd::Neon::SynetRestrictRange32f), FUNC_RR(SimdSynetRestrictRange32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncSG
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* slope, float* dst);

            FuncPtr func;
            String description;

            FuncSG(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, float slope, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func((float*)src.data, src.width, &slope, (float*)dst.data);
            }
        };
    }
#define FUNC_SG(function) FuncSG(function, #function)

    bool SynetSigmoid32fAutoTest(int size, const FuncSG& f1, const FuncSG& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(size));
        const float lo = -10.0f, hi = 10.0f, slope = 1.1f;
        FillRandom32f(src, lo, hi);

        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(size));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(size));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, slope, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, slope, dst2));

        result = Compare(dst1, dst2, EPS, true, 32, false);

        return result;
    }

    bool SynetSigmoid32fAutoTest(const FuncSG& f1, const FuncSG& f2)
    {
        bool result = true;

        result = result && SynetSigmoid32fAutoTest(W * H, f1, f2);
        result = result && SynetSigmoid32fAutoTest(W * H + O, f1, f2);

        return result;
    }

    bool SynetSigmoid32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetSigmoid32fAutoTest(FUNC_SG(Simd::Base::SynetSigmoid32f), FUNC_SG(SimdSynetSigmoid32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetSigmoid32fAutoTest(FUNC_SG(Simd::Sse41::SynetSigmoid32f), FUNC_SG(SimdSynetSigmoid32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetSigmoid32fAutoTest(FUNC_SG(Simd::Avx2::SynetSigmoid32f), FUNC_SG(SimdSynetSigmoid32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetSigmoid32fAutoTest(FUNC_SG(Simd::Avx512bw::SynetSigmoid32f), FUNC_SG(SimdSynetSigmoid32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetSigmoid32fAutoTest(FUNC_SG(Simd::Neon::SynetSigmoid32f), FUNC_SG(SimdSynetSigmoid32f));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncSW
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* slope, float* dst);

            FuncPtr func;
            String description;

            FuncSW(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, float slope, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func((float*)src.data, src.width, &slope, (float*)dst.data);
            }
        };
    }
#define FUNC_SW(function) FuncSW(function, #function)

    bool SynetSwish32fAutoTest(int size, const FuncSW& f1, const FuncSW& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(size));
        const float lo = -10.0f, hi = 10.0f, slope = 1.1f;
        FillRandom32f(src, lo, hi);

        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(size));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(size));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, slope, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, slope, dst2));

        result = Compare(dst1, dst2, EPS, true, 32, false);

        return result;
    }

    bool SynetSwish32fAutoTest(const FuncSW& f1, const FuncSW& f2)
    {
        bool result = true;

        result = result && SynetSwish32fAutoTest(W * H, f1, f2);
        result = result && SynetSwish32fAutoTest(W * H + O, f1, f2);

        return result;
    }

    bool SynetSwish32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetSwish32fAutoTest(FUNC_SW(Simd::Base::SynetSwish32f), FUNC_SW(SimdSynetSwish32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetSwish32fAutoTest(FUNC_SW(Simd::Sse41::SynetSwish32f), FUNC_SW(SimdSynetSwish32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetSwish32fAutoTest(FUNC_SW(Simd::Avx2::SynetSwish32f), FUNC_SW(SimdSynetSwish32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetSwish32fAutoTest(FUNC_SW(Simd::Avx512bw::SynetSwish32f), FUNC_SW(SimdSynetSwish32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetSwish32fAutoTest(FUNC_SW(Simd::Neon::SynetSwish32f), FUNC_SW(SimdSynetSwish32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncSP
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* beta, const float* threshold, float* dst);

            FuncPtr func;
            String desc;

            FuncSP(const FuncPtr& f, const String& d) : func(f), desc(d) {}

            void Call(const View& src, float beta, float threshold, View & dst) const
            {
                TEST_PERFORMANCE_TEST(desc);
                func((float*)src.data, src.width, &beta, &threshold, (float*)dst.data);
            }
        };
    }

#define FUNC_SP(function) FuncSP(function, #function)

    bool SynetSoftplus32fAutoTest(size_t size, const FuncSP& f1, const FuncSP& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));

        const float beta = 3.0f, threshold = 20.0f;
        FillRandom32f(src, -10.0f, 10.0f);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, beta, threshold, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, beta, threshold, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32, false);

        return result;
    }

    bool SynetSoftplus32fAutoTest(const FuncSP& f1, const FuncSP& f2)
    {
        bool result = true;

        result = result && SynetSoftplus32fAutoTest(H * W, f1, f2);
        result = result && SynetSoftplus32fAutoTest(H * W + O, f1, f2);

        return result;
    }

    bool SynetSoftplus32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetSoftplus32fAutoTest(FUNC_SP(Simd::Base::SynetSoftplus32f), FUNC_SP(SimdSynetSoftplus32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetSoftplus32fAutoTest(FUNC_SP(Simd::Sse41::SynetSoftplus32f), FUNC_SP(SimdSynetSoftplus32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetSoftplus32fAutoTest(FUNC_SP(Simd::Avx2::SynetSoftplus32f), FUNC_SP(SimdSynetSoftplus32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetSoftplus32fAutoTest(FUNC_SP(Simd::Avx512bw::SynetSoftplus32f), FUNC_SP(SimdSynetSoftplus32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetSoftplus32fAutoTest(FUNC_SP(Simd::Neon::SynetSoftplus32f), FUNC_SP(SimdSynetSoftplus32f));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncTH
        {
            typedef void(*FuncPtr)(const float* src, size_t size, const float* slope, float* dst);

            FuncPtr func;
            String description;

            FuncTH(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, float slope, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func((float*)src.data, src.width, &slope, (float*)dst.data);
            }
        };
    }
#define FUNC_TH(function) FuncTH(function, #function)

    bool SynetTanh32fAutoTest(int size, const FuncTH& f1, const FuncTH& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(size));
        const float lo = -10.0f, hi = 10.0f, slope = 1.1f;
        FillRandom32f(src, lo, hi);

        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(size));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(size));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, slope, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, slope, dst2));

        result = Compare(dst1, dst2, EPS, true, 32, false);

        return result;
    }

    bool SynetTanh32fAutoTest(const FuncTH& f1, const FuncTH& f2)
    {
        bool result = true;

        result = result && SynetTanh32fAutoTest(W * H, f1, f2);
        result = result && SynetTanh32fAutoTest(W * H + O, f1, f2);

        return result;
    }

    bool SynetTanh32fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && SynetTanh32fAutoTest(FUNC_TH(Simd::Base::SynetTanh32f), FUNC_TH(SimdSynetTanh32f));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && SynetTanh32fAutoTest(FUNC_TH(Simd::Sse41::SynetTanh32f), FUNC_TH(SimdSynetTanh32f));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && SynetTanh32fAutoTest(FUNC_TH(Simd::Avx2::SynetTanh32f), FUNC_TH(SimdSynetTanh32f));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && SynetTanh32fAutoTest(FUNC_TH(Simd::Avx512bw::SynetTanh32f), FUNC_TH(SimdSynetTanh32f));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && SynetTanh32fAutoTest(FUNC_TH(Simd::Neon::SynetTanh32f), FUNC_TH(SimdSynetTanh32f));
#endif

        return result;
    }
#endif
}
