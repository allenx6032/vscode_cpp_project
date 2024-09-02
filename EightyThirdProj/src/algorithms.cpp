//#include <opencv2/opencv.hpp>

#include "mat.hpp"
#include <iostream>
#include <math.h>

#include <emmintrin.h>  //sse2

//#define DEBUG
//#define TIME_MACRO

#include "algorithms.h"


#define MAGNIFY


#define __asm__ asm

_MCV_START_


inline uchar GrayShort2Uchar(short n) {
	if (n < 0) n = 0;
	if (n > 255) n = 255;
	return (uchar)n;
}

//image negtive
void NegativeImg(const Mat& img, Mat& saved_img)
{
	START_TIME;
	saved_img.release();
	saved_img.creat(img.rows_, img.cols_, img.type());
	//std::cout << img.total() << std::endl;
	for (size_t i = 0; i < img.total(); i++) {
		saved_img.data_[i] = ~img.data_[i];
	}

	PRINT_TIME("negative the picture");
}


//compression of dynamic range
void CompressGray(const Mat& img, Mat& saved_img, double c)
{
	START_TIME;

	saved_img.release();
	saved_img.creat(img.rows_, img.cols_, img.type());

	for (int i = 0; i < img.total(); i++) {
		saved_img.data_[i] = (uchar)(c * log2(1.0 + img.data_[i]));
	}

	PRINT_TIME("compress gray");
}




#if 0
inline void SmUi2M128i(__m128i* p_mn, unsigned int data_) {
	/*mn.m128i_i16[0] = p_data[0];
	mn.m128i_i16[1] = p_data[1];
	mn.m128i_i16[2] = mn.m128i_i16[1];
	mn.m128i_i16[3] = p_data[2];
	mn.m128i_i16[4] = mn.m128i_i16[3];
	mn.m128i_i16[5] = p_data[3];*/
	__asm {
		mov eax, data_;
		mov esi, p_mn;
		xor ebx, ebx;
		mov bl, al;
		mov word ptr[esi], bx;
		mov bl, ah;
		mov word ptr[esi + 2], bx;
		mov word ptr[esi + 4], bx;
		shr eax, 16;
		mov bl, al;
		mov word ptr[esi + 6], bx;
		mov word ptr[esi + 8], bx;
		mov bl, ah;
		mov word ptr[esi + 10], bx;
	}
}
#endif

#define SMOOTH_MN_MM128I(i) _mm_set_epi16(0, 0, img.data_[(i)*img.cols_ + j + 3],\
	img.data_[(i)*img.cols_ + j + 2], img.data_[(i)*img.cols_ + j + 2], \
	img.data_[(i)*img.cols_ + j + 1], img.data_[(i)*img.cols_ + j + 1], \
	img.data_[(i)*img.cols_ + j + 0])


int Smooth(const Mat& img, Mat& saved_img, SMOOTH_TYPE smooth_type)
{
	START_TIME;

	saved_img.release();
	saved_img.creat(img.rows_, img.cols_, img.type());

	if (img.type() != MCV_8UC1) {
		std::cerr << "smoth do not support this kind of image\n";
		return -1;
	}

	img.copyTo(saved_img);

	const int ksize = 3;
	short neib_avg_kernel[ksize][ksize] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
	short weit_avg_kernel[ksize][ksize] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };
	short lap_mask1_kernel[ksize][ksize] = { { 0, -1, 0 }, { -1, 4, -1 }, { 0, -1, 0 } };
	short lap_mask2_kernel[ksize][ksize] = { { -1, -1, -1 }, { -1, 8, -1 }, { -1, -1, -1 } };


	short *k = NULL;
	switch (smooth_type){
	case SMOOTH_NEIB_AVG:
		k = &neib_avg_kernel[0][0];
		break;
	case SMOOTH_WEIT_AVG:
		k = &weit_avg_kernel[0][0];
		break;
	case SMOOTH_LAP_MASK1:
		k = &lap_mask1_kernel[0][0];
		break;
	case SMOOTH_LAP_MASK2:
		k = &lap_mask2_kernel[0][0];
		break;
	default:
		return -1;
	}



	__m128i mn;
	mn = _mm_set_epi16(0, 0, k[2], k[2], k[1], k[1], k[0], k[0]);
	// __asm 	movdqa xmm4, mn;
	__asm__("movdqa xmm4, mn;");

	mn = _mm_set_epi16(0, 0, k[5], k[5], k[4], k[4], k[3], k[3]);
	// __asm 	movdqa xmm5, mn;
	__asm__("movdqa xmm5, mn;");

	mn = _mm_set_epi16(0, 0, k[8], k[8], k[7], k[7], k[6], k[6]);
	// __asm 	movdqa xmm6, mn;
	__asm__("movdqa xmm6, mn;");

	uchar gray_trans = 100;

#ifdef MAGNIFY
	for (int loop = 0; loop < 100; ++loop) {
#endif // MAGNIFY

		for (int i = 0; i < img.rows_ - 2; ++i) {
			for (int j = 0; j < img.cols_ - 2; j += 2) {

				//mn = _mm_setzero_si128();
				// __asm {
				// 	pxor xmm0, xmm0;
				// 	movdqa mn, xmm0;
				// }
				__asm__(
					"pxor xmm0, xmm0;"	\
					"movdqa mn, xmm0;"	\
				);
				mn = SMOOTH_MN_MM128I(i);
				// __asm	movdqa xmm1, mn;
				__asm__("movdqa xmm1, mn;");

				mn = SMOOTH_MN_MM128I(i + 1);
				// __asm	movdqa xmm2, mn;
				__asm__("movdqa xmm2, mn;");

				mn = SMOOTH_MN_MM128I(i + 2);
				// __asm 	movdqa xmm3, mn;
				__asm__("movdqa xmm3, mn;");

				// __asm {
				// 	pmullw	xmm1, xmm4;
				// 	pmullw	xmm2, xmm5;
				// 	pmullw	xmm3, xmm6;
				// 	paddw	xmm1, xmm2;
				// 	paddw	xmm1, xmm3;
				// 	movdqa  xmm2, xmm1;
				// 	psrldq  xmm2, 4;
				// 	movdqa  xmm3, xmm1;
				// 	psrldq  xmm3, 8;
				// 	paddw	xmm1, xmm2;
				// 	paddw	xmm1, xmm3;
				// 	movdqa	mn, xmm1;
				// }
				__asm__(
						"pmullw	xmm1, xmm4;"	\
						"pmullw	xmm2, xmm5;"	\
						"pmullw	xmm3, xmm6;" 	\
						"paddw	xmm1, xmm2;"	\
						"paddw	xmm1, xmm3;"	\
						"movdqa  xmm2, xmm1;"	\
						"psrldq  xmm2, 4;"	\
						"movdqa  xmm3, xmm1;" 	\
						"psrldq  xmm3, 8;" 	\
						"paddw	xmm1, xmm2;" 	\
						"paddw	xmm1, xmm3;" \
						"movdqa	mn, xmm1;"	\
						);

				switch (smooth_type)
				{
				case SMOOTH_NEIB_AVG:
					mn.m128i_i16[0] /= 9;
					mn.m128i_i16[1] /= 9;
					break;
				case SMOOTH_WEIT_AVG:
					mn = _mm_srli_epi16(mn, 4);	//divived by 16
					break;
				case SMOOTH_LAP_MASK1:
					break;
				case SMOOTH_LAP_MASK2:

					break;
				}
				saved_img.data_[(i + 1)*img.cols_ + j] = GrayShort2Uchar(mn.m128i_i16[0]);
				saved_img.data_[(i + 1)*img.cols_ + j + 1] = GrayShort2Uchar(mn.m128i_i16[1]);

				if (smooth_type == SMOOTH_LAP_MASK1 || smooth_type == SMOOTH_LAP_MASK2) {
					
					saved_img.data_[(i + 1)*img.cols_ + j] += gray_trans;
					saved_img.data_[(i + 1)*img.cols_ + j + 1] += gray_trans;
				}
			}
		}

#ifdef MAGNIFY
	}
#endif	// MAGNIFY

	PRINT_TIME("Smooth");

	return 0;
}

#ifdef DEBUG

//for comparing
int SmoothNosse(const Mat& img, Mat& saved_img, SMOOTH_TYPE smooth_type)
{
	START_TIME;

	img.copyTo(saved_img);

	const int ksize = 3;
	short neib_avg_kernel[ksize][ksize] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
	short weit_avg_kernel[ksize][ksize] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };
	short lap_mask1_kernel[ksize][ksize] = { { 0, -1, 0 }, { -1, 4, -1 }, { 0, -1, 0 } };
	short lap_mask2_kernel[ksize][ksize] = { { -1, -1, -1 }, { -1, 8, -1 }, { -1, -1, -1 } };


	short *k = NULL;
	switch (smooth_type)
	{
	case SMOOTH_NEIB_AVG:
		k = &neib_avg_kernel[0][0];
		break;
	case SMOOTH_WEIT_AVG:
		k = &weit_avg_kernel[0][0];
		break;
	case SMOOTH_LAP_MASK1:
		k = &lap_mask1_kernel[0][0];
		break;
	case SMOOTH_LAP_MASK2:
		k = &lap_mask2_kernel[0][0];
		break;
	default:
		return -1;
	}



	uchar gray_trans = 100;

#ifdef MAGNIFY
	for (int loop = 0; loop < 100; ++loop) {
#endif // MAGNIFY

		for (int i = 0; i < img.rows_ - 2; ++i) {
			for (int j = 0; j < img.cols_ - 2; ++j) {

				short sum;
				sum = img.ptr(i)[j] * k[0] + img.ptr(i)[j + 1] * k[1] + img.ptr(i)[j + 2] * k[2]
					+ img.ptr(i + 1)[j] * k[3] + img.ptr(i + 1)[j + 1] * k[4] + img.ptr(i + 1)[j + 2] * k[5]
					+ img.ptr(i + 2)[j] * k[6] + img.ptr(i + 2)[j + 1] * k[7] + img.ptr(i + 2)[j + 2] * k[8];

				switch (smooth_type)
				{
				case SMOOTH_NEIB_AVG:
					sum /= 9;
					break;
				case SMOOTH_WEIT_AVG:
					sum = sum >> 4;	//divived by 16
					break;
				case SMOOTH_LAP_MASK1:
					break;
				case SMOOTH_LAP_MASK2:

					break;
				}
				saved_img.ptr(i + 1)[j] = GrayShort2Uchar(sum);

				if (smooth_type == SMOOTH_LAP_MASK1 || smooth_type == SMOOTH_LAP_MASK2) {
					//gray translation
					saved_img.ptr(i + 1)[j] += gray_trans;
				}
			}
		}

#ifdef MAGNIFY
	}
#endif	// MAGNIFY
	PRINT_TIME("smoothNosse");
	return 0;
}

#endif // DEBUG


_MCV_END_