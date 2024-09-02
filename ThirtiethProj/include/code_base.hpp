#ifndef _CODE_BASE_HPP_
#define _CODE_BASE_HPP_

#include "mcv_types.h"
#include "mat.hpp"
#include <memory>
#include <string>
#include <vector>

_MCV_START_


class BaseImageDecoder;
class BaseImageEncoder;

typedef BaseImageEncoder* ImageEncoder;
typedef BaseImageDecoder* ImageDecoder;

class BaseImageDecoder {

public:
	BaseImageDecoder();
	virtual ~BaseImageDecoder() {}

	int width() const { return width_; }
	int height() const { return height_; }
	virtual int type() const { return type_; }
	virtual bool setSource(const std::string& filename);

	virtual bool readHeader() = 0;
	virtual bool readData(Mat& img) = 0;

	virtual size_t BaseImageDecoder::signLength() const;
	virtual bool checkSign(const std::string& sign) const;
	virtual ImageDecoder newDecoder() const;

protected:
	int width_;
	int height_;
	int type_;
	std::string filename_;
	std::string sign_;

	Mat buf_;

};


class BaseImageEncoder {
public:
	BaseImageEncoder();
	virtual ~BaseImageEncoder() {}
	virtual bool isFormatSupported(int depth) const;

	virtual bool setDestination(const String& filename);

	virtual bool write(const Mat& img) = 0;

	virtual String getDescription() const;
	virtual ImageEncoder newEncoder() const;

	
protected:
	String description_;

	String filename_;
	
};

_MCV_END_

#endif // !_CODE_BASE_HPP_
