/*
* Tests for Simd Library (http://ermig1979.github.io/Simd).
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
#include "TestCompare.h"
#include "TestPerformance.h"
#include "TestRandom.h"
#include "TestOptions.h"

namespace Test
{
    namespace
    {
        struct FuncM
        {
            typedef void(*FuncPtr)(const uint8_t* src, size_t srcStride, size_t width, size_t height,
                const uint8_t* mask, size_t maskStride, uint8_t indexMin, uint8_t* dst, size_t dstStride);

            FuncPtr func;
            String description;

            FuncM(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, const View& mask, uint8_t indexMin, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, mask.data, mask.stride, indexMin, dst.data, dst.stride);
            }
        };
    }

#define FUNC_M(function) \
    FuncM(function, std::string(#function))

    bool ContourMetricsMaskedAutoTest(int width, int height, const FuncM& f1, const FuncM& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View s(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(s);

        View m(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(m);

        View d1(width, height, View::Int16, NULL, TEST_ALIGN(width));
        View d2(width, height, View::Int16, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, m, 128, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, m, 128, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool ContourMetricsMaskedAutoTest(const FuncM& f1, const FuncM& f2)
    {
        bool result = true;

        result = result && ContourMetricsMaskedAutoTest(W, H, f1, f2);
        result = result && ContourMetricsMaskedAutoTest(W + O, H - O, f1, f2);

        return result;
    }

    bool ContourMetricsMaskedAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && ContourMetricsMaskedAutoTest(FUNC_M(Simd::Base::ContourMetricsMasked), FUNC_M(SimdContourMetricsMasked));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W > Simd::Sse41::A)
            result = result && ContourMetricsMaskedAutoTest(FUNC_M(Simd::Sse41::ContourMetricsMasked), FUNC_M(SimdContourMetricsMasked));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W > Simd::Avx2::A)
            result = result && ContourMetricsMaskedAutoTest(FUNC_M(Simd::Avx2::ContourMetricsMasked), FUNC_M(SimdContourMetricsMasked));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw() && W > Simd::Avx512bw::A)
            result = result && ContourMetricsMaskedAutoTest(FUNC_M(Simd::Avx512bw::ContourMetricsMasked), FUNC_M(SimdContourMetricsMasked));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W > Simd::Neon::A)
            result = result && ContourMetricsMaskedAutoTest(FUNC_M(Simd::Neon::ContourMetricsMasked), FUNC_M(SimdContourMetricsMasked));
#endif

        return result;
    }

    namespace
    {
        struct FuncA
        {
            typedef void(*FuncPtr)(const uint8_t* src, size_t srcStride, size_t width, size_t height,
                size_t step, int16_t threshold, uint8_t* dst, size_t dstStride);

            FuncPtr func;
            String description;

            FuncA(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, size_t step, int16_t threshold, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, step, threshold, dst.data, dst.stride);
            }
        };
    }

#define FUNC_A(function) \
    FuncA(function, std::string(#function))

    bool ContourAnchorsAutoTest(int width, int height, const FuncA& f1, const FuncA& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View s(width, height, View::Int16, NULL, TEST_ALIGN(width));
        FillRandom(s);

        View d1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View d2(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        Simd::Fill(d1, 0);
        Simd::Fill(d2, 0);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, 3, 0, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, 3, 0, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool ContourAnchorsAutoTest(const FuncA& f1, const FuncA& f2)
    {
        bool result = true;

        result = result && ContourAnchorsAutoTest(W, H, f1, f2);
        result = result && ContourAnchorsAutoTest(W + O, H - O, f1, f2);

        return result;
    }

    bool ContourAnchorsAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && ContourAnchorsAutoTest(FUNC_A(Simd::Base::ContourAnchors), FUNC_A(SimdContourAnchors));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W > Simd::Sse41::A)
            result = result && ContourAnchorsAutoTest(FUNC_A(Simd::Sse41::ContourAnchors), FUNC_A(SimdContourAnchors));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W > Simd::Avx2::A)
            result = result && ContourAnchorsAutoTest(FUNC_A(Simd::Avx2::ContourAnchors), FUNC_A(SimdContourAnchors));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw() && W > Simd::Avx512bw::A)
            result = result && ContourAnchorsAutoTest(FUNC_A(Simd::Avx512bw::ContourAnchors), FUNC_A(SimdContourAnchors));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W > Simd::Neon::A)
            result = result && ContourAnchorsAutoTest(FUNC_A(Simd::Neon::ContourAnchors), FUNC_A(SimdContourAnchors));
#endif

        return result;
    }
}

//-----------------------------------------------------------------------------

#include "SimdContour.hpp"
#include "SimdDrawing.hpp"

namespace Test
{
    typedef Simd::ContourDetector<Simd::Allocator> ContourDetector;

    bool ContourDetectorSpecialTest(const Options & options)
    {
        ContourDetector::View image;

        String path = ROOT_PATH + "/data/image/face/lena.pgm";
        if (!image.Load(path))
        {
            TEST_LOG_SS(Error, "Can't load test image '" << path << "' !");
            return false;
        }

        ContourDetector detector;
        detector.Init(image.Size());

        ContourDetector::Contours contours;
        detector.Detect(image, contours);

        TEST_LOG_SS(Info, contours.size() << " contours were found.");

        for (size_t i = 0; i < contours.size(); ++i)
        {
            for (size_t j = 1; j < contours[i].size(); ++j)
                Simd::DrawLine(image, contours[i][j - 1], contours[i][j], uint8_t(255));
        }
        image.Save("result.pgm");

        return true;
    }
}


