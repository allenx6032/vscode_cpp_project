#ifndef _CODE_BMP_HPP_
#define _CODE_BMP_HPP_



#include "code_base.hpp"
#include "easybmp/EasyBMP.h"


#ifdef HAVE_BMP

_MCV_START_



class BmpDecoder : public BaseImageDecoder
{
public:

	BmpDecoder();
	~BmpDecoder();

	bool  readData(Mat& img);
	bool  readHeader();
	void  close();

	ImageDecoder newDecoder() const;

protected:
	
};



class BmpEncoder : public BaseImageEncoder
{
public:
	BmpEncoder();
	~BmpEncoder();

	bool  write(const Mat& img);

	ImageEncoder newEncoder() const;
};



_MCV_END_




#endif // HAVE_BMP


#endif // !_CODE_BMP_HPP_
