#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include LC_ERROR_H 

#ifdef USE_LIBJPEG
/* *********************************************************************
 * jmorecfg.h 里面的boolean是这样定义的：
 * typedef enum { FALSE = 0, TRUE = 1 } boolean;
 * 为了不使LCUI的FALSE和TRUE这两个宏与boolean枚举中的FALSE和TRUE产生冲突，在此用
 * #undef取消这两个宏。
 * ********************************************************************/
#undef TRUE
#undef FALSE

#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	(*cinfo->err->output_message)(cinfo);
	longjmp(myerr->setjmp_buffer,1);
}
#endif

/* 载入jpeg图片文件 */
LCUI_API int Graph_LoadJPEG( const char *filepath, LCUI_Graph *out )
{
#ifdef USE_LIBJPEG
	FILE *fp;
	int row_stride,jaka;
	int x,y, m, n, k;
	short int JPsyg;
	
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	JSAMPARRAY buffer;

	fp = fopen(filepath,"r");
	if(fp == NULL) {
		return FILE_ERROR_OPEN_ERROR;
	}
	
	if( fread( &JPsyg, sizeof(short int), 1, fp ) ) {
		if ( JPsyg != -9985 ) {  /* 如果不是jpg图片 */
			return  FILE_ERROR_UNKNOWN_FORMAT; 
		}
	}
	rewind(fp);
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		return 2;
	}
	
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo,fp);
	(void) jpeg_read_header(&cinfo,TRUE);
	(void) jpeg_start_decompress(&cinfo);    
	
	jaka = cinfo.num_components;
	
	//if (jaka==3) printf("color\n"); else printf("grayscale\n");
	out->color_type = COLOR_TYPE_RGB;
	n = Graph_Create(out, cinfo.output_width, cinfo.output_height);
	if( n != 0 ){
		printf("%s (): error: %s",__FUNCTION__, MALLOC_ERROR);
		exit(-1);
	}
	
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)(
			(j_common_ptr) &cinfo,JPOOL_IMAGE,row_stride,1);
	
	for(y=0; cinfo.output_scanline <cinfo.output_height; ++y) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		m = y*out->w;
		if ( jaka == 3 ) {
			for (x=0;x<out->w;x++) {
				n = x+m;
				k=x*3;
				out->rgba[0][n]=buffer[0][k++];
				out->rgba[1][n]=buffer[0][k++];
				out->rgba[2][n]=buffer[0][k++];
			}
		} else {
			for (x=0;x<out->w;x++) {
				n = x+m;
				out->rgba[0][n]=buffer[0][x];
				out->rgba[1][n]=buffer[0][x];
				out->rgba[2][n]=buffer[0][x];
			}
		} 
	}
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);
#else
	printf("warning: not JPEG support!"); 
#endif
	return 0;
}
