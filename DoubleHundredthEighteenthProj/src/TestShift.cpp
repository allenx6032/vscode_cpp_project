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
#include "TestString.h"
#include "TestRandom.h"
#include "TestOptions.h"

namespace Test
{
    namespace
    {
        struct Func
        {
            typedef void(*FuncPtr)(const uint8_t* src, size_t srcStride, size_t width, size_t height, size_t channelCount,
                const uint8_t* bkg, size_t bkgStride, const double* shiftX, const double* shiftY,
                size_t cropLeft, size_t cropTop, size_t cropRight, size_t cropBottom, uint8_t* dst, size_t dstStride);

            FuncPtr func;
            String description;

            Func(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, const View& bkg, double shiftX, double shiftY,
                size_t cropLeft, size_t cropTop, size_t cropRight, size_t cropBottom, View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, View::PixelSize(src.format), bkg.data, bkg.stride,
                    &shiftX, &shiftY, cropLeft, cropTop, cropRight, cropBottom, dst.data, dst.stride);
            }
        };
    }

#define ARGS(format, width, height, function1, function2) \
	format, width, height, \
	Func(function1.func, function1.description + ColorDescription(format)), \
	Func(function2.func, function2.description + ColorDescription(format))

#define FUNC(function) \
    Func(function, std::string(#function))

    bool ShiftAutoTest(View::Format format, int width, int height, double dx, double dy, int crop, const Func& f1, const Func& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, std::setprecision(1) << std::fixed << "Test " << f1.description << " & " << f2.description
            << " [" << width << ", " << height << "]," << " (" << dx << ", " << dy << ", " << crop << ").");

        View s(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(s);
        View b(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(b);

        View d1(width, height, format, NULL, TEST_ALIGN(width));
        View d2(width, height, format, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, b, dx, dy, crop, crop, width - crop, height - crop, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, b, dx, dy, crop, crop, width - crop, height - crop, d2));

        result = result && Compare(d1, d2, 0, true, 32);

        return result;
    }

    bool ShiftAutoTest(View::Format format, int width, int height, const Func& f1, const Func& f2)
    {
        bool result = true;

        const double x0 = 6.9, dx = -5.3, y0 = -5.2, dy = 3.7;
        for (int i = 0; i < 4; ++i)
            result = result && ShiftAutoTest(format, width, height, x0 + i * dx, y0 + i * dy, i * 3, f1, f2);

        return result;
    }

    bool ShiftBilinearAutoTest(const Func& f1, const Func& f2)
    {
        bool result = true;

        for (View::Format format = View::Gray8; format <= View::Bgra32; format = View::Format(format + 1))
        {
            result = result && ShiftAutoTest(ARGS(format, W, H, f1, f2));
            result = result && ShiftAutoTest(ARGS(format, W + O, H - O, f1, f2));
        }

        return result;
    }

    bool ShiftBilinearAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && ShiftBilinearAutoTest(FUNC(Simd::Base::ShiftBilinear), FUNC(SimdShiftBilinear));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && ShiftBilinearAutoTest(FUNC(Simd::Sse41::ShiftBilinear), FUNC(SimdShiftBilinear));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && ShiftBilinearAutoTest(FUNC(Simd::Avx2::ShiftBilinear), FUNC(SimdShiftBilinear));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && ShiftBilinearAutoTest(FUNC(Simd::Avx512bw::ShiftBilinear), FUNC(SimdShiftBilinear));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && ShiftBilinearAutoTest(FUNC(Simd::Neon::ShiftBilinear), FUNC(SimdShiftBilinear));
#endif 

        return result;
    }
}

//-------------------------------------------------------------------------------------------------

#include "SimdShift.hpp"
#include "SimdDrawing.hpp"

namespace Test
{
    bool CreateBackground(View & bkg, Rect & rect)
    {
        View obj;

        const uint8_t lo = 64, hi = 192;
        const size_t s = 256, co = s / 2;
        const size_t rl2 = Simd::Square(co * 2 / 7), rh2 = Simd::Square(co * 4 / 7);
        obj.Recreate(s, s, View::Gray8);
        for (size_t y = 0; y < s; ++y)
        {
            size_t dy2 = Simd::Square(co - y);
            for (size_t x = 0; x < s; ++x)
            {
                size_t dx2 = Simd::Square(co - x);
                size_t r2 = dy2 + dx2;
                obj.At<uint8_t>(x, y) = (r2 >= rl2 && r2 <= rh2 ? hi : lo);

            }
        }
        FillRandom(bkg, 0, lo * 2);
        Point c(bkg.width / 2, bkg.height / 2);

        std::vector<uint8_t> profile(s, 255);
        for (size_t i = 0, n = s / 4; i < n; ++i)
            profile[s - i - 1] = profile[i] = uint8_t(i * 255 / n);
        View alpha(s, s, View::Gray8);
        Simd::VectorProduct(profile.data(), profile.data(), alpha);

        size_t hs = s / 2;
        rect = Rect(c.x - hs, c.y - hs, c.x + hs, c.y + hs);
        Simd::AlphaBlending(obj, alpha, bkg.Region(rect).Ref());
        rect.AddBorder(-int(hs / 4));

        return true;
    }

    bool ShiftDetectorRandSpecialTest(const Options & options)
    {
        typedef Simd::ShiftDetector<Simd::Allocator> ShiftDetector;

        ::srand(1);

        Rect region;
        View background(1920, 1080, View::Gray8);
        if (!CreateBackground(background, region))
            return false;

        ShiftDetector shiftDetector;
        double time;

        time = GetTime();
        shiftDetector.InitBuffers(background.Size(), 6, ShiftDetector::TextureGray, ShiftDetector::SquaredDifference);
        TEST_LOG_SS(Info, "InitBuffers : " << (GetTime() - time) * 1000 << " ms ");

        time = GetTime();
        shiftDetector.SetBackground(background);
        TEST_LOG_SS(Info, "SetBackground : " << (GetTime() - time) * 1000 << " ms ");

        int n = 10;
        time = GetTime();
        for (int i = 0; i < n; ++i)
        {
            const int ms = (int)region.Width() / 4;
            Point ss(Random(2 * ms) - ms, Random(2 * ms) - ms);

            shiftDetector.Estimate(background.Region(region), region.Shifted(ss), ms * 2);

            ShiftDetector::FPoint ds = shiftDetector.ProximateShift();

            if (Simd::SquaredDistance(ShiftDetector::FPoint(ss), ds) > 1.0)
            {
                Simd::DrawRectangle(background, region, uint8_t(255));
                Simd::DrawRectangle(background, region.Shifted(ss), uint8_t(0));
                background.Save("background.pgm");
                TEST_LOG_SS(Error, "Detected shift (" << ds.x << ", " << ds.y << ") is not equal to original shift (" << ss.x << ", " << ss.y << ") !");
                return false;
            }
        }
        TEST_LOG_SS(Info, "Estimate : " << (GetTime() - time) * 1000.0 / n << " ms ");

        return true;
    }

    bool ShiftDetectorFileSpecialTest(const Options & options)
    {
        typedef Simd::ShiftDetector<Simd::Allocator> ShiftDetector;

        ShiftDetector shiftDetector;

        ShiftDetector::View background;
        String path = ROOT_PATH + "/data/image/face/lena.pgm";
        if (!background.Load(path))
        {
            TEST_LOG_SS(Error, "Can't load test image '" << path << "' !");
            return false;
        }

        shiftDetector.InitBuffers(background.Size(), 4);

        shiftDetector.SetBackground(background);

        ShiftDetector::Rect region(64, 64, 192, 192);

        ShiftDetector::View current = background.Region(region.Shifted(10, 10));

        if (shiftDetector.Estimate(current, region, 32))
        {
            ShiftDetector::Point shift = shiftDetector.Shift();
            std::cout << "Shift = (" << shift.x << ", " << shift.y << "). " << std::endl;
        }
        else
            std::cout << "Can't find shift for current image!" << std::endl;

        return true;
    }
}
