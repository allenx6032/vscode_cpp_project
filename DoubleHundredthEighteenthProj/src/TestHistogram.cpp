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
#include "TestString.h"
#include "TestRandom.h"

namespace Test
{
    namespace
    {
        struct FuncH
        {
            typedef void(*FuncPtr)(
                const uint8_t *src, size_t width, size_t height, size_t stride, uint32_t * histogram);

            FuncPtr func;
            String description;

            FuncH(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, uint32_t * histogram) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.width, src.height, src.stride, histogram);
            }
        };

        struct FuncHM
        {
            typedef void(*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height,
                const uint8_t * mask, size_t maskStride, uint8_t index, uint32_t * histogram);

            FuncPtr func;
            String description;

            FuncHM(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, const View & mask, uint8_t index, uint32_t * histogram) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, mask.data, mask.stride, index, histogram);
            }
        };


        struct FuncASDH
        {
            typedef void(*FuncPtr)(
                const uint8_t *src, size_t width, size_t height, size_t stride,
                size_t step, size_t indent, uint32_t * histogram);

            FuncPtr func;
            String description;

            FuncASDH(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, size_t step, size_t indent, uint32_t * histogram) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.width, src.height, src.stride,
                    step, indent, histogram);
            }
        };
    }

#define FUNC_H(function) FuncH(function, #function)

#define FUNC_HM(function) FuncHM(function, #function)

#define FUNC_ASDH(function) FuncASDH(function, #function)

    bool HistogramAutoTest(int width, int height, const FuncH & f1, const FuncH & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View s(int(width), int(height), View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(s);

        Histogram h1 = { 0 }, h2 = { 0 };

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, h1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, h2));

        result = result && Compare(h1, h2, 0, true, 32);

        return result;
    }

    bool HistogramAutoTest(const FuncH & f1, const FuncH & f2)
    {
        bool result = true;

        result = result && HistogramAutoTest(W, H, f1, f2);
        result = result && HistogramAutoTest(W + O, H - O, f1, f2);

        return result;
    }

    bool HistogramAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && HistogramAutoTest(FUNC_H(Simd::Base::Histogram), FUNC_H(SimdHistogram));

        return result;
    }

    bool HistogramMaskedAutoTest(int width, int height, const FuncHM & f1, const FuncHM & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View s(int(width), int(height), View::Gray8, NULL, TEST_ALIGN(width));
        View m(int(width), int(height), View::Gray8, NULL, TEST_ALIGN(width));

        const uint8_t index = 77;
        FillRandom(s);
        FillRandomMask(m, index);

        Histogram h1 = { 0 }, h2 = { 0 };

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, m, index, h1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, m, index, h2));

        result = result && Compare(h1, h2, 0, true, 32);

        return result;
    }

    bool HistogramMaskedAutoTest(const FuncHM & f1, const FuncHM & f2)
    {
        bool result = true;

        result = result && HistogramMaskedAutoTest(W, H, f1, f2);
        result = result && HistogramMaskedAutoTest(W + O, H - O, f1, f2);

        return result;
    }

    bool HistogramMaskedAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && HistogramMaskedAutoTest(FUNC_HM(Simd::Base::HistogramMasked), FUNC_HM(SimdHistogramMasked));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && HistogramMaskedAutoTest(FUNC_HM(Simd::Sse41::HistogramMasked), FUNC_HM(SimdHistogramMasked));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && HistogramMaskedAutoTest(FUNC_HM(Simd::Avx2::HistogramMasked), FUNC_HM(SimdHistogramMasked));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && HistogramMaskedAutoTest(FUNC_HM(Simd::Avx512bw::HistogramMasked), FUNC_HM(SimdHistogramMasked));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && HistogramMaskedAutoTest(FUNC_HM(Simd::Neon::HistogramMasked), FUNC_HM(SimdHistogramMasked));
#endif 

        return result;
    }

    bool AbsSecondDerivativeHistogramAutoTest(int width, int height, int step, int indent, int A, const FuncASDH & f1, const FuncASDH & f2)
    {
        bool result = true;

        if (width > 2 * indent && height > 2 * indent && indent >= step && width >= A + 2 * indent)
        {
            TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "] (" << step << ", " << indent << ").");

            View s(int(width), int(height), View::Gray8, NULL, TEST_ALIGN(width));
            FillRandom(s);

            Histogram h1 = { 0 }, h2 = { 0 };

            TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, step, indent, h1));

            TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, step, indent, h2));

            result = result && Compare(h1, h2, 0, true, 32);
        }

        return result;
    }

    bool AbsSecondDerivativeHistogramAutoTest(int A, const FuncASDH & f1, const FuncASDH & f2)
    {
        bool result = true;

        result = result && AbsSecondDerivativeHistogramAutoTest(W, H, 1, 16, A, f1, f2);
        result = result && AbsSecondDerivativeHistogramAutoTest(W + O, H - O, 2, 16, A, f1, f2);
        result = result && AbsSecondDerivativeHistogramAutoTest(W, H, 3, 8, A, f1, f2);

        return result;
    }

    bool AbsSecondDerivativeHistogramAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AbsSecondDerivativeHistogramAutoTest(1, FUNC_ASDH(Simd::Base::AbsSecondDerivativeHistogram), FUNC_ASDH(SimdAbsSecondDerivativeHistogram));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && AbsSecondDerivativeHistogramAutoTest(Simd::Sse41::A, FUNC_ASDH(Simd::Sse41::AbsSecondDerivativeHistogram), FUNC_ASDH(SimdAbsSecondDerivativeHistogram));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && AbsSecondDerivativeHistogramAutoTest(Simd::Avx2::A, FUNC_ASDH(Simd::Avx2::AbsSecondDerivativeHistogram), FUNC_ASDH(SimdAbsSecondDerivativeHistogram));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AbsSecondDerivativeHistogramAutoTest(Simd::Avx512bw::A, FUNC_ASDH(Simd::Avx512bw::AbsSecondDerivativeHistogram), FUNC_ASDH(SimdAbsSecondDerivativeHistogram));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && AbsSecondDerivativeHistogramAutoTest(Simd::Neon::A, FUNC_ASDH(Simd::Neon::AbsSecondDerivativeHistogram), FUNC_ASDH(SimdAbsSecondDerivativeHistogram));
#endif 

        return result;
    }

    namespace
    {
        struct FuncCC
        {
            typedef void(*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height,
                const uint8_t * colors, uint8_t * dst, size_t dstStride);

            FuncPtr func;
            String description;

            FuncCC(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, const View & colors, const View & dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, colors.data, dst.data, dst.stride);
            }
        };
    }

#define FUNC_CC(function) \
    FuncCC(function, std::string(#function))

    bool ChangeColorsAutoTest(int width, int height, const FuncCC & f1, const FuncCC & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View s(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(s);

        View c(Simd::HISTOGRAM_SIZE, 1, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(c, 64, 191);

        View d1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View d2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, c, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, c, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool ChangeColorsAutoTest(const FuncCC & f1, const FuncCC & f2)
    {
        bool result = true;

        result = result && ChangeColorsAutoTest(W, H, f1, f2);
        result = result && ChangeColorsAutoTest(W + O, H - O, f1, f2);

        return result;
    }

    bool ChangeColorsAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && ChangeColorsAutoTest(FUNC_CC(Simd::Base::ChangeColors), FUNC_CC(SimdChangeColors));

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw() && W >= Simd::Avx512bw::HA)
            result = result && ChangeColorsAutoTest(FUNC_CC(Simd::Avx512bw::ChangeColors), FUNC_CC(SimdChangeColors));
#endif 

#ifdef SIMD_AMXBF16_ENABLE
        if (Simd::AmxBf16::Enable && TestAmxBf16())
            result = result && ChangeColorsAutoTest(FUNC_CC(Simd::AmxBf16::ChangeColors), FUNC_CC(SimdChangeColors));
#endif

        return result;
    }

    namespace
    {
        struct FuncHC
        {
            typedef void(*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height,
                const uint8_t * mask, size_t maskStride, uint8_t value, SimdCompareType compareType, uint32_t * histogram);

            FuncPtr func;
            String description;

            FuncHC(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, const View & mask, uint8_t value, SimdCompareType compareType, uint32_t * histogram) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, mask.data, mask.stride, value, compareType, histogram);
            }
        };
    }

#define ARGS_HC(width, height, type, function1, function2) \
    width, height, type, \
    FuncHC(function1.func, function1.description + CompareTypeDescription(type)), \
    FuncHC(function2.func, function2.description + CompareTypeDescription(type))

#define FUNC_HC(function) \
    FuncHC(function, std::string(#function))

    bool HistogramConditionalAutoTest(int width, int height, SimdCompareType type, const FuncHC & f1, const FuncHC & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View s(int(width), int(height), View::Gray8, NULL, TEST_ALIGN(width));
        View m(int(width), int(height), View::Gray8, NULL, TEST_ALIGN(width));

        uint8_t value = 127;
        FillRandom(s);
        FillRandom(m);

        Histogram h1 = { 0 }, h2 = { 0 };

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, m, value, type, h1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, m, value, type, h2));

        result = result && Compare(h1, h2, 0, true, 32);

        return result;
    }

    bool HistogramConditionalAutoTest(const FuncHC & f1, const FuncHC & f2)
    {
        bool result = true;

        for (SimdCompareType type = SimdCompareEqual; type <= SimdCompareLesserOrEqual && result; type = SimdCompareType(type + 1))
        {
            result = result && HistogramConditionalAutoTest(ARGS_HC(W, H, type, f1, f2));
            result = result && HistogramConditionalAutoTest(ARGS_HC(W + O, H - O, type, f1, f2));
        }

        return result;
    }

    bool HistogramConditionalAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && HistogramConditionalAutoTest(FUNC_HC(Simd::Base::HistogramConditional), FUNC_HC(SimdHistogramConditional));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && HistogramConditionalAutoTest(FUNC_HC(Simd::Sse41::HistogramConditional), FUNC_HC(SimdHistogramConditional));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && HistogramConditionalAutoTest(FUNC_HC(Simd::Avx2::HistogramConditional), FUNC_HC(SimdHistogramConditional));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && HistogramConditionalAutoTest(FUNC_HC(Simd::Avx512bw::HistogramConditional), FUNC_HC(SimdHistogramConditional));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && HistogramConditionalAutoTest(FUNC_HC(Simd::Neon::HistogramConditional), FUNC_HC(SimdHistogramConditional));
#endif 

        return result;
    }
}
