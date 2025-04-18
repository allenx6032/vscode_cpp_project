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
#ifndef __TestConfig_h__
#define __TestConfig_h__

#if defined(NDEBUG)
#define TEST_PERFORMANCE_TEST_ENABLE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <sstream>
#include <limits>
#include <iomanip>
#include <memory>
#include <exception>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <cstring>

#define SIMD_STATIC
#include "SimdConfig.h"
#include "SimdLib.hpp"

#include "SimdConst.h"
#include "SimdMath.h"
#include "SimdEnable.h"
#include "SimdMemory.h"
#include "SimdBase.h"
#include "SimdSse41.h"
#include "SimdAvx2.h"
#include "SimdAvx512bw.h"
#include "SimdAvx512vnni.h"
#include "SimdAmxBf16.h"
#include "SimdNeon.h"

namespace Test
{
    template <class T> class Tensor;

    typedef std::string String;
    typedef std::vector<String> Strings;
    typedef Simd::View<Simd::Allocator> View;
    typedef std::vector<View> Views;
    typedef Simd::Point<ptrdiff_t> Point;
    typedef std::vector<Point> Points;
    typedef Point Size;
    typedef Simd::Rectangle<ptrdiff_t> Rect;
    typedef uint32_t Histogram[Simd::HISTOGRAM_SIZE];
    typedef std::vector<int> Ints;
    typedef std::vector<uint32_t> Sums;
    typedef std::vector<uint64_t> Sums64;
    typedef std::vector<float, Simd::Allocator<float> > Buffer32f;
    typedef std::vector<uint8_t> Buffer8u;
    typedef std::vector<float*> FloatPtrs;
    typedef std::vector<uint16_t*> UInt16Ptrs;
    typedef std::vector<uint8_t*> UInt8Ptrs;
    typedef std::vector<size_t> Shape;
    typedef Tensor<float> Tensor32f;
    typedef Tensor<uint8_t> Tensor8u;
    typedef Tensor<int8_t> Tensor8i;
    typedef Tensor<int32_t> Tensor32i;
    typedef Tensor<uint16_t> Tensor16u;

    const int E = 10;
    const int O = 9;

    extern double MINIMAL_TEST_EXECUTION_TIME;
    //extern double WARM_UP_TIME;

    const int DW = 48;
    const int DH = 64;

    const float EPS = 0.001f;

    extern int C;
    extern int H;    
    extern int W;

    extern uint32_t DISABLED_EXTENSIONS;

    extern String ROOT_PATH;
    extern String REAL_IMAGE;

    extern int LITTER_CPU_CACHE;

    enum DifferenceType
    {
        DifferenceAbsolute,
        DifferenceRelative,
        DifferenceBoth,
        DifferenceAny,
        DifferenceLogical,
    };

    //-------------------------------------------------------------------------------------------------

    SIMD_INLINE bool TestBase()
    {
        return (DISABLED_EXTENSIONS & 0x000000001) == 0;
    }

    SIMD_INLINE bool TestSse41()
    {
        return (DISABLED_EXTENSIONS & 0x000000002) == 0;
    }

    SIMD_INLINE bool TestAvx2()
    {
        return (DISABLED_EXTENSIONS & 0x000000004) == 0;
    }


    SIMD_INLINE bool TestAvx512bw()
    {
        return (DISABLED_EXTENSIONS & 0x000000008) == 0;
    }


    SIMD_INLINE bool TestAvx512vnni()
    {
        return (DISABLED_EXTENSIONS & 0x000000010) == 0;
    }


    SIMD_INLINE bool TestAmxBf16()
    {
        return (DISABLED_EXTENSIONS & 0x000000020) == 0;
    }


    SIMD_INLINE bool TestNeon()
    {
        return (DISABLED_EXTENSIONS & 0x000000002) == 0;
    }
}

#endif//__TestConfig_h__
