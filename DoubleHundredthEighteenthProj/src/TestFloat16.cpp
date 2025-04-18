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
#include "TestTensor.h"
#include "TestString.h"
#include "TestRandom.h"
#include "TestOptions.h"

namespace Test
{
    namespace
    {
        struct FuncSH
        {
            typedef void(*FuncPtr)(const float * src, size_t size, uint16_t * dst);

            FuncPtr func;
            String description;

            FuncSH(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, View & dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func((const float*)src.data, src.width, (uint16_t*)dst.data);
            }
        };
    }

#define FUNC_SH(function) FuncSH(function, #function)

    bool Float32ToFloat16AutoTest(size_t size, const FuncSH & f1, const FuncSH & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View src(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst1(size, 1, View::Int16, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst2(size, 1, View::Int16, NULL, TEST_ALIGN(SIMD_ALIGN));

        FillRandom32f(src, -10.0, 10.0);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, dst2));

        result = result && Compare(dst1, dst2, 1, true, 32);

        return result;
    }

    bool Float32ToFloat16AutoTest(const FuncSH & f1, const FuncSH & f2)
    {
        bool result = true;

        result = result && Float32ToFloat16AutoTest(W*H, f1, f2);
        result = result && Float32ToFloat16AutoTest(W*H - 1, f1, f2);

        return result;
    }

    bool Float32ToFloat16AutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && Float32ToFloat16AutoTest(FUNC_SH(Simd::Base::Float32ToFloat16), FUNC_SH(SimdFloat32ToFloat16));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && Float32ToFloat16AutoTest(FUNC_SH(Simd::Sse41::Float32ToFloat16), FUNC_SH(SimdFloat32ToFloat16));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && Float32ToFloat16AutoTest(FUNC_SH(Simd::Avx2::Float32ToFloat16), FUNC_SH(SimdFloat32ToFloat16));
#endif 

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && Float32ToFloat16AutoTest(FUNC_SH(Simd::Avx512bw::Float32ToFloat16), FUNC_SH(SimdFloat32ToFloat16));
#endif 

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && Float32ToFloat16AutoTest(FUNC_SH(Simd::Neon::Float32ToFloat16), FUNC_SH(SimdFloat32ToFloat16));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    namespace
    {
        struct FuncHS
        {
            typedef void(*FuncPtr)(const uint16_t * src, size_t size, float * dst);

            FuncPtr func;
            String description;

            FuncHS(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, View & dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func((const uint16_t*)src.data, src.width, (float*)dst.data);
            }
        };
    }

#define FUNC_HS(function) FuncHS(function, #function)

    bool Float16ToFloat32AutoTest(size_t size, const FuncHS & f1, const FuncHS & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View origin(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View src(size, 1, View::Int16, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst1(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));
        View dst2(size, 1, View::Float, NULL, TEST_ALIGN(SIMD_ALIGN));

        FillRandom32f(origin, -10.0, 10.0);
        ::SimdFloat32ToFloat16((const float*)origin.data, size, (uint16_t*)src.data);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 32);

        return result;
    }

    bool Float16ToFloat32AutoTest(const FuncHS & f1, const FuncHS & f2)
    {
        bool result = true;

        result = result && Float16ToFloat32AutoTest(W*H, f1, f2);
        result = result && Float16ToFloat32AutoTest(W*H - 1, f1, f2);

        return result;
    }

    bool Float16ToFloat32AutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && Float16ToFloat32AutoTest(FUNC_HS(Simd::Base::Float16ToFloat32), FUNC_HS(SimdFloat16ToFloat32));

#ifdef SIMD_SSE41_ENABLE
        if (Simd::Sse41::Enable && TestSse41())
            result = result && Float16ToFloat32AutoTest(FUNC_HS(Simd::Sse41::Float16ToFloat32), FUNC_HS(SimdFloat16ToFloat32));
#endif

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && Float16ToFloat32AutoTest(FUNC_HS(Simd::Avx2::Float16ToFloat32), FUNC_HS(SimdFloat16ToFloat32));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && Float16ToFloat32AutoTest(FUNC_HS(Simd::Avx512bw::Float16ToFloat32), FUNC_HS(SimdFloat16ToFloat32));
#endif 

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && Float16ToFloat32AutoTest(FUNC_HS(Simd::Neon::Float16ToFloat32), FUNC_HS(SimdFloat16ToFloat32));
#endif 

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    struct FuncS
    {
        typedef void(*FuncPtr)(const uint16_t * a, const uint16_t * b, size_t size, float * sum);

        FuncPtr func;
        String description;

        FuncS(const FuncPtr & f, const String & d) : func(f), description(d) {}

        void Call(const View & a, const View & b, float * sum) const
        {
            TEST_PERFORMANCE_TEST(description);
            func((const uint16_t*)a.data, (const uint16_t*)b.data, a.width, sum);
        }
    };

    typedef void(*CheckPtr)(const float * a, const float * b, size_t size, float * sum);

#define FUNC_S(function) FuncS(function, #function)

    bool DifferenceSum16fAutoTest(int size, float eps, const FuncS & f1, const FuncS & f2, CheckPtr check)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << size << "].");

        View aOrigin(size, 1, View::Float, NULL, TEST_ALIGN(size));
        FillRandom32f(aOrigin, -10.0, 10.0);
        View a(size, 1, View::Int16, NULL, TEST_ALIGN(size));
        ::SimdFloat32ToFloat16((float*)aOrigin.data, a.width, (uint16_t*)a.data);

        View bOrigin(size, 1, View::Float, NULL, TEST_ALIGN(size));
        FillRandom32f(bOrigin, -10.0, 10.0);
        View b(size, 1, View::Int16, NULL, TEST_ALIGN(size));
        ::SimdFloat32ToFloat16((float*)bOrigin.data, b.width, (uint16_t*)b.data);

        float s1, s2, s3;
        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(a, b, &s1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(a, b, &s2));

        check((float*)aOrigin.data, (float*)bOrigin.data, a.width, &s3);

        result = Compare(s1, s2, eps, true, DifferenceRelative, "s1 & s2");
        result = Compare(s2, s3, eps*2, true, DifferenceRelative, "s2 & s3");

        return result;
    }

    bool DifferenceSum16fAutoTest(float eps, const FuncS & f1, const FuncS & f2, CheckPtr check)
    {
        bool result = true;

        result = result && DifferenceSum16fAutoTest(W*H, eps, f1, f2, check);
        result = result && DifferenceSum16fAutoTest(W*H - O, eps, f1, f2, check);

        return result;
    }

    bool SquaredDifferenceSum16fAutoTest()
    {
        bool result = true;

        CheckPtr check = ::SimdSquaredDifferenceSum32f;

        if (TestBase())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Base::SquaredDifferenceSum16f), FUNC_S(SimdSquaredDifferenceSum16f), check);

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Avx2::SquaredDifferenceSum16f), FUNC_S(SimdSquaredDifferenceSum16f), check);
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Avx512bw::SquaredDifferenceSum16f), FUNC_S(SimdSquaredDifferenceSum16f), check);
#endif

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Neon::SquaredDifferenceSum16f), FUNC_S(SimdSquaredDifferenceSum16f), check);
#endif 

        return result;
    }

    bool CosineDistance16fAutoTest()
    {
        bool result = true;

        CheckPtr check = ::SimdCosineDistance32f;

        if (TestBase())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Base::CosineDistance16f), FUNC_S(SimdCosineDistance16f), check);

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Avx2::CosineDistance16f), FUNC_S(SimdCosineDistance16f), check);
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Avx512bw::CosineDistance16f), FUNC_S(SimdCosineDistance16f), check);
#endif

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && DifferenceSum16fAutoTest(EPS, FUNC_S(Simd::Neon::CosineDistance16f), FUNC_S(SimdCosineDistance16f), check);
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------
 
//#define TEST_COS_DIST_LONG_TEST

    typedef std::vector<uint16_t*> F16Ptrs;

    SIMD_INLINE void InitRandomF16(View& f16, size_t width, size_t height, float lo = -1.0f, float hi = 1.0f, int gap = 0, F16Ptrs *f16p = NULL)
    {
        View f32(width, height, View::Float, NULL, 1);
        FillRandom32f(f32, lo, hi);
        f16.Recreate(width + gap, height, View::Int16, NULL, 1);
        if (f16p)
            f16p->resize(height);
        for (size_t r = 0; r < height; r++)
        {
            uint16_t* dst = f16.Row<uint16_t>(r) + Random(gap);
            ::SimdFloat32ToFloat16(f32.Row<float>(r), width, dst);
            if(f16p)
                f16p->at(r) = dst;
        }
        TEST_ALIGN(width);
    }

    //-------------------------------------------------------------------------------------------------

    struct FuncCDP
    {
        typedef void(*FuncPtr)(size_t M, size_t N, size_t K, const uint16_t* A, const uint16_t* B, float* distances);

        FuncPtr func;
        String desc;

        FuncCDP(const FuncPtr& f, const String& d) : func(f), desc(d) {}

        void Update(size_t M, size_t N, size_t K)
        {
            desc = desc + "[" + ToString(M) + "-" + ToString(N) + "-" + ToString(K) + "]";
        }

        void Call(const View& A, const View& B, Tensor32f& D) const
        {
            TEST_PERFORMANCE_TEST(desc);
            func(A.height, B.height, A.width, (uint16_t*)A.data, (uint16_t*)B.data, D.Data());
        }
    };

#define FUNC_CDP(function) FuncCDP(function, #function)

    bool CosineDistancesMxNp16fAutoTest(size_t M, size_t N, size_t K, float eps, FuncCDP f1, FuncCDP f2)
    {
        bool result = true;

        f1.Update(M, N, K);
        f2.Update(M, N, K);

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc);

        View Ai, Bi;
        InitRandomF16(Ai, K, M, -1.0f, 1.0f, 0, NULL);
        InitRandomF16(Bi, K, N, -1.0f, 1.0f, 0, NULL);

        Tensor32f D1({ M, N, });
        Tensor32f D2({ M, N, });

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(Ai, Bi, D1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(Ai, Bi, D2));

        result = Compare(D1, D2, eps, true, 32, DifferenceAbsolute);

        return result;
    }

    bool CosineDistancesMxNp16fAutoTest(float eps, const FuncCDP& f1, const FuncCDP& f2)
    {
        bool result = true;

        //result = result && CosineDistancesMxNp16fAutoTest(128, 1024, 512, eps, f1, f2);
        //result = result && CosineDistancesMxNp16fAutoTest(1024, 128, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNp16fAutoTest(1024, 129, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNp16fAutoTest(1023, 128, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNp16fAutoTest(1023, 129, 1023, eps, f1, f2);
        result = result && CosineDistancesMxNp16fAutoTest(256, 128, 256, eps, f1, f2);
        result = result && CosineDistancesMxNp16fAutoTest(128, 128, 512, eps, f1, f2);

#if !defined(SIMD_NEON_ENABLE) && defined(TEST_COS_DIST_LONG_TEST)
        result = result && CosineDistancesMxNp16fAutoTest(10 * 1024, 128, 1024, eps, f1, f2);
        result = result && CosineDistancesMxNp16fAutoTest(1024, 10 * 128, 1024, eps, f1, f2);
#endif

        return result;
    }

    bool CosineDistancesMxNp16fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && CosineDistancesMxNp16fAutoTest(EPS, FUNC_CDP(Simd::Base::CosineDistancesMxNp16f), FUNC_CDP(SimdCosineDistancesMxNp16f));

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && CosineDistancesMxNp16fAutoTest(EPS, FUNC_CDP(Simd::Avx2::CosineDistancesMxNp16f), FUNC_CDP(SimdCosineDistancesMxNp16f));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && CosineDistancesMxNp16fAutoTest(EPS, FUNC_CDP(Simd::Avx512bw::CosineDistancesMxNp16f), FUNC_CDP(SimdCosineDistancesMxNp16f));
#endif

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && CosineDistancesMxNp16fAutoTest(EPS, FUNC_CDP(Simd::Neon::CosineDistancesMxNp16f), FUNC_CDP(SimdCosineDistancesMxNp16f));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    struct FuncCDA
    {
        typedef void(*FuncPtr)(size_t M, size_t N, size_t K, const uint16_t * const * A, const uint16_t * const * B, float * distances);

        FuncPtr func;
        String desc;

        FuncCDA(const FuncPtr & f, const String & d) : func(f), desc(d) {}

        void Update(size_t M, size_t N, size_t K)
        {
            desc = desc + "[" + ToString(M) + "-" + ToString(N) + "-" + ToString(K) + "]";
        }

        void Call(size_t K, const F16Ptrs & A, const F16Ptrs & B, Tensor32f & D) const
        {
            TEST_PERFORMANCE_TEST(desc);
            func(A.size(), B.size(), K, A.data(), B.data(), D.Data());
        }
    };

#define FUNC_CDA(function) FuncCDA(function, #function)

    bool CosineDistancesMxNa16fAutoTest(size_t M, size_t N, size_t K, float eps, FuncCDA f1, FuncCDA f2)
    {
        bool result = true;

        f1.Update(M, N, K);
        f2.Update(M, N, K);

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc);

        View Ai, Bi;
        F16Ptrs A, B;
        InitRandomF16(Ai, K, M, -1.0f, 1.0f, 1024, &A);
        InitRandomF16(Bi, K, N, -1.0f, 1.0f, 1024, &B);

        Tensor32f D1({ M, N, });
        Tensor32f D2({ M, N, });

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(K, A, B, D1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(K, A, B, D2));

        result = Compare(D1, D2, eps, true, 32, DifferenceAbsolute);

        return result;
    }

    bool CosineDistancesMxNa16fAutoTest(float eps, const FuncCDA & f1, const FuncCDA & f2)
    {
        bool result = true;

        //result = result && CosineDistancesMxNa16fAutoTest(128, 1024, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNa16fAutoTest(1024, 128, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNa16fAutoTest(1024, 129, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNa16fAutoTest(1023, 128, 1024, eps, f1, f2);
        //result = result && CosineDistancesMxNa16fAutoTest(1023, 129, 1023, eps, f1, f2);
        result = result && CosineDistancesMxNa16fAutoTest(256, 128, 256, eps, f1, f2);
        result = result && CosineDistancesMxNa16fAutoTest(128, 128, 512, eps, f1, f2);

#if !defined(SIMD_NEON_ENABLE) && defined(TEST_COS_DIST_LONG_TEST)
        result = result && CosineDistancesMxNa16fAutoTest(10*1024, 128, 1024, eps, f1, f2);
        result = result && CosineDistancesMxNa16fAutoTest(1024, 10*128, 1024, eps, f1, f2);
#endif

        return result;
    }

    bool CosineDistancesMxNa16fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && CosineDistancesMxNa16fAutoTest(EPS, FUNC_CDA(Simd::Base::CosineDistancesMxNa16f), FUNC_CDA(SimdCosineDistancesMxNa16f));

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && CosineDistancesMxNa16fAutoTest(EPS, FUNC_CDA(Simd::Avx2::CosineDistancesMxNa16f), FUNC_CDA(SimdCosineDistancesMxNa16f));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && CosineDistancesMxNa16fAutoTest(EPS, FUNC_CDA(Simd::Avx512bw::CosineDistancesMxNa16f), FUNC_CDA(SimdCosineDistancesMxNa16f));
#endif

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && CosineDistancesMxNa16fAutoTest(EPS, FUNC_CDA(Simd::Neon::CosineDistancesMxNa16f), FUNC_CDA(SimdCosineDistancesMxNa16f));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    struct FuncVNP
    {
        typedef void(*FuncPtr)(size_t N, size_t K, const uint16_t* A, float* norms);

        FuncPtr func;
        String desc;

        FuncVNP(const FuncPtr& f, const String& d) : func(f), desc(d) {}

        void Update(size_t N, size_t K)
        {
            desc = desc + "[" + ToString(N) + "-" + ToString(K) + "]";
        }

        void Call(const View& A, Tensor32f& norms) const
        {
            TEST_PERFORMANCE_TEST(desc);
            func(A.height, A.width, (uint16_t*)A.data, norms.Data());
        }
    };

#define FUNC_VNP(function) FuncVNP(function, #function)

    bool VectorNormNp16fAutoTest(size_t N, size_t K, float eps, FuncVNP f1, FuncVNP f2)
    {
        bool result = true;

        f1.Update(N, K);
        f2.Update(N, K);

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc);

        View Ai;
        InitRandomF16(Ai, K, N, -1.0f, 1.0f, 0, NULL);

        Tensor32f norms1({ N });
        Tensor32f norms2({ N });

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(Ai, norms1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(Ai, norms2));

        result = Compare(norms1, norms2, eps, true, 32, DifferenceAbsolute);

        return result;
    }

    bool VectorNormNp16fAutoTest(float eps, const FuncVNP& f1, const FuncVNP& f2)
    {
        bool result = true;

        result = result && VectorNormNp16fAutoTest(1, 1024, eps, f1, f2);
        result = result && VectorNormNp16fAutoTest(128, 512, eps, f1, f2);
        result = result && VectorNormNp16fAutoTest(129, 513, eps, f1, f2);

        return result;
    }

    bool VectorNormNp16fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && VectorNormNp16fAutoTest(EPS, FUNC_VNP(Simd::Base::VectorNormNp16f), FUNC_VNP(SimdVectorNormNp16f));

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && VectorNormNp16fAutoTest(EPS, FUNC_VNP(Simd::Avx2::VectorNormNp16f), FUNC_VNP(SimdVectorNormNp16f));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && VectorNormNp16fAutoTest(EPS, FUNC_VNP(Simd::Avx512bw::VectorNormNp16f), FUNC_VNP(SimdVectorNormNp16f));
#endif

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && VectorNormNp16fAutoTest(EPS, FUNC_VNP(Simd::Neon::VectorNormNp16f), FUNC_VNP(SimdVectorNormNp16f));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    struct FuncVNA
    {
        typedef void(*FuncPtr)(size_t N, size_t K, const uint16_t* const* A, float* norms);

        FuncPtr func;
        String desc;

        FuncVNA(const FuncPtr& f, const String& d) : func(f), desc(d) {}

        void Update(size_t N, size_t K)
        {
            desc = desc + "[" + ToString(N) + "-" + ToString(K) + "]";
        }

        void Call(size_t K, const F16Ptrs& A, Tensor32f& norms) const
        {
            TEST_PERFORMANCE_TEST(desc);
            func(A.size(), K, A.data(), norms.Data());
        }
    };

#define FUNC_VNA(function) FuncVNA(function, #function)

    bool VectorNormNa16fAutoTest(size_t N, size_t K, float eps, FuncVNA f1, FuncVNA f2)
    {
        bool result = true;

        f1.Update(N, K);
        f2.Update(N, K);

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc);

        View Ai;
        F16Ptrs A;
        InitRandomF16(Ai, K, N, -1.0f, 1.0f, 1024, &A);

        Tensor32f norms1({ N, });
        Tensor32f norms2({ N, });

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(K, A, norms1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(K, A, norms2));

        result = Compare(norms1, norms2, eps, true, 32, DifferenceAbsolute);

        return result;
    }

    bool VectorNormNa16fAutoTest(float eps, const FuncVNA& f1, const FuncVNA& f2)
    {
        bool result = true;

        result = result && VectorNormNa16fAutoTest(1, 1024, eps, f1, f2);
        result = result && VectorNormNa16fAutoTest(128, 512, eps, f1, f2);
        result = result && VectorNormNa16fAutoTest(129, 513, eps, f1, f2);

        return result;
    }

    bool VectorNormNa16fAutoTest()
    {
        bool result = true;

        if (TestBase())
            result = result && VectorNormNa16fAutoTest(EPS, FUNC_VNA(Simd::Base::VectorNormNa16f), FUNC_VNA(SimdVectorNormNa16f));

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            result = result && VectorNormNa16fAutoTest(EPS, FUNC_VNA(Simd::Avx2::VectorNormNa16f), FUNC_VNA(SimdVectorNormNa16f));
#endif

#ifdef SIMD_AVX512BW_ENABLE
        if (Simd::Avx512bw::Enable && TestAvx512bw())
            result = result && VectorNormNa16fAutoTest(EPS, FUNC_VNA(Simd::Avx512bw::VectorNormNa16f), FUNC_VNA(SimdVectorNormNa16f));
#endif

#if defined(SIMD_NEON_ENABLE) && defined(SIMD_NEON_FP16_ENABLE)
        if (Simd::Neon::Enable && TestNeon())
            result = result && VectorNormNa16fAutoTest(EPS, FUNC_VNA(Simd::Neon::VectorNormNa16f), FUNC_VNA(SimdVectorNormNa16f));
#endif

        return result;
    }

    //-------------------------------------------------------------------------------------------------

    bool CosineDistancesMxNp16fSpecialTest(const Options & options)
    {
        bool result = true;

        const size_t M = 3, N = 5, K = 512;
        const size_t MN = std::max(M, N);

        View ABf(K, MN, View::Float, NULL, TEST_ALIGN(K));
        FillRandom32f(ABf, -1.0, 1.0);
        View ABi(K, MN, View::Int16, NULL, TEST_ALIGN(K));
        ::SimdFloat32ToFloat16((float*)ABf.data, K * MN, (uint16_t*)ABi.data);

        Tensor32f D1({ M, N, }), D2({ M, N, });

        SimdCosineDistancesMxNp16f(M, N, K, (uint16_t*)ABi.data, (uint16_t*)ABi.data, D1.Data());

#ifdef SIMD_AVX2_ENABLE
        if (Simd::Avx2::Enable && TestAvx2())
            Simd::Avx2::CosineDistancesMxNp16f(M, N, K, (uint16_t*)ABi.data, (uint16_t*)ABi.data, D2.Data());
#endif

        return result;
    }
}
