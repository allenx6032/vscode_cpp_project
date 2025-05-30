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
        struct FuncAB
        {
            typedef void(*FuncPtr)(const uint8_t *src, size_t srcStride, size_t width, size_t height, size_t channelCount,
                const uint8_t *alpha, size_t alphaStride, uint8_t *dst, size_t dstStride);
            FuncPtr func;
            String description;

            FuncAB(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, const View & alpha, const View & dstSrc, View & dstDst) const
            {
                Simd::Copy(dstSrc, dstDst);
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, src.ChannelCount(), alpha.data, alpha.stride, dstDst.data, dstDst.stride);
            }
        };
    }

#define FUNC_AB(func) FuncAB(func, #func)

    bool AlphaBlendingAutoTest(View::Format format, int width, int height, const FuncAB & f1, const FuncAB & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size [" << width << "," << height << "].");

        View s(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(s);
        View a(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(a);
        View b(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(b);

        View d1(width, height, format, NULL, TEST_ALIGN(width));
        View d2(width, height, format, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, a, b, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, a, b, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool AlphaBlendingAutoTest(const FuncAB & f1, const FuncAB & f2)
    {
        bool result = true;

        for (View::Format format = View::Gray8; format <= View::Bgra32; format = View::Format(format + 1))
        {
            FuncAB f1c = FuncAB(f1.func, f1.description + ColorDescription(format));
            FuncAB f2c = FuncAB(f2.func, f2.description + ColorDescription(format));

            result = result && AlphaBlendingAutoTest(format, W, H, f1c, f2c);
            result = result && AlphaBlendingAutoTest(format, W + O, H - O, f1c, f2c);
        }

        return result;
    }

    bool AlphaBlendingAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaBlendingAutoTest(FUNC_AB(Simd::Base::AlphaBlending), FUNC_AB(SimdAlphaBlending));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && AlphaBlendingAutoTest(FUNC_AB(Simd::Sse41::AlphaBlending), FUNC_AB(SimdAlphaBlending));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && AlphaBlendingAutoTest(FUNC_AB(Simd::Avx2::AlphaBlending), FUNC_AB(SimdAlphaBlending));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AlphaBlendingAutoTest(FUNC_AB(Simd::Avx512bw::AlphaBlending), FUNC_AB(SimdAlphaBlending));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && AlphaBlendingAutoTest(FUNC_AB(Simd::Neon::AlphaBlending), FUNC_AB(SimdAlphaBlending));
#endif

        return result;
    }

    //---------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncAB2
        {
            typedef void(*FuncPtr)(const uint8_t* src0, size_t src0Stride, const uint8_t* alpha0, size_t alpha0Stride, 
                const uint8_t* src1, size_t src1Stride, const uint8_t* alpha1, size_t alpha1Stride,
                size_t width, size_t height, size_t channelCount, uint8_t* dst, size_t dstStride);
            FuncPtr func;
            String description;

            FuncAB2(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src0, const View& alpha0, const View& src1, const View& alpha1, const View& dstSrc, View& dstDst) const
            {
                Simd::Copy(dstSrc, dstDst);
                TEST_PERFORMANCE_TEST(description);
                func(src0.data, src0.stride, alpha0.data, alpha0.stride, src1.data, src1.stride, alpha1.data, alpha1.stride,
                    dstDst.width, dstDst.height, dstDst.ChannelCount(), dstDst.data, dstDst.stride);
            }
        };
    }

#define FUNC_AB2(func) FuncAB2(func, #func)

    bool AlphaBlending2xAutoTest(View::Format format, int width, int height, const FuncAB2& f1, const FuncAB2& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size [" << width << "," << height << "].");

        View s0(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(s0);
        View a0(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(a0);
        View s1(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(s1);
        View a1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(a1);
        View b(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(b);

        View d1(width, height, format, NULL, TEST_ALIGN(width));
        View d2(width, height, format, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s0, a0, s1, a1, b, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s0, a0, s1, a1, b, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool AlphaBlending2xAutoTest(const FuncAB2& f1, const FuncAB2& f2)
    {
        bool result = true;

        for (View::Format format = View::Gray8; format <= View::Bgra32; format = View::Format(format + 1))
        {
            FuncAB2 f1c = FuncAB2(f1.func, f1.description + ColorDescription(format));
            FuncAB2 f2c = FuncAB2(f2.func, f2.description + ColorDescription(format));

            result = result && AlphaBlending2xAutoTest(format, W, H, f1c, f2c);
            result = result && AlphaBlending2xAutoTest(format, W + O, H - O, f1c, f2c);
        }

        return result;
    }

    bool AlphaBlending2xAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaBlending2xAutoTest(FUNC_AB2(Simd::Base::AlphaBlending2x), FUNC_AB2(SimdAlphaBlending2x));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && AlphaBlending2xAutoTest(FUNC_AB2(Simd::Sse41::AlphaBlending2x), FUNC_AB2(SimdAlphaBlending2x));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && AlphaBlending2xAutoTest(FUNC_AB2(Simd::Avx2::AlphaBlending2x), FUNC_AB2(SimdAlphaBlending2x));
#endif

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && AlphaBlending2xAutoTest(FUNC_AB2(Simd::Neon::AlphaBlending2x), FUNC_AB2(SimdAlphaBlending2x));
#endif 

        return result;
    }

    //---------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncABBY
        {
            typedef void(*FuncPtr)(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
                uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType);
            FuncPtr func;
            String description;

            FuncABBY(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& bgra, View * y, View * u, View * v, int i, SimdYuvType yuvType) const
            {
                Simd::Copy(y[0], y[i]);
                Simd::Copy(u[0], u[i]);
                Simd::Copy(v[0], v[i]);
                TEST_PERFORMANCE_TEST(description);
                func(bgra.data, bgra.stride, bgra.width, bgra.height, y[i].data, y[i].stride, u[i].data, u[i].stride, v[i].data, v[i].stride, yuvType);
            }
        };
    }

#define FUNC_ABBY(func) FuncABBY(func, #func)

    bool AlphaBlendingBgraToYuv420pAutoTest(int width, int height, SimdYuvType yuvType, const FuncABBY& f1, const FuncABBY& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size [" << width << "," << height << "].");

        View bgra(width, height, View::Bgra32, NULL, TEST_ALIGN(width));

        View y[3], u[3], v[3];
        for (int i = 0; i < 3; ++i)
        {
            y[i].Recreate(width, height, View::Gray8, NULL, TEST_ALIGN(width));
            u[i].Recreate(width / 2, height / 2, View::Gray8, NULL, TEST_ALIGN(width));
            v[i].Recreate(width / 2, height / 2, View::Gray8, NULL, TEST_ALIGN(width));
        }

        FillRandom(bgra);
        FillRandom(y[0]);
        FillRandom(u[0]);
        FillRandom(v[0]);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(bgra, y, u, v, 1, yuvType));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(bgra, y, u, v, 2, yuvType));

        result = result && Compare(y[1], y[2], 0, true, 64, 0, "y");
        result = result && Compare(u[1], u[2], 0, true, 64, 0, "u");
        result = result && Compare(v[1], v[2], 0, true, 64, 0, "v");

        return result;
    }

    bool AlphaBlendingBgraToYuv420pAutoTest(const FuncABBY& f1, const FuncABBY& f2)
    {
        bool result = true;

        result = result && AlphaBlendingBgraToYuv420pAutoTest(W, H, SimdYuvBt601, f1, f2);
        result = result && AlphaBlendingBgraToYuv420pAutoTest(W + E, H - E, SimdYuvBt709, f1, f2);

        return result;
    }

    bool AlphaBlendingBgraToYuv420pAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaBlendingBgraToYuv420pAutoTest(FUNC_ABBY(Simd::Base::AlphaBlendingBgraToYuv420p), FUNC_ABBY(SimdAlphaBlendingBgraToYuv420p));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && AlphaBlendingBgraToYuv420pAutoTest(FUNC_ABBY(Simd::Sse41::AlphaBlendingBgraToYuv420p), FUNC_ABBY(SimdAlphaBlendingBgraToYuv420p));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && AlphaBlendingBgraToYuv420pAutoTest(FUNC_ABBY(Simd::Avx2::AlphaBlendingBgraToYuv420p), FUNC_ABBY(SimdAlphaBlendingBgraToYuv420p));
#endif 


#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AlphaBlendingBgraToYuv420pAutoTest(FUNC_ABBY(Simd::Avx512bw::AlphaBlendingBgraToYuv420p), FUNC_ABBY(SimdAlphaBlendingBgraToYuv420p));
#endif

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && AlphaBlendingBgraToYuv420pAutoTest(FUNC_ABBY(Simd::Neon::AlphaBlendingBgraToYuv420p), FUNC_ABBY(SimdAlphaBlendingBgraToYuv420p));
#endif 

        return result;
    }

    //---------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncABU
        {
            typedef void(*FuncPtr)(const uint8_t* src, size_t srcStride, size_t width, size_t height, size_t channelCount, uint8_t alpha, uint8_t* dst, size_t dstStride);
            FuncPtr func;
            String description;

            FuncABU(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, uint8_t alpha, const View& dstSrc, View& dstDst) const
            {
                Simd::Copy(dstSrc, dstDst);
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, src.ChannelCount(), alpha, dstDst.data, dstDst.stride);
            }
        };
    }

#define FUNC_ABU(func) FuncABU(func, #func)

    bool AlphaBlendingUniformAutoTest(View::Format format, int width, int height, const FuncABU& f1, const FuncABU& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size [" << width << "," << height << "].");

        View s(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(s);
        View b(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(b);

        View d1(width, height, format, NULL, TEST_ALIGN(width));
        View d2(width, height, format, NULL, TEST_ALIGN(width));

        uint8_t a = 77;

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, a, b, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, a, b, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool AlphaBlendingUniformAutoTest(const FuncABU& f1, const FuncABU& f2)
    {
        bool result = true;

        for (View::Format format = View::Gray8; format <= View::Bgra32; format = View::Format(format + 1))
        {
            FuncABU f1c = FuncABU(f1.func, f1.description + ColorDescription(format));
            FuncABU f2c = FuncABU(f2.func, f2.description + ColorDescription(format));

            result = result && AlphaBlendingUniformAutoTest(format, W, H, f1c, f2c);
            result = result && AlphaBlendingUniformAutoTest(format, W + O, H - O, f1c, f2c);
        }

        return result;
    }

    bool AlphaBlendingUniformAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaBlendingUniformAutoTest(FUNC_ABU(Simd::Base::AlphaBlendingUniform), FUNC_ABU(SimdAlphaBlendingUniform));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && AlphaBlendingUniformAutoTest(FUNC_ABU(Simd::Sse41::AlphaBlendingUniform), FUNC_ABU(SimdAlphaBlendingUniform));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && AlphaBlendingUniformAutoTest(FUNC_ABU(Simd::Avx2::AlphaBlendingUniform), FUNC_ABU(SimdAlphaBlendingUniform));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AlphaBlendingUniformAutoTest(FUNC_ABU(Simd::Avx512bw::AlphaBlendingUniform), FUNC_ABU(SimdAlphaBlendingUniform));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && AlphaBlendingUniformAutoTest(FUNC_ABU(Simd::Neon::AlphaBlendingUniform), FUNC_ABU(SimdAlphaBlendingUniform));
#endif

        return result;
    }

    //-------------------------------------------------------------------------

    namespace
    {
        struct FuncAF
        {
            typedef void(*FuncPtr)(uint8_t * dst, size_t dstStride, size_t width, size_t height,
                const uint8_t * channel, size_t channelCount, const uint8_t * alpha, size_t alphaStride);
            FuncPtr func;
            String description;

            FuncAF(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & pixel, const View & alpha, const View & dstSrc, View & dstDst) const
            {
                Simd::Copy(dstSrc, dstDst);
                TEST_PERFORMANCE_TEST(description);
                func(dstDst.data, dstDst.stride, dstDst.width, dstDst.height, pixel.data, pixel.ChannelCount(), alpha.data, alpha.stride);
            }
        };
    }

#define FUNC_AF(func) FuncAF(func, #func)

    bool AlphaFillingAutoTest(View::Format format, int width, int height, const FuncAF & f1, const FuncAF & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size [" << width << "," << height << "].");

        View p(1, 1, format, NULL, TEST_ALIGN(width));
        FillRandom(p);
        View a(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(a);
        View b(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(b);

        View d1(width, height, format, NULL, TEST_ALIGN(width));
        View d2(width, height, format, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(p, a, b, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(p, a, b, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool AlphaFillingAutoTest(const FuncAF & f1, const FuncAF & f2)
    {
        bool result = true;

        for (View::Format format = View::Gray8; format <= View::Bgra32; format = View::Format(format + 1))
        {
            FuncAF f1c = FuncAF(f1.func, f1.description + ColorDescription(format));
            FuncAF f2c = FuncAF(f2.func, f2.description + ColorDescription(format));

            result = result && AlphaFillingAutoTest(format, W, H, f1c, f2c);
            result = result && AlphaFillingAutoTest(format, W + O, H - O, f1c, f2c);
        }

        return result;
    }

    bool AlphaFillingAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaFillingAutoTest(FUNC_AF(Simd::Base::AlphaFilling), FUNC_AF(SimdAlphaFilling));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41() && W >= Simd::Sse41::A)
            result = result && AlphaFillingAutoTest(FUNC_AF(Simd::Sse41::AlphaFilling), FUNC_AF(SimdAlphaFilling));
#endif

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2() && W >= Simd::Avx2::A)
            result = result && AlphaFillingAutoTest(FUNC_AF(Simd::Avx2::AlphaFilling), FUNC_AF(SimdAlphaFilling));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AlphaFillingAutoTest(FUNC_AF(Simd::Avx512bw::AlphaFilling), FUNC_AF(SimdAlphaFilling));
#endif

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon() && W >= Simd::Neon::A)
            result = result && AlphaFillingAutoTest(FUNC_AF(Simd::Neon::AlphaFilling), FUNC_AF(SimdAlphaFilling));
#endif

        return result;
    }

    //-----------------------------------------------------------------------

    namespace
    {
        struct FuncAP
        {
            typedef void(*FuncPtr)(const uint8_t* src, size_t srcStride, size_t width, size_t height, uint8_t* dst, size_t dstStride, SimdBool argb);
            FuncPtr func;
            String description;

            FuncAP(const FuncPtr& f, const String& d) : func(f), description(d) {}

            void Call(const View& src, const View& dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, dst.data, dst.stride, src.format == View::Argb32 ? SimdTrue : SimdFalse);
            }
        };
    }

#define FUNC_AP(func) FuncAP(func, #func)

    bool AlphaPremultiplyAutoTest(bool inv, int width, int height, View::Format format, const FuncAP& f1, const FuncAP& f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size [" << width << "x" << height << "-" << ToString(format) << "].");

        View s(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(s);
        s.data[format == View::Argb32 ? 0 : 3] = 0;
        if (inv)
            Simd::AlphaPremultiply(s, s);
        s.data[0] = 7;
        s.data[1] = 7;
        s.data[2] = 7;
        s.data[3] = 7;

        View d1(width, height, format, NULL, TEST_ALIGN(width));
        View d2(width, height, format, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, d1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, d2));

        result = result && Compare(d1, d2, 0, true, 64);

        return result;
    }

    bool AlphaPremultiplyAutoTest(bool inv, const FuncAP& f1, const FuncAP& f2)
    {
        bool result = true;

        result = result && AlphaPremultiplyAutoTest(inv, W, H, View::Bgra32, f1, f2);
        result = result && AlphaPremultiplyAutoTest(inv, W + O, H - O, View::Rgba32, f1, f2);
        result = result && AlphaPremultiplyAutoTest(inv, W, H, View::Argb32, f1, f2);

        return result;
    }

    bool AlphaPremultiplyAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaPremultiplyAutoTest(false, FUNC_AP(Simd::Base::AlphaPremultiply), FUNC_AP(SimdAlphaPremultiply));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && AlphaPremultiplyAutoTest(false, FUNC_AP(Simd::Sse41::AlphaPremultiply), FUNC_AP(SimdAlphaPremultiply));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && AlphaPremultiplyAutoTest(false, FUNC_AP(Simd::Avx2::AlphaPremultiply), FUNC_AP(SimdAlphaPremultiply));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AlphaPremultiplyAutoTest(false, FUNC_AP(Simd::Avx512bw::AlphaPremultiply), FUNC_AP(SimdAlphaPremultiply));
#endif

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && AlphaPremultiplyAutoTest(false, FUNC_AP(Simd::Neon::AlphaPremultiply), FUNC_AP(SimdAlphaPremultiply));
#endif

        return result;
    }

    bool AlphaUnpremultiplyAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && AlphaPremultiplyAutoTest(true, FUNC_AP(Simd::Base::AlphaUnpremultiply), FUNC_AP(SimdAlphaUnpremultiply));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && AlphaPremultiplyAutoTest(true, FUNC_AP(Simd::Sse41::AlphaUnpremultiply), FUNC_AP(SimdAlphaUnpremultiply));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && AlphaPremultiplyAutoTest(true, FUNC_AP(Simd::Avx2::AlphaUnpremultiply), FUNC_AP(SimdAlphaUnpremultiply));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && AlphaPremultiplyAutoTest(true, FUNC_AP(Simd::Avx512bw::AlphaUnpremultiply), FUNC_AP(SimdAlphaUnpremultiply));
#endif 

#ifdef SIMD_NEON_ENABLE
        if (Simd::Neon::Enable && TestNeon())
            result = result && AlphaPremultiplyAutoTest(true, FUNC_AP(Simd::Neon::AlphaUnpremultiply), FUNC_AP(SimdAlphaUnpremultiply));
#endif 

        return result;
    }
}

//-----------------------------------------------------------------------------

#include "SimdDrawing.hpp"

namespace Test
{
    bool DrawLineSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 256, m = 20, w = 3;

        for (size_t i = o; i < n; ++i)
        {
            ptrdiff_t x1 = Random(W * 2) - W / 2;
            ptrdiff_t y1 = Random(H * 2) - H / 2;
            ptrdiff_t x2 = i % m == 0 ? x1 : Random(W * 2) - W / 2;
            ptrdiff_t y2 = i % m == 1 ? y1 : Random(H * 2) - H / 2;
            Simd::DrawLine(image, x1, y1, x2, y2, uint8_t(i), Random(w) + 1);
        }

        image.Save("lines.pgm");

        return true;
    }

    bool DrawRectangleSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 256, w = 3;

        for (size_t i = o; i < n; i += 5)
        {
            ptrdiff_t x1 = Random(W * 5 / 4) - W / 8;
            ptrdiff_t y1 = Random(H * 5 / 4) - H / 8;
            ptrdiff_t x2 = Random(W * 5 / 4) - W / 8;
            ptrdiff_t y2 = Random(H * 5 / 4) - H / 8;

            Simd::DrawRectangle(image, Rect(std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2)), uint8_t(i), Random(w) + 1);
        }

        Simd::DrawRectangle(image, Point(W / 20, H / 20), Point(W / 10, H / 10), uint8_t(64), 2);
        Simd::DrawRectangle(image, W / 25, H / 25, W / 15, H / 15, uint8_t(96), 3);

        image.Save("rectangles.pgm");

        return true;
    }

    bool DrawFilledRectangleSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 256;

        for (size_t i = o; i < n; i += 25)
        {
            ptrdiff_t x1 = Random(W * 5 / 4) - W / 8;
            ptrdiff_t y1 = Random(H * 5 / 4) - H / 8;
            ptrdiff_t x2 = Random(W * 5 / 4) - W / 8;
            ptrdiff_t y2 = Random(H * 5 / 4) - H / 8;

            Simd::DrawFilledRectangle(image, Rect(std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2)), uint8_t(i));
        }

        image.Save("filled_rectangles.pgm");

        return true;
    }

    bool DrawPolygonSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 255, s = 10, w = 3;

        for (size_t i = o; i < n; i += 25)
        {
            Points polygon(3 + Random(s));
            ptrdiff_t x0 = Random(W) - W / 8;
            ptrdiff_t y0 = Random(H) - H / 8;
            for (size_t j = 0; j < polygon.size(); ++j)
            {
                ptrdiff_t x = x0 + Random(W / 4);
                ptrdiff_t y = y0 + Random(H / 4);
                polygon[j] = Point(x, y);
            }
            Simd::DrawPolygon(image, polygon, uint8_t(i), Random(w) + 1);
        }

        image.Save("polygons.pgm");

        return true;
    }

    bool DrawFilledPolygonSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 255, s = 10;

        for (size_t i = o; i < n; i += 25)
        {
            Points polygon(3 + Random(s));
            ptrdiff_t x0 = Random(W) - W / 8;
            ptrdiff_t y0 = Random(H) - H / 8;
            for (size_t j = 0; j < polygon.size(); ++j)
            {
                ptrdiff_t x = x0 + Random(W / 4);
                ptrdiff_t y = y0 + Random(H / 4);
                polygon[j] = Point(x, y);
            }
            Simd::DrawFilledPolygon(image, polygon, uint8_t(i));
        }

        image.Save("filled_polygons.pgm");

        return true;
    }

    bool DrawEllipseSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 255, s = 10, w = 3;

        Point c, a;
        for (size_t i = o; i < n; i += 25)
        {
            c.x = Random(W * 5 / 4) - W / 8;
            c.y = Random(H * 5 / 4) - H / 8;
            a.x = Random(W / 4);
            a.y = Random(H / 4);
            Simd::DrawEllipse(image, c, a, Random(s)*M_PI / s, uint8_t(i), Random(w) + 1);
        }

        image.Save("ellipses.pgm");

        return true;
    }

    bool DrawCircleSpecialTest(const Options & options)
    {
        View image(W, H, View::Gray8);

        Simd::Fill(image, 0);

        const size_t o = 55, n = 255, w = 3;

        Point c;
        for (size_t i = o; i < n; i += 25)
        {
            c.x = Random(W * 5 / 4) - W / 8;
            c.y = Random(H * 5 / 4) - H / 8;
            ptrdiff_t r = Random(H / 4);
            Simd::DrawCircle(image, c, r, uint8_t(i), Random(w) + 1);
        }

        image.Save("circles.pgm");

        return true;
    }
}
