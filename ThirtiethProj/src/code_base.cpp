#include "code_base.hpp"


_MCV_START_

BaseImageDecoder::BaseImageDecoder()
{
	width_ = height_ = 0;
	type_ = -1;
}

bool BaseImageDecoder::setSource(const std::string& filename)
{
	filename_ = filename;
	return true;
}

size_t BaseImageDecoder::signLength() const
{
	return sign_.size();
}

bool BaseImageDecoder::checkSign(const String& signature) const
{
	size_t len = signLength();
	
	return signature.size() >= len && memcmp(signature.c_str(), sign_.c_str(), len) == 0;
}

ImageDecoder BaseImageDecoder::newDecoder() const
{
	return ImageDecoder();
}



BaseImageEncoder::BaseImageEncoder()
{
	
}

bool  BaseImageEncoder::isFormatSupported(int depth) const
{
	return depth == MCV_8U;
}

String BaseImageEncoder::getDescription() const
{
	return description_;
}

bool BaseImageEncoder::setDestination(const String& filename)
{
	filename_ = filename;
	
	return true;
}

ImageEncoder BaseImageEncoder::newEncoder() const
{
	return ImageEncoder();
}



_MCV_END_