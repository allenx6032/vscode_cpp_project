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
#include "SimdSynetMergedConvolution32f.h"
#include "SimdSynetConvolution32fCommon.h"
#include "SimdUpdate.h"
#include "SimdCpu.h"

namespace Simd
{
#if defined(SIMD_SSE41_ENABLE) && defined(SIMD_SYNET_ENABLE) 
	namespace Sse41
	{
		template<SimdConvolutionActivationType type> SIMD_INLINE void InputConvolution1x1_2x6(const float* src0, size_t srcC,
			const float* weight, const __m128* bias, const __m128* params, float* dst0, float* dst1)
		{
			__m128 d00, d01, d10, d11, d20, d21, d30, d31, d40, d41, d50, d51, s0, w0, w1;
			d00 = bias[0], d01 = bias[1];
			d10 = bias[0], d11 = bias[1];
			d20 = bias[0], d21 = bias[1];
			d30 = bias[0], d31 = bias[1];
			d40 = bias[0], d41 = bias[1];
			d50 = bias[0], d51 = bias[1];
			const float* src1 = src0 + 1 * srcC;
			const float* src2 = src0 + 2 * srcC;
			const float* src3 = src0 + 3 * srcC;
			const float* src4 = src0 + 4 * srcC;
			const float* src5 = src0 + 5 * srcC;
			for (size_t sc = 0; sc < srcC; ++sc)
			{
				w0 = _mm_loadu_ps(weight + 0);
				w1 = _mm_loadu_ps(weight + F);
				s0 = _mm_set1_ps(src0[sc]);
				d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
				d01 = _mm_add_ps(_mm_mul_ps(s0, w1), d01);
				s0 = _mm_set1_ps(src1[sc]);
				d10 = _mm_add_ps(_mm_mul_ps(s0, w0), d10);
				d11 = _mm_add_ps(_mm_mul_ps(s0, w1), d11);
				s0 = _mm_set1_ps(src2[sc]);
				d20 = _mm_add_ps(_mm_mul_ps(s0, w0), d20);
				d21 = _mm_add_ps(_mm_mul_ps(s0, w1), d21);
				s0 = _mm_set1_ps(src3[sc]);
				d30 = _mm_add_ps(_mm_mul_ps(s0, w0), d30);
				d31 = _mm_add_ps(_mm_mul_ps(s0, w1), d31);
				s0 = _mm_set1_ps(src4[sc]);
				d40 = _mm_add_ps(_mm_mul_ps(s0, w0), d40);
				d41 = _mm_add_ps(_mm_mul_ps(s0, w1), d41);
				s0 = _mm_set1_ps(src5[sc]);
				d50 = _mm_add_ps(_mm_mul_ps(s0, w0), d50);
				d51 = _mm_add_ps(_mm_mul_ps(s0, w1), d51);
				weight += DF;
			}
			_mm_storeu_ps(dst0 + 0 * F, Activate<type>(d00, params, 0));
			_mm_storeu_ps(dst1 + 0 * F, Activate<type>(d01, params, 1));
			_mm_storeu_ps(dst0 + 1 * F, Activate<type>(d10, params, 0));
			_mm_storeu_ps(dst1 + 1 * F, Activate<type>(d11, params, 1));
			_mm_storeu_ps(dst0 + 2 * F, Activate<type>(d20, params, 0));
			_mm_storeu_ps(dst1 + 2 * F, Activate<type>(d21, params, 1));
			_mm_storeu_ps(dst0 + 3 * F, Activate<type>(d30, params, 0));
			_mm_storeu_ps(dst1 + 3 * F, Activate<type>(d31, params, 1));
			_mm_storeu_ps(dst0 + 4 * F, Activate<type>(d40, params, 0));
			_mm_storeu_ps(dst1 + 4 * F, Activate<type>(d41, params, 1));
			_mm_storeu_ps(dst0 + 5 * F, Activate<type>(d50, params, 0));
			_mm_storeu_ps(dst1 + 5 * F, Activate<type>(d51, params, 1));
		}

		template<SimdConvolutionActivationType type, int M> SIMD_INLINE void InputConvolution1x1_2xM(const float* src0, size_t srcC,
			const float* weight, const __m128* bias, const __m128* params, float* dst0, float* dst1)
		{
			__m128 d00, d01, d10, d11, d20, d21, d30, d31, d40, d41, d50, d51, s0, w0, w1;
			if (M > 0) d00 = bias[0], d01 = bias[1];
			if (M > 1) d10 = bias[0], d11 = bias[1];
			if (M > 2) d20 = bias[0], d21 = bias[1];
			if (M > 3) d30 = bias[0], d31 = bias[1];
			if (M > 4) d40 = bias[0], d41 = bias[1];
			if (M > 5) d50 = bias[0], d51 = bias[1];
			const float* src1 = src0 + 1 * srcC;
			const float* src2 = src0 + 2 * srcC;
			const float* src3 = src0 + 3 * srcC;
			const float* src4 = src0 + 4 * srcC;
			const float* src5 = src0 + 5 * srcC;
			for (size_t sc = 0; sc < srcC; ++sc)
			{
				w0 = _mm_loadu_ps(weight + 0);
				w1 = _mm_loadu_ps(weight + F);
				if (M > 0) s0 = _mm_set1_ps(src0[sc]), d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00), d01 = _mm_add_ps(_mm_mul_ps(s0, w1), d01);
				if (M > 1) s0 = _mm_set1_ps(src1[sc]), d10 = _mm_add_ps(_mm_mul_ps(s0, w0), d10), d11 = _mm_add_ps(_mm_mul_ps(s0, w1), d11);
				if (M > 2) s0 = _mm_set1_ps(src2[sc]), d20 = _mm_add_ps(_mm_mul_ps(s0, w0), d20), d21 = _mm_add_ps(_mm_mul_ps(s0, w1), d21);
				if (M > 3) s0 = _mm_set1_ps(src3[sc]), d30 = _mm_add_ps(_mm_mul_ps(s0, w0), d30), d31 = _mm_add_ps(_mm_mul_ps(s0, w1), d31);
				if (M > 4) s0 = _mm_set1_ps(src4[sc]), d40 = _mm_add_ps(_mm_mul_ps(s0, w0), d40), d41 = _mm_add_ps(_mm_mul_ps(s0, w1), d41);
				if (M > 5) s0 = _mm_set1_ps(src5[sc]), d50 = _mm_add_ps(_mm_mul_ps(s0, w0), d50), d51 = _mm_add_ps(_mm_mul_ps(s0, w1), d51);
				weight += DF;
			}
			if (M > 0) _mm_storeu_ps(dst0 + 0 * F, Activate<type>(d00, params, 0)), _mm_storeu_ps(dst1 + 0 * F, Activate<type>(d01, params, 1));
			if (M > 1) _mm_storeu_ps(dst0 + 1 * F, Activate<type>(d10, params, 0)), _mm_storeu_ps(dst1 + 1 * F, Activate<type>(d11, params, 1));
			if (M > 2) _mm_storeu_ps(dst0 + 2 * F, Activate<type>(d20, params, 0)), _mm_storeu_ps(dst1 + 2 * F, Activate<type>(d21, params, 1));
			if (M > 3) _mm_storeu_ps(dst0 + 3 * F, Activate<type>(d30, params, 0)), _mm_storeu_ps(dst1 + 3 * F, Activate<type>(d31, params, 1));
			if (M > 4) _mm_storeu_ps(dst0 + 4 * F, Activate<type>(d40, params, 0)), _mm_storeu_ps(dst1 + 4 * F, Activate<type>(d41, params, 1));
			if (M > 5) _mm_storeu_ps(dst0 + 5 * F, Activate<type>(d50, params, 0)), _mm_storeu_ps(dst1 + 5 * F, Activate<type>(d51, params, 1));
		}

		typedef void(*InputConvolution1x1_2xM_Ptr)(const float* src0, size_t srcC, const float* weight, const __m128* bias, const __m128* params, float* dst0, float* dst1);

		template<SimdConvolutionActivationType type> InputConvolution1x1_2xM_Ptr GetInputConvolution1x1_2xM(size_t M)
		{
			switch (M)
			{
			case 0: return InputConvolution1x1_2xM<type, 0>;
			case 1: return InputConvolution1x1_2xM<type, 1>;
			case 2: return InputConvolution1x1_2xM<type, 2>;
			case 3: return InputConvolution1x1_2xM<type, 3>;
			case 4: return InputConvolution1x1_2xM<type, 4>;
			case 5: return InputConvolution1x1_2xM<type, 5>;
			}
			assert(0);
			return NULL;
		}

		template<SimdConvolutionActivationType type> SIMD_INLINE void InputConvolution1x1_1x6(const float* src0, size_t srcC,
			const float* weight, const __m128* bias, const __m128* params, float* dst0)
		{
			__m128 d00, d10, d20, d30, d40, d50, s0, w0;
			d00 = bias[0];
			d10 = bias[0];
			d20 = bias[0];
			d30 = bias[0];
			d40 = bias[0];
			d50 = bias[0];
			const float* src1 = src0 + 1 * srcC;
			const float* src2 = src0 + 2 * srcC;
			const float* src3 = src0 + 3 * srcC;
			const float* src4 = src0 + 4 * srcC;
			const float* src5 = src0 + 5 * srcC;
			for (size_t sc = 0; sc < srcC; ++sc)
			{
				w0 = _mm_loadu_ps(weight + 0);
				s0 = _mm_set1_ps(src0[sc]);
				d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
				s0 = _mm_set1_ps(src1[sc]);
				d10 = _mm_add_ps(_mm_mul_ps(s0, w0), d10);
				s0 = _mm_set1_ps(src2[sc]);
				d20 = _mm_add_ps(_mm_mul_ps(s0, w0), d20);
				s0 = _mm_set1_ps(src3[sc]);
				d30 = _mm_add_ps(_mm_mul_ps(s0, w0), d30);
				s0 = _mm_set1_ps(src4[sc]);
				d40 = _mm_add_ps(_mm_mul_ps(s0, w0), d40);
				s0 = _mm_set1_ps(src5[sc]);
				d50 = _mm_add_ps(_mm_mul_ps(s0, w0), d50);
				weight += DF;
			}
			_mm_storeu_ps(dst0 + 0 * F, Activate<type>(d00, params, 0));
			_mm_storeu_ps(dst0 + 1 * F, Activate<type>(d10, params, 0));
			_mm_storeu_ps(dst0 + 2 * F, Activate<type>(d20, params, 0));
			_mm_storeu_ps(dst0 + 3 * F, Activate<type>(d30, params, 0));
			_mm_storeu_ps(dst0 + 4 * F, Activate<type>(d40, params, 0));
			_mm_storeu_ps(dst0 + 5 * F, Activate<type>(d50, params, 0));
		}

		template<SimdConvolutionActivationType type, int M> SIMD_INLINE void InputConvolution1x1_1xM(const float* src0, size_t srcC,
			const float* weight, const __m128* bias, const __m128* params, float* dst0)
		{
			__m128 d00, d10, d20, d30, d40, d50, s0, w0;
			if (M > 0) d00 = bias[0];
			if (M > 1) d10 = bias[0];
			if (M > 2) d20 = bias[0];
			if (M > 3) d30 = bias[0];
			if (M > 4) d40 = bias[0];
			if (M > 5) d50 = bias[0];
			const float* src1 = src0 + 1 * srcC;
			const float* src2 = src0 + 2 * srcC;
			const float* src3 = src0 + 3 * srcC;
			const float* src4 = src0 + 4 * srcC;
			const float* src5 = src0 + 5 * srcC;
			for (size_t sc = 0; sc < srcC; ++sc)
			{
				w0 = _mm_loadu_ps(weight + 0);
				if (M > 0) s0 = _mm_set1_ps(src0[sc]), d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
				if (M > 1) s0 = _mm_set1_ps(src1[sc]), d10 = _mm_add_ps(_mm_mul_ps(s0, w0), d10);
				if (M > 2) s0 = _mm_set1_ps(src2[sc]), d20 = _mm_add_ps(_mm_mul_ps(s0, w0), d20);
				if (M > 3) s0 = _mm_set1_ps(src3[sc]), d30 = _mm_add_ps(_mm_mul_ps(s0, w0), d30);
				if (M > 4) s0 = _mm_set1_ps(src4[sc]), d40 = _mm_add_ps(_mm_mul_ps(s0, w0), d40);
				if (M > 5) s0 = _mm_set1_ps(src5[sc]), d50 = _mm_add_ps(_mm_mul_ps(s0, w0), d50);
				weight += DF;
			}
			if (M > 0) _mm_storeu_ps(dst0 + 0 * F, Activate<type>(d00, params, 0));
			if (M > 1) _mm_storeu_ps(dst0 + 1 * F, Activate<type>(d10, params, 0));
			if (M > 2) _mm_storeu_ps(dst0 + 2 * F, Activate<type>(d20, params, 0));
			if (M > 3) _mm_storeu_ps(dst0 + 3 * F, Activate<type>(d30, params, 0));
			if (M > 4) _mm_storeu_ps(dst0 + 4 * F, Activate<type>(d40, params, 0));
			if (M > 5) _mm_storeu_ps(dst0 + 5 * F, Activate<type>(d50, params, 0));
		}

		typedef void(*InputConvolution1x1_1xM_Ptr)(const float* src0, size_t srcC, const float* weight, const __m128* bias, const __m128* params, float* dst0);

		template<SimdConvolutionActivationType type> InputConvolution1x1_1xM_Ptr GetInputConvolution1x1_1xM(size_t M)
		{
			switch (M)
			{
			case 0: return InputConvolution1x1_1xM<type, 0>;
			case 1: return InputConvolution1x1_1xM<type, 1>;
			case 2: return InputConvolution1x1_1xM<type, 2>;
			case 3: return InputConvolution1x1_1xM<type, 3>;
			case 4: return InputConvolution1x1_1xM<type, 4>;
			case 5: return InputConvolution1x1_1xM<type, 5>;
			}
			assert(0);
			return NULL;
		}

		template<SimdConvolutionActivationType type> void InputConvolution1x1(const float* src, const SimdConvolutionParameters& p,
			size_t dstC, size_t yBeg, size_t yEnd, const size_t bufH[2], const float* weight, const float* bias, const float* params, float* dst, int first)
		{
			size_t srcH = p.srcH, srcW = p.srcW, srcC = p.srcC, dstW = p.dstW;
			size_t dstM = (bufH[0] - 1), dstS = bufH[0] * dstW * F;
			size_t dstCDF = AlignLo(dstC, DF);
			__m128 _params[2], _bias[2];
			_params[0] = _mm_set1_ps(params[0]);
			if (type == SimdConvolutionActivationRestrictRange ||
				type == SimdConvolutionActivationHswish ||
				type == SimdConvolutionActivationHardSigmoid)
				_params[1] = _mm_set1_ps(params[1]);
			size_t yInt = Simd::Max(yBeg, yEnd & (~dstM)), nBeg = yBeg * dstW, nInt = yInt * dstW, nEnd = yEnd * dstW;
			size_t nInt6 = AlignLoAny(nInt - nBeg, 6) + nBeg, nEnd6 = AlignLoAny(nEnd - nInt, 6) + nInt, nIntTail = nInt - nInt6, nEndTail = nEnd - nEnd6;
			InputConvolution1x1_2xM_Ptr tailInt_2 = GetInputConvolution1x1_2xM<type>(nIntTail);
			InputConvolution1x1_2xM_Ptr tailEnd_2 = GetInputConvolution1x1_2xM<type>(nEndTail);

			size_t dc = 0;
			for (; dc < dstC; dc += DF)
			{
				_bias[0] = bias ? _mm_loadu_ps(bias + dc + 0) : _mm_setzero_ps();
				_bias[1] = bias ? _mm_loadu_ps(bias + dc + F) : _mm_setzero_ps();
				if (type == ::SimdConvolutionActivationPrelu)
				{
					_params[0] = _mm_loadu_ps(params + dc + 0);
					_params[1] = _mm_loadu_ps(params + dc + F);
				}
				const float* pS = src + yBeg * srcW * srcC;
				const float* pW = weight + dc * srcC;
				float* pD = dst + (dc / F) * dstS;
				float* dst0 = pD + (yBeg & dstM) * dstW * F;
				float* dst1 = pD + (yInt & dstM) * dstW * F;
				size_t dn = nBeg;
				if (dstC - dc > F)
				{
					for (; dn < nInt6; dn += 6, pS += 6 * srcC, dst0 += 6 * F)
						InputConvolution1x1_2x6<type>(pS, srcC, pW, _bias, _params, dst0, dst0 + dstS);
					if (nIntTail)
						tailInt_2(pS, srcC, pW, _bias, _params, dst0, dst0 + dstS), pS += nIntTail * srcC, dn += nIntTail;
					for (; dn < nEnd6; dn += 6, pS += 6 * srcC, dst1 += 6 * F)
						InputConvolution1x1_2x6<type>(pS, srcC, pW, _bias, _params, dst1, dst1 + dstS);
					if (nEndTail)
						tailEnd_2(pS, srcC, pW, _bias, _params, dst1, dst1 + dstS), pS += nEndTail * srcC, dn += nEndTail;
				}
				else
				{
					InputConvolution1x1_1xM_Ptr tailInt_1 = GetInputConvolution1x1_1xM<type>(nIntTail);
					InputConvolution1x1_1xM_Ptr tailEnd_1 = GetInputConvolution1x1_1xM<type>(nEndTail);
					for (; dn < nInt6; dn += 6, pS += 6 * srcC, dst0 += 6 * F)
						InputConvolution1x1_1x6<type>(pS, srcC, pW, _bias, _params, dst0);
					if (nIntTail)
						tailInt_1(pS, srcC, pW, _bias, _params, dst0), pS += nIntTail * srcC, dn += nIntTail;
					for (; dn < nEnd6; dn += 6, pS += 6 * srcC, dst1 += 6 * F)
						InputConvolution1x1_1x6<type>(pS, srcC, pW, _bias, _params, dst1);
					if (nEndTail)
						tailEnd_1(pS, srcC, pW, _bias, _params, dst1), pS += nEndTail * srcC, dn += nEndTail;
				}
			}
		}

		//---------------------------------------------------------------------

		template<SimdConvolutionActivationType type> SIMD_INLINE void InputConvolution_2x1(const float* src0, const SimdConvolutionParameters& p,
			size_t kH, size_t kW, const float* weight, const __m128* bias, const __m128* params, float* dst0, float* dst1)
		{
			__m128 d00, d01, s0, w0, w1;
			d00 = bias[0];
			d01 = bias[1];
			size_t size = kW * p.srcC, tail = DF * (p.kernelX - kW) * p.srcC, stride = p.srcW * p.srcC;
			for (size_t ky = 0; ky < kH; ++ky)
			{
				for (size_t i = 0; i < size; ++i)
				{
					w0 = _mm_loadu_ps(weight + 0);
					w1 = _mm_loadu_ps(weight + F);
					s0 = _mm_set1_ps(src0[i]);
					d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
					d01 = _mm_add_ps(_mm_mul_ps(s0, w1), d01);
					weight += DF;
				}
				weight += tail;
				src0 += stride;
			}
			_mm_storeu_ps(dst0, Activate<type>(d00, params, 0));
			_mm_storeu_ps(dst1, Activate<type>(d01, params, 1));
		}

		template<SimdConvolutionActivationType type> SIMD_INLINE void InputConvolution_1x1(const float* src0, const SimdConvolutionParameters& p,
			size_t kH, size_t kW, const float* weight, const __m128* bias, const __m128* params, float* dst0)
		{
			__m128 d00, s0, w0;
			d00 = bias[0];
			size_t size = kW * p.srcC, tail = DF * (p.kernelX - kW) * p.srcC, stride = p.srcW * p.srcC;
			for (size_t ky = 0; ky < kH; ++ky)
			{
				for (size_t i = 0; i < size; ++i)
				{
					w0 = _mm_loadu_ps(weight + 0);
					s0 = _mm_set1_ps(src0[i]);
					d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
					weight += DF;
				}
				weight += tail;
				src0 += stride;
			}
			_mm_storeu_ps(dst0, Activate<type>(d00, params, 0));
		}

		template<SimdConvolutionActivationType type> SIMD_INLINE void InputConvolution_2x6(const float* src0, const SimdConvolutionParameters& p,
			size_t kH, size_t kW, const float* weight, const __m128* bias, const __m128* params, float* dst0, float* dst1)
		{
			__m128 d00, d01, d10, d11, d20, d21, d30, d31, d40, d41, d50, d51, s0, w0, w1;
			d00 = bias[0], d01 = bias[1];
			d10 = bias[0], d11 = bias[1];
			d20 = bias[0], d21 = bias[1];
			d30 = bias[0], d31 = bias[1];
			d40 = bias[0], d41 = bias[1];
			d50 = bias[0], d51 = bias[1];
			size_t size = kW * p.srcC, tail = DF * (p.kernelX - kW) * p.srcC, stride = p.srcW * p.srcC, step = p.srcC * p.strideX;
			const float* src1 = src0 + 1 * step;
			const float* src2 = src0 + 2 * step;
			const float* src3 = src0 + 3 * step;
			const float* src4 = src0 + 4 * step;
			const float* src5 = src0 + 5 * step;
			for (size_t ky = 0; ky < kH; ++ky)
			{
				size_t offset = ky * stride;
				for (size_t end = offset + size; offset < end; ++offset)
				{
					w0 = _mm_loadu_ps(weight + 0);
					w1 = _mm_loadu_ps(weight + F);
					s0 = _mm_set1_ps(src0[offset]);
					d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
					d01 = _mm_add_ps(_mm_mul_ps(s0, w1), d01);
					s0 = _mm_set1_ps(src1[offset]);
					d10 = _mm_add_ps(_mm_mul_ps(s0, w0), d10);
					d11 = _mm_add_ps(_mm_mul_ps(s0, w1), d11);
					s0 = _mm_set1_ps(src2[offset]);
					d20 = _mm_add_ps(_mm_mul_ps(s0, w0), d20);
					d21 = _mm_add_ps(_mm_mul_ps(s0, w1), d21);
					s0 = _mm_set1_ps(src3[offset]);
					d30 = _mm_add_ps(_mm_mul_ps(s0, w0), d30);
					d31 = _mm_add_ps(_mm_mul_ps(s0, w1), d31);
					s0 = _mm_set1_ps(src4[offset]);
					d40 = _mm_add_ps(_mm_mul_ps(s0, w0), d40);
					d41 = _mm_add_ps(_mm_mul_ps(s0, w1), d41);
					s0 = _mm_set1_ps(src5[offset]);
					d50 = _mm_add_ps(_mm_mul_ps(s0, w0), d50);
					d51 = _mm_add_ps(_mm_mul_ps(s0, w1), d51);
					weight += DF;
				}
				weight += tail;
			}
			_mm_storeu_ps(dst0 + 0 * F, Activate<type>(d00, params, 0));
			_mm_storeu_ps(dst1 + 0 * F, Activate<type>(d01, params, 1));
			_mm_storeu_ps(dst0 + 1 * F, Activate<type>(d10, params, 0));
			_mm_storeu_ps(dst1 + 1 * F, Activate<type>(d11, params, 1));
			_mm_storeu_ps(dst0 + 2 * F, Activate<type>(d20, params, 0));
			_mm_storeu_ps(dst1 + 2 * F, Activate<type>(d21, params, 1));
			_mm_storeu_ps(dst0 + 3 * F, Activate<type>(d30, params, 0));
			_mm_storeu_ps(dst1 + 3 * F, Activate<type>(d31, params, 1));
			_mm_storeu_ps(dst0 + 4 * F, Activate<type>(d40, params, 0));
			_mm_storeu_ps(dst1 + 4 * F, Activate<type>(d41, params, 1));
			_mm_storeu_ps(dst0 + 5 * F, Activate<type>(d50, params, 0));
			_mm_storeu_ps(dst1 + 5 * F, Activate<type>(d51, params, 1));
		}

		template<SimdConvolutionActivationType type> SIMD_INLINE void InputConvolution_1x6(const float* src0, const SimdConvolutionParameters& p,
			size_t kH, size_t kW, const float* weight, const __m128* bias, const __m128* params, float* dst0)
		{
			__m128 d00, d10, d20, d30, d40, d50, s0, w0;
			d00 = bias[0];
			d10 = bias[0];
			d20 = bias[0];
			d30 = bias[0];
			d40 = bias[0];
			d50 = bias[0];
			size_t size = kW * p.srcC, tail = DF * (p.kernelX - kW) * p.srcC, stride = p.srcW * p.srcC, step = p.srcC * p.strideX;
			const float* src1 = src0 + 1 * step;
			const float* src2 = src0 + 2 * step;
			const float* src3 = src0 + 3 * step;
			const float* src4 = src0 + 4 * step;
			const float* src5 = src0 + 5 * step;
			for (size_t ky = 0; ky < kH; ++ky)
			{
				size_t offset = ky * stride;
				for (size_t end = offset + size; offset < end; ++offset)
				{
					w0 = _mm_loadu_ps(weight + 0);
					s0 = _mm_set1_ps(src0[offset]);
					d00 = _mm_add_ps(_mm_mul_ps(s0, w0), d00);
					s0 = _mm_set1_ps(src1[offset]);
					d10 = _mm_add_ps(_mm_mul_ps(s0, w0), d10);
					s0 = _mm_set1_ps(src2[offset]);
					d20 = _mm_add_ps(_mm_mul_ps(s0, w0), d20);
					s0 = _mm_set1_ps(src3[offset]);
					d30 = _mm_add_ps(_mm_mul_ps(s0, w0), d30);
					s0 = _mm_set1_ps(src4[offset]);
					d40 = _mm_add_ps(_mm_mul_ps(s0, w0), d40);
					s0 = _mm_set1_ps(src5[offset]);
					d50 = _mm_add_ps(_mm_mul_ps(s0, w0), d50);
					weight += DF;
				}
				weight += tail;
			}
			_mm_storeu_ps(dst0 + 0 * F, Activate<type>(d00, params, 0));
			_mm_storeu_ps(dst0 + 1 * F, Activate<type>(d10, params, 0));
			_mm_storeu_ps(dst0 + 2 * F, Activate<type>(d20, params, 0));
			_mm_storeu_ps(dst0 + 3 * F, Activate<type>(d30, params, 0));
			_mm_storeu_ps(dst0 + 4 * F, Activate<type>(d40, params, 0));
			_mm_storeu_ps(dst0 + 5 * F, Activate<type>(d50, params, 0));
		}

		template<SimdConvolutionActivationType type> void InputConvolution(const float* src, const SimdConvolutionParameters& p,
			size_t dstC, size_t yBeg, size_t yEnd, const size_t bufH[2], const float* weight, const float* bias, const float* params, float* dst, int first)
		{
			size_t srcH = p.srcH, srcW = p.srcW, srcC = p.srcC, dstW = p.dstW;
			size_t kernelY = p.kernelY, kernelX = p.kernelX, strideY = p.strideY, strideX = p.strideX;
			size_t dstM = (bufH[0] - 1), dstS = bufH[0] * dstW * F;
			size_t dstCDF = AlignLo(dstC, DF);
			if (dstC - F > dstCDF)
				dstCDF += DF;

			size_t noseH = p.padY, noseW = p.padX;
			size_t bodyH = p.srcH - p.kernelY + 1 + noseH, bodyW = p.srcW - p.kernelX + 1 + noseW;
			size_t bodyW6 = AlignLoAny(bodyW - noseW, 6 * p.strideX) + noseW;
			size_t tailH = bodyH + p.padH, tailW = bodyW + p.padW;
			size_t wS = p.srcC * p.dstC;
			size_t kY = p.kernelY - noseH, kX = p.kernelX - noseW, kH = bodyH + p.kernelY - 1, kW = bodyW + p.kernelX - 1;

			__m128 _params[2], _bias[2];
			_params[0] = _mm_set1_ps(params[0]);
			if (type == SimdConvolutionActivationRestrictRange ||
				type == SimdConvolutionActivationHswish ||
				type == SimdConvolutionActivationHardSigmoid)
				_params[1] = _mm_set1_ps(params[1]);

			size_t dc = 0;
			for (; dc < dstCDF; dc += DF)
			{
				_bias[0] = bias ? _mm_loadu_ps(bias + dc + 0) : _mm_setzero_ps();
				_bias[1] = bias ? _mm_loadu_ps(bias + dc + F) : _mm_setzero_ps();
				if (type == ::SimdConvolutionActivationPrelu)
				{
					_params[0] = _mm_loadu_ps(params + dc + 0);
					_params[1] = _mm_loadu_ps(params + dc + F);
				}
				size_t dy = yBeg, sy = dy * strideY;
				for (; sy < noseH && dy < yEnd; sy += strideY, dy++)
				{
					float* dst0 = dst + (dy & dstM) * dstW * F + (dc / F) * dstS, * dst1 = dst0 + dstS;
					size_t sx = 0;
					const float* s = src;
					const float* w = weight + (noseH - sy) * kernelX * DF * srcC;
					for (; sx < noseW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s, p, kY + sy, kX + sx, w + (noseW - sx) * srcC * DF, _bias, _params, dst0, dst1);
					for (; sx < bodyW6; sx += 6 * strideX, dst0 += 6 * F, dst1 += 6 * F)
						InputConvolution_2x6<type>(s + (sx - noseW) * srcC, p, kY + sy, kernelX, w, _bias, _params, dst0, dst1);
					for (; sx < bodyW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s + (sx - noseW) * srcC, p, kY + sy, kernelX, w, _bias, _params, dst0, dst1);
					for (; sx < tailW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s + (sx - noseW) * srcC, p, kY + sy, kW - sx, w, _bias, _params, dst0, dst1);
				}
				for (; sy < bodyH && dy < yEnd; sy += strideY, dy++)
				{
					float* dst0 = dst + (dy & dstM) * dstW * F + (dc / F) * dstS, * dst1 = dst0 + dstS;
					size_t sx = 0;
					const float* s = src + (sy - noseH) * srcW * srcC;
					const float* w = weight;
					for (; sx < noseW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s, p, kernelY, kX + sx, w + (noseW - sx) * srcC * DF, _bias, _params, dst0, dst1);
					for (; sx < bodyW6; sx += 6 * strideX, dst0 += 6 * F, dst1 += 6 * F)
						InputConvolution_2x6<type>(s + (sx - noseW) * srcC, p, kernelY, kernelX, w, _bias, _params, dst0, dst1);
					for (; sx < bodyW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s + (sx - noseW) * srcC, p, kernelY, kernelX, w, _bias, _params, dst0, dst1);
					for (; sx < tailW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s + (sx - noseW) * srcC, p, kernelY, kW - sx, w, _bias, _params, dst0, dst1);
				}
				for (; sy < tailH && dy < yEnd; sy += strideY, dy++)
				{
					float* dst0 = dst + (dy & dstM) * dstW * F + (dc / F) * dstS, * dst1 = dst0 + dstS;
					size_t sx = 0;
					const float* s = src + (sy - noseH) * srcW * srcC;
					const float* w = weight;
					for (; sx < noseW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s, p, kH - sy, kX + sx, w + (noseW - sx) * srcC * DF, _bias, _params, dst0, dst1);
					for (; sx < bodyW6; sx += 6 * strideX, dst0 += 6 * F, dst1 += 6 * F)
						InputConvolution_2x6<type>(s + (sx - noseW) * srcC, p, kH - sy, kernelX, w, _bias, _params, dst0, dst1);
					for (; sx < bodyW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s + (sx - noseW) * srcC, p, kH - sy, kernelX, w, _bias, _params, dst0, dst1);
					for (; sx < tailW; sx += strideX, dst0 += F, dst1 += F)
						InputConvolution_2x1<type>(s + (sx - noseW) * srcC, p, kH - sy, kW - sx, w, _bias, _params, dst0, dst1);
				}
				weight += kernelY * kernelX * srcC * DF;
			}
			if (dc < dstC)
			{
				_bias[0] = bias ? _mm_loadu_ps(bias + dc) : _mm_setzero_ps();
				if (type == ::SimdConvolutionActivationPrelu)
					_params[0] = _mm_loadu_ps(params + dc);
				size_t dy = yBeg, sy = dy * strideY;
				for (; sy < noseH && dy < yEnd; sy += strideY, dy++)
				{
					float* dst0 = dst + (dy & dstM) * dstW * F + (dc / F) * dstS;
					size_t sx = 0;
					const float* s = src;
					const float* w = weight + (noseH - sy) * kernelX * DF * srcC;
					for (; sx < noseW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s, p, kY + sy, kX + sx, w + (noseW - sx) * srcC * DF, _bias, _params, dst0);
					for (; sx < bodyW6; sx += 6 * strideX, dst0 += 6 * F)
						InputConvolution_1x6<type>(s + (sx - noseW) * srcC, p, kY + sy, kernelX, w, _bias, _params, dst0);
					for (; sx < bodyW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s + (sx - noseW) * srcC, p, kY + sy, kernelX, w, _bias, _params, dst0);
					for (; sx < tailW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s + (sx - noseW) * srcC, p, kY + sy, kW - sx, w, _bias, _params, dst0);
				}
				for (; sy < bodyH && dy < yEnd; sy += strideY, dy++)
				{
					float* dst0 = dst + (dy & dstM) * dstW * F + (dc / F) * dstS;
					size_t sx = 0;
					const float* s = src + (sy - noseH) * srcW * srcC;
					const float* w = weight;
					for (; sx < noseW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s, p, kernelY, kX + sx, w + (noseW - sx) * srcC * DF, _bias, _params, dst0);
					for (; sx < bodyW6; sx += 6 * strideX, dst0 += 6 * F)
						InputConvolution_1x6<type>(s + (sx - noseW) * srcC, p, kernelY, kernelX, w, _bias, _params, dst0);
					for (; sx < bodyW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s + (sx - noseW) * srcC, p, kernelY, kernelX, w, _bias, _params, dst0);
					for (; sx < tailW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s + (sx - noseW) * srcC, p, kernelY, kW - sx, w, _bias, _params, dst0);
				}
				for (; sy < tailH && dy < yEnd; sy += strideY, dy++)
				{
					float* dst0 = dst + (dy & dstM) * dstW * F + (dc / F) * dstS;
					size_t sx = 0;
					const float* s = src + (sy - noseH) * srcW * srcC;
					const float* w = weight;
					for (; sx < noseW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s, p, kH - sy, kX + sx, w + (noseW - sx) * srcC * DF, _bias, _params, dst0);
					for (; sx < bodyW6; sx += 6 * strideX, dst0 += 6 * F)
						InputConvolution_1x6<type>(s + (sx - noseW) * srcC, p, kH - sy, kernelX, w, _bias, _params, dst0);
					for (; sx < bodyW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s + (sx - noseW) * srcC, p, kH - sy, kernelX, w, _bias, _params, dst0);
					for (; sx < tailW; sx += strideX, dst0 += F)
						InputConvolution_1x1<type>(s + (sx - noseW) * srcC, p, kH - sy, kW - sx, w, _bias, _params, dst0);
				}
			}
		}

		//-------------------------------------------------------------------------------------------------------

		template <SimdConvolutionActivationType type> void SetInput(const ConvParam& p, Base::SynetMergedConvolution32f::ConvolutionPtr* convolution)
		{
			if (p.kernelY == 1 && p.strideY == 1)
				convolution[0] = InputConvolution1x1<type>;
			else
				convolution[0] = InputConvolution<type>;
		}

		void SetInput(const ConvParam& p, Base::SynetMergedConvolution32f::ConvolutionPtr* convolution)
		{
			switch (p.activation)
			{
			case SimdConvolutionActivationIdentity: SetInput<SimdConvolutionActivationRestrictRange>(p, convolution); break;
			case SimdConvolutionActivationRelu: SetInput<SimdConvolutionActivationRestrictRange>(p, convolution); break;
			case SimdConvolutionActivationLeakyRelu: SetInput<SimdConvolutionActivationPrelu>(p, convolution); break;
			case SimdConvolutionActivationRestrictRange: SetInput<SimdConvolutionActivationRestrictRange>(p, convolution); break;
			case SimdConvolutionActivationPrelu: SetInput<SimdConvolutionActivationPrelu>(p, convolution); break;
			case SimdConvolutionActivationElu: SetInput<SimdConvolutionActivationElu>(p, convolution); break;
			case SimdConvolutionActivationHswish: SetInput<SimdConvolutionActivationHswish>(p, convolution); break;
			case SimdConvolutionActivationMish: SetInput<SimdConvolutionActivationMish>(p, convolution); break;
			case SimdConvolutionActivationHardSigmoid: SetInput<SimdConvolutionActivationHardSigmoid>(p, convolution); break;
			case SimdConvolutionActivationSwish: SetInput<SimdConvolutionActivationSwish>(p, convolution); break;
			case SimdConvolutionActivationGelu: SetInput<SimdConvolutionActivationGelu>(p, convolution); break;
			default: assert(0);
			}
		}
	}
#endif
}
