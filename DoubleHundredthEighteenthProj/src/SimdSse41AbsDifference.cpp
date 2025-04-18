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
#include "SimdMemory.h"
#include "SimdStore.h"

namespace Simd
{
#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {
		void AbsDifference(const uint8_t* a, size_t aStride, const uint8_t* b, size_t bStride, uint8_t* c, size_t cStride, size_t width, size_t height)
		{
			assert(width >= A);
			size_t bodyWidth = AlignLo(width, A);
			for (size_t row = 0; row < height; ++row)
			{
				for (size_t col = 0; col < bodyWidth; col += A)
				{
					const __m128i _a = _mm_loadu_si128((__m128i*)(a + col));
					const __m128i _b = _mm_loadu_si128((__m128i*)(b + col));
					_mm_storeu_si128((__m128i*)(c + col), _mm_sub_epi8(_mm_max_epu8(_a, _b), _mm_min_epu8(_a, _b)));
				}
				if (width - bodyWidth)
				{
					const __m128i _a = _mm_loadu_si128((__m128i*)(a + width - A));
					const __m128i _b = _mm_loadu_si128((__m128i*)(b + width - A));
					_mm_storeu_si128((__m128i*)(c + width - A), _mm_sub_epi8(_mm_max_epu8(_a, _b), _mm_min_epu8(_a, _b)));
				}
				a += aStride;
				b += bStride;
				c += bStride;
			}
		}
    }
#endif
}
