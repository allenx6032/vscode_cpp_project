#ifndef _MCV_TYPES_H_
#define _MCV_TYPES_H_

#define _CRT_SECURE_NO_WARNINGS

//#define BMP_DEBUG

#define HAVE_PNG
#define HAVE_BMP



#include <iostream>
#include <algorithm>
#include <string>
typedef std::string String;


#define _USE_NAMESPACE_MCV_

#ifdef _USE_NAMESPACE_MCV_	
#define _MCV_START_	namespace mcv {
#define _MCV_END_	}
#define _MCV_ mcv::

#else
#define _MCV_START_
#define _MCV_END_
#define _MCV_ 
#endif // _USE_NAMESPACE_MCV_	





_MCV_START_

typedef unsigned char uchar;

template<typename T>
T* makePtr()
{
	return (new T());
}






/* Matrix type from opencv*/

#define MCV_CN_MAX     512
#define MCV_CN_SHIFT   3
#define MCV_DEPTH_MAX  (1 << MCV_CN_SHIFT)

#define MCV_8U   0
#define MCV_8S   1
#define MCV_16U  2
#define MCV_16S  3
#define MCV_32S  4
#define MCV_32F  5
#define MCV_64F  6
#define MCV_USRTYPE1 7

#define MCV_MAT_DEPTH_MASK       (MCV_DEPTH_MAX - 1)
#define MCV_MAT_DEPTH(flags)     ((flags) & MCV_MAT_DEPTH_MASK)

#define MCV_MAKETYPE(depth,cn) (MCV_MAT_DEPTH(depth) + (((cn)-1) << MCV_CN_SHIFT))
#define MCV_MAKE_TYPE MCV_MAKETYPE

#define MCV_8UC1 MCV_MAKETYPE(MCV_8U,1)
#define MCV_8UC2 MCV_MAKETYPE(MCV_8U,2)
#define MCV_8UC3 MCV_MAKETYPE(MCV_8U,3)
#define MCV_8UC4 MCV_MAKETYPE(MCV_8U,4)
#define MCV_8UC(n) MCV_MAKETYPE(MCV_8U,(n))

#define MCV_8SC1 MCV_MAKETYPE(MCV_8S,1)
#define MCV_8SC2 MCV_MAKETYPE(MCV_8S,2)
#define MCV_8SC3 MCV_MAKETYPE(MCV_8S,3)
#define MCV_8SC4 MCV_MAKETYPE(MCV_8S,4)
#define MCV_8SC(n) MCV_MAKETYPE(MCV_8S,(n))

#define MCV_16UC1 MCV_MAKETYPE(MCV_16U,1)
#define MCV_16UC2 MCV_MAKETYPE(MCV_16U,2)
#define MCV_16UC3 MCV_MAKETYPE(MCV_16U,3)
#define MCV_16UC4 MCV_MAKETYPE(MCV_16U,4)
#define MCV_16UC(n) MCV_MAKETYPE(MCV_16U,(n))

#define MCV_16SC1 MCV_MAKETYPE(MCV_16S,1)
#define MCV_16SC2 MCV_MAKETYPE(MCV_16S,2)
#define MCV_16SC3 MCV_MAKETYPE(MCV_16S,3)
#define MCV_16SC4 MCV_MAKETYPE(MCV_16S,4)
#define MCV_16SC(n) MCV_MAKETYPE(MCV_16S,(n))

#define MCV_32SC1 MCV_MAKETYPE(MCV_32S,1)
#define MCV_32SC2 MCV_MAKETYPE(MCV_32S,2)
#define MCV_32SC3 MCV_MAKETYPE(MCV_32S,3)
#define MCV_32SC4 MCV_MAKETYPE(MCV_32S,4)
#define MCV_32SC(n) MCV_MAKETYPE(MCV_32S,(n))

#define MCV_32FC1 MCV_MAKETYPE(MCV_32F,1)
#define MCV_32FC2 MCV_MAKETYPE(MCV_32F,2)
#define MCV_32FC3 MCV_MAKETYPE(MCV_32F,3)
#define MCV_32FC4 MCV_MAKETYPE(MCV_32F,4)
#define MCV_32FC(n) MCV_MAKETYPE(MCV_32F,(n))

#define MCV_64FC1 MCV_MAKETYPE(MCV_64F,1)
#define MCV_64FC2 MCV_MAKETYPE(MCV_64F,2)
#define MCV_64FC3 MCV_MAKETYPE(MCV_64F,3)
#define MCV_64FC4 MCV_MAKETYPE(MCV_64F,4)
#define MCV_64FC(n) MCV_MAKETYPE(MCV_64F,(n))

#define MCV_MAT_CN_MASK          ((MCV_CN_MAX - 1) << MCV_CN_SHIFT)
#define MCV_MAT_CN(flags)        ((((flags) & MCV_MAT_CN_MASK) >> MCV_CN_SHIFT) + 1)
#define MCV_MAT_TYPE_MASK        (MCV_DEPTH_MAX*MCV_CN_MAX - 1)
#define MCV_MAT_TYPE(flags)      ((flags) & MCV_MAT_TYPE_MASK)
#define MCV_MAT_CONT_FLAG_SHIFT  14
#define MCV_MAT_CONT_FLAG        (1 << MCV_MAT_CONT_FLAG_SHIFT)
#define MCV_IS_MAT_CONT(flags)   ((flags) & MCV_MAT_CONT_FLAG)
#define MCV_IS_CONT_MAT          MCV_IS_MAT_CONT
#define MCV_SUBMAT_FLAG_SHIFT    15
#define MCV_SUBMAT_FLAG          (1 << MCV_SUBMAT_FLAG_SHIFT)
#define MCV_IS_SUBMAT(flags)     ((flags) & MCV_MAT_SUBMAT_FLAG)


#define MCV_ELEM_SIZE1(type) \
	((((sizeof(size_t) << 28) | 0x8442211) >> MCV_MAT_DEPTH(type) * 4) & 15)


#define MCV_ELEM_SIZE(type) \
	(MCV_MAT_CN(type) << ((((sizeof(size_t) / 4 + 1) * 16384 | 0x3a50) >> MCV_MAT_DEPTH(type) * 2) & 3))




_MCV_END_

#endif // !_MCV_TYPES_H_
