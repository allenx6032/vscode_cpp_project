

#include "code_bmp.hpp"

#ifdef HAVE_BMP

_MCV_START_

BmpDecoder::BmpDecoder() {
	sign_ = "BM";
}


BmpDecoder::~BmpDecoder() {
	close();
}


void  BmpDecoder::close() {
	buf_.~Mat();
}

ImageDecoder BmpDecoder::newDecoder() const {
	return makePtr<BmpDecoder>();
}

bool  BmpDecoder::readHeader() {
	BMP bmp;
	if (!bmp.ReadFromFile(filename_.c_str())) return false;
	width_ = bmp.TellWidth();
	height_ = bmp.TellHeight();
	
	type_ = bmp.TellBitDepth() == 1? MCV_8UC1: MCV_8UC3;

	buf_.flags_ = type_;
	
	buf_.creat(height_, width_, type_); //height -> rows, width -> cols

	uchar* p_data = buf_.data_;

	
	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			if (type_ == MCV_8UC1) {
				*p_data = bmp(i, j)->Red;
				++p_data;
			}
			else {
				*p_data = bmp(j, i)->Red;
				++p_data;
				*p_data = bmp(j, i)->Green;
				++p_data;
				*p_data = bmp(j, i)->Blue;
				++p_data;
			}
		}
	}
	
	return true;
}

bool BmpDecoder::readData(Mat& img) {

	if (img.type() == buf_.type()) {
		img = buf_;
	}
	else {
		buf_.convertTo(img, img.type());
	}

	close();
	return true;
}


/* BmpEncoder */

BmpEncoder::BmpEncoder(){
	description_ = "Windows bitmap (*.bmp)";

}

BmpEncoder::~BmpEncoder() {

}

ImageEncoder BmpEncoder::newEncoder() const {
	return makePtr<BmpEncoder>();
}

bool BmpEncoder::write(const Mat& img) {

	BMP bmp;

	RGBApixel pix_tmp = { 0 };

	int height = img.rows_;
	int width = img.cols_;

	bmp.SetSize(width, height);

	uchar* p_data = img.data_;
	

	if (img.channels() == 1) {
		
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				pix_tmp.Red = *p_data;
				pix_tmp.Green = *p_data;
				pix_tmp.Blue = *p_data;
				++p_data;
				bmp.SetPixel(j, i, pix_tmp);
			}
		}
		
		bmp.SetBitDepth(24);
	}
	else {
		
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				pix_tmp.Red = *p_data;
				++p_data;
				pix_tmp.Green = *p_data;
				++p_data;
				pix_tmp.Blue = *p_data;
				++p_data;

				bmp.SetPixel(j, i, pix_tmp);
			}
		}
		bmp.SetBitDepth(24);
	}
	bmp.WriteToFile(filename_.c_str());

	return true;
}
_MCV_END_



#endif // HAVE_BMP