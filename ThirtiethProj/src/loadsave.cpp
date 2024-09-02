#include "code_hfs.h"
#include <vector>
#include <iostream>

_MCV_START_

struct ImageCodecInitializer {
	
	ImageCodecInitializer()
	{

#ifdef HAVE_BMP
		
		decoders.push_back(makePtr<BmpDecoder>());
		encoders.push_back(makePtr<BmpEncoder>());

#endif // HAVE_BMP
		
#ifdef HAVE_PNG
		decoders.push_back(makePtr<PngDecoder>());
		encoders.push_back(makePtr<PngEncoder>());
#endif
	}

	std::vector<ImageDecoder> decoders;
	std::vector<ImageEncoder> encoders;
};

static ImageCodecInitializer codecs;

static ImageDecoder findDecoder(const String& filename) 
{

	size_t i, maxlen = 0;


	for (i = 0; i < codecs.decoders.size(); i++)
	{
		size_t len = codecs.decoders[i]->signLength();
		maxlen = std::max(maxlen, len);
	}

	
	FILE* f = fopen(filename.c_str(), "rb");

	
	if (!f)
		return ImageDecoder();

	// read the file signature
	String signature(maxlen, ' ');
	maxlen = fread((void*)signature.c_str(), 1, maxlen, f);
	fclose(f);
	signature = signature.substr(0, maxlen);

	/// compare signature against all decoders
	for (i = 0; i < codecs.decoders.size(); i++)
	{
		if (codecs.decoders[i]->checkSign(signature))
			return codecs.decoders[i]->newDecoder();
	}

	
	return ImageDecoder();
}


static void imread_(const String& filename, int flags, Mat& img) 
{
	ImageDecoder decoder = findDecoder(filename);

	if (decoder == NULL) return ;

	decoder->setSource(filename);

	if (!decoder->readHeader()) return ;

	int width, height;
	width = decoder->width();
	height = decoder->height();
	
	int type;
	
	if (flags == 0) {
		type = MCV_8UC1;
	}
	else {
		type = decoder->type();
	}
	
	img.creat(height, width, type);

	if (decoder->readData(img) == false) {
		img.release();
	}
}

Mat imread(const String& filename, int flags )
{
	
	Mat img;
	
	imread_(filename, flags, img);
	
	return img;
}


//--------------------------------  imwrite ----------------//



static ImageEncoder findEncoder(const String& _ext)
{
	if (_ext.size() <= 1)
		return ImageEncoder();

	const char* ext = strrchr(_ext.c_str(), '.');
	if (!ext)
		return ImageEncoder();
	int len = 0;
	for (ext++; isalnum(ext[len]) && len < 128; len++)
		;

	for (size_t i = 0; i < codecs.encoders.size(); i++)	{
		String description = codecs.encoders[i]->getDescription();
		const char* descr = strchr(description.c_str(), '(');

		while (descr){
			descr = strchr(descr + 1, '.');
			if (!descr)
				break;
			int j = 0;
			for (descr++; isalnum(descr[j]) && j < len; j++){
				int c1 = tolower(ext[j]);
				int c2 = tolower(descr[j]);
				if (c1 != c2)
					break;
			}
			if (j == len && !isalnum(descr[j]))
				return codecs.encoders[i]->newEncoder();
			descr += j;
		}
	}

	return ImageEncoder();
}


bool imwrite(const String& filename, const Mat& img, int flags )
{
	Mat tmp;

	ImageEncoder encoder = findEncoder(filename);

	if (encoder == NULL) {
		std::cerr << "cannot find a writer for this format picture\n ";
		return false;
	}

	if (encoder->isFormatSupported(img.depth()) == false) {
		//img.convertTo( tmp, MCV_8U );
		return false;
	}
	else {
		tmp = img;
	}

	encoder->setDestination(filename);

	return encoder->write(tmp);
}

_MCV_END_