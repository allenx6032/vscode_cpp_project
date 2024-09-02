


#define _CRT_SECURE_NO_WARNINGS

#include "libpng/png.h"
#include "zlib/zlib.h"

#include "code_png.hpp"


#ifdef HAVE_PNG


_MCV_START_


PngDecoder::PngDecoder() {
	sign_ = "\x89\x50\x4e\x47\xd\xa\x1a\xa";
	color_type_ = 0;
	png_ptr_ = NULL;
	f_ = NULL;
}

PngDecoder::~PngDecoder(){
	close();
}

ImageDecoder PngDecoder::newDecoder() const
{
	return makePtr<PngDecoder>();
}

void  PngDecoder::close()
{
	if (f_)	{
		fclose(f_);
		f_ = NULL;
	}

	if (png_ptr_){
		png_structp png_ptr = (png_structp)png_ptr_;
		png_infop info_ptr = (png_infop)info_ptr_;
		png_infop end_info = (png_infop)end_info_;
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		png_ptr_ = info_ptr_ = end_info_ = 0;
	}
}

bool  PngDecoder::readHeader()
{
	bool result = false;
	close();

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	if (png_ptr){
		png_infop info_ptr = png_create_info_struct(png_ptr);
		png_infop end_info = png_create_info_struct(png_ptr);

		png_ptr_ = png_ptr;
		info_ptr_ = info_ptr;
		end_info_ = end_info;
		
		if (info_ptr && end_info) {
			if (setjmp(png_jmpbuf(png_ptr)) == 0){
				
				f_ = fopen(filename_.c_str(), "rb");
				if (f_)
					png_init_io(png_ptr, f_);
			
				if (f_){
					png_uint_32 wdth, hght;
					int bit_depth, color_type, num_trans = 0;
					png_bytep trans;
					png_color_16p trans_values;

					png_read_info(png_ptr, info_ptr);

					png_get_IHDR(png_ptr, info_ptr, &wdth, &hght,
						&bit_depth, &color_type, 0, 0, 0);

					width_ = (int)wdth;
					height_ = (int)hght;
					color_type_ = color_type;
					bit_depth_ = bit_depth;

					if (bit_depth <= 8 || bit_depth == 16) {
						switch (color_type)	{
						case PNG_COLOR_TYPE_RGB:
							type_ = MCV_8UC3;
							break;
						case PNG_COLOR_TYPE_PALETTE:
							png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values);
							
							if (num_trans > 0)
								type_ = MCV_8UC4;
							else
								type_ = MCV_8UC3;
							break;
						case PNG_COLOR_TYPE_RGB_ALPHA:
							type_ = MCV_8UC4;
							break;
						default:
							type_ = MCV_8UC1;
						}
						if (bit_depth == 16)
							type_ = MCV_MAKETYPE(MCV_16U, MCV_MAT_CN(type_));
						result = true;
					}		
				}
			}
		}
	}

	if (!result)
		close();

	return result;
}


bool  PngDecoder::readData( Mat& img )
{
	bool result = false;
	
	uchar** buffer = new uchar*[height_];
	int color = img.channels() > 1;
	uchar* data = img.data_;
	int step = (int)img.elemSize()*img.cols_;

	if (png_ptr_ && info_ptr_ && end_info_ && width_ && height_){
		png_structp png_ptr = (png_structp)png_ptr_;
		png_infop info_ptr = (png_infop)info_ptr_;
		png_infop end_info = (png_infop)end_info_;

		if (setjmp(png_jmpbuf(png_ptr)) == 0){
			if (img.depth() == MCV_8U && bit_depth_ == 16)
				png_set_strip_16(png_ptr);
		
	
			if (color_type_ == PNG_COLOR_TYPE_PALETTE)
				png_set_palette_to_rgb(png_ptr);


			png_set_interlace_handling(png_ptr);
			png_read_update_info(png_ptr, info_ptr);

			for (int y = 0; y < height_; y++)
				buffer[y] = data + y*step;

			png_read_image(png_ptr, buffer);
			png_read_end(png_ptr, end_info);

			result = true;
		}
	}

	close();
	delete[] buffer;
	return result;
}




PngEncoder::PngEncoder()
{
	description_ = "Portable Network Graphics files (*.png)";
	
}


PngEncoder::~PngEncoder()
{
}


bool  PngEncoder::isFormatSupported(int depth) const
{
	return depth == MCV_8U || depth == MCV_16U;
}

ImageEncoder PngEncoder::newEncoder() const
{
	return makePtr<PngEncoder>();
}



bool  PngEncoder::write(const Mat& img)
{
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = 0;
	FILE* f = 0;
	int y, width = img.cols_, height = img.rows_;
	int depth = img.depth(), channels = img.channels();
	bool result = false;

	uchar** buffer = NULL;
	if (depth != MCV_8U && depth != MCV_16U)
		return false;

	if (png_ptr){
		info_ptr = png_create_info_struct(png_ptr);

		if (info_ptr){
			if (setjmp(png_jmpbuf(png_ptr)) == 0){
			
					f = fopen(filename_.c_str(), "wb");
					if (f)
						png_init_io(png_ptr, f);
			
				int compression_level = 5; // Invalid value to allow setting 0-9 as valid
				int compression_strategy = Z_RLE; // Default strategy
				bool isBilevel = false;

				if ( f)	{
					if (compression_level >= 0){
						png_set_compression_level(png_ptr, compression_level);
					}
					else{
						png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_FILTER_SUB);
						png_set_compression_level(png_ptr, Z_BEST_SPEED);
					}
					png_set_compression_strategy(png_ptr, compression_strategy);

					png_set_IHDR(png_ptr, info_ptr, width, height, depth == MCV_8U ? isBilevel ? 1 : 8 : 16,
						channels == 1 ? PNG_COLOR_TYPE_GRAY :
						channels == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA,
						PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
						PNG_FILTER_TYPE_DEFAULT);

					png_write_info(png_ptr, info_ptr);

					if (isBilevel)
						png_set_packing(png_ptr);

					
					buffer = new uchar*[height];
					
					for (y = 0; y < height; y++)
						buffer[y] = img.data_ + y*img.cols_*img.elemSize();

					png_write_image(png_ptr, buffer);
					png_write_end(png_ptr, info_ptr);

					result = true;
				}
			}
		}
	}

	png_destroy_write_struct(&png_ptr, &info_ptr);
	if (f) fclose(f);

	delete[] buffer;
	return result;
return false;
}

_MCV_END_

#endif // HAVE_PNG