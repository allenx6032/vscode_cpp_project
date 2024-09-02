#ifndef _ALGORITHMS_H_
#define _ALGORITHMS_H_

#include "mcv_types.h"
#include "mat.hpp"

#include <ctime>
#include <iostream>

_MCV_START_


//#define TIME_MACRO
///#define MAGNIFY


#ifdef TIME_MACRO

#define START_TIME clock_t __start_time_, __end_time_; __start_time_ = clock();
#define PRINT_TIME(word) __end_time_ = clock(); \
	std::cout << word << " costs " << ": \t" << \
	(__end_time_ - __start_time_)  \
	<< "ms" << std::endl;
#else
#define START_TIME 0
#define PRINT_TIME(word) 0
#endif // TIME_MACRO

enum SMOOTH_TYPE
{
	SMOOTH_NEIB_AVG = 0,
	SMOOTH_WEIT_AVG = 1,
	SMOOTH_LAP_MASK1 = 2,
	SMOOTH_LAP_MASK2 = 3
};


void NegativeImg(const Mat& img, Mat& saved_img);

void CompressGray(const Mat& img, Mat& saved_img, double c = 20);

int Smooth(const Mat& img, Mat& saved_img, SMOOTH_TYPE smooth_type);

int SmoothNosse(const Mat& img, Mat& saved_img, SMOOTH_TYPE smooth_type);

_MCV_END_


#endif // _ALGORITHMS_H_
