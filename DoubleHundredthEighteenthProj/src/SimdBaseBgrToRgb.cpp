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
#include "SimdDefs.h"

namespace Simd
{
    namespace Base
    {
        void BgrToRgb(const uint8_t * bgr, size_t width, size_t height, size_t bgrStride, uint8_t * rgb, size_t rgbStride)
        {
            size_t size = width * 3;
            for (size_t row = 0; row < height; ++row)
            {
                for (size_t i = 0; i < size; i += 3)
                {
                    rgb[i + 0] = bgr[i + 2];
                    rgb[i + 1] = bgr[i + 1];
                    rgb[i + 2] = bgr[i + 0];
                }
                bgr += bgrStride;
                rgb += rgbStride;
            }
        }
    }
}
