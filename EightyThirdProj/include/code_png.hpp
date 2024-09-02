#ifndef _CODE_PNG_HPP_
#define _CODE_PNG_HPP_

#include "code_base.hpp"

#ifdef HAVE_PNG


_MCV_START_

class PngDecoder : public BaseImageDecoder
{
public:

	PngDecoder();
	virtual ~PngDecoder();

	bool  readData(Mat& img);
	bool  readHeader();
	void  close();

	ImageDecoder newDecoder() const;

protected:

	int   bit_depth_;
	void* png_ptr_;  
	void* info_ptr_; 
	void* end_info_; 
	FILE* f_;

	int   color_type_;
};




class PngEncoder : public BaseImageEncoder
{
public:
	PngEncoder();
	virtual ~PngEncoder();

	bool  isFormatSupported(int depth) const;
	bool  write(const Mat& img);

	ImageEncoder newEncoder() const;

};




_MCV_END_

#endif // HAVE_PNG


#endif // !_CODE_PNG_HPP_
