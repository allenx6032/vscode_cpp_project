#ifndef _MCV_H_
#define _MCV_H_

//#define _USE_NAMESPACE_MCV_

#include "mcv_types.h"
#include "mat.hpp"
#include "algorithms.h"

_MCV_START_



Mat imread(const String& filename, int flags = -1);
bool imwrite(const String& filename, const Mat& img, int flags = -1);



_MCV_END_


#endif // !_MCV_H_
