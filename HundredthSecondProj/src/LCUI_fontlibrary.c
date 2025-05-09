/** ******************************************************************************
 * @file	fontlibrary.c
 * @brief	The font database management module.
 * @author	Liu Chao <lc-soft@live.cn>
 * @warning
 * Copyright (C) 2012-2013 by							\n
 * Liu Chao									\n
 * 										\n
 * This file is part of the LCUI project, and may only be used, modified, and	\n
 * distributed under the terms of the GPLv2.					\n
 * 										\n
 * (GPLv2 is abbreviation of GNU General Public License Version 2)		\n
 * 										\n
 * By continuing to use, modify, or distribute this file you indicate that you	\n
 * have read the license and understand and accept it fully.			\n
 *  										\n
 * The LCUI project is distributed in the hope that it will be useful, but 	\n
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 	\n
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for more details.	\n
 * 										\n
 * You should have received a copy of the GPLv2 along with this file. It is 	\n
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.	\n
 * ******************************************************************************/
 
/** ******************************************************************************
 * @file	fontlibrary.c
 * @brief	字体数据库管理模块。
 * @author	刘超 <lc-soft@live.cn>
 * @warning
 * 版权所有 (C) 2012-2013 归属于						\n
 * 刘超										\n
 * 										\n
 * 这个文件是LCUI项目的一部分，并且只可以根据GPLv2许可协议来使用、更改和发布。	\n
 * 										\n
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)				\n
 * 										\n
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。	\n
 * 										\n
 * LCUI 项目是基于使用目的而加以散布的，但不负任何担保责任，甚至没有适销性或特定\n
 * 用途的隐含担保，详情请参照GPLv2许可协议。					\n
 * 										\n
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果	\n
 * 没有，请查看：<http://www.gnu.org/licenses/>. 				\n
 * ******************************************************************************/

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_ERROR_H
#include LC_FONT_H

#ifdef LCUI_FONT_ENGINE_FREETYPE

#include <ft2_build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

#else

typedef void* FT_Library;
typedef void* FT_Face;

#endif

#define NAME_MAX_LEN	256	/**< 最大名字长度 */
#define PATH_MAX_LEN	1024	/**< 最大路径长度 */

typedef struct LCUI_FontBMPItem_ {
	int pixel_size;			/**< 位图像素大小 */
	LCUI_FontBMP *bitmap;		/**< 位图数据 */
} LCUI_FontBMPItem;

typedef struct LCUI_FontDataItem_ {
	int font_id;			/**< 字体信息ID */
	LCUI_Queue font_bmp;		/**< 位图库 */
} LCUI_FontDataItem;

typedef struct LCUI_FontCharItem_ {
	wchar_t char_code;		/**< 字符码 */
	LCUI_Queue data;		/**< 与该字符关联的数据 */
} LCUI_FontCharItem;

typedef struct LCUI_FontInfo_ {
	int id;				/**< 字体信息ID */
	char style_name[NAME_MAX_LEN];	/**< 风格名 */
	char family_name[NAME_MAX_LEN];	/**< 字族名 */
	char filepath[PATH_MAX_LEN];	/**< 字体文件路径 */
	FT_Face face;
} LCUI_FontInfo;

static int font_count = 0;
static FT_Library library = NULL;		/**< FreeType 库句柄 */
static LCUI_BOOL database_init = FALSE;		/**< 标记，指示数据库是否初始化 */
static LCUI_Queue font_database;		/**< 字体信息数据库 */
static LCUI_Queue fontbitmap_database;		/**< 字体位图数据库 */
static LCUI_FontInfo *default_font = NULL;	/**< 指向默认字体信息的指针 */
static LCUI_FontInfo *in_core_font = NULL;	/**< 指向内置字体的信息的指针 */

static void FontLIB_DestroyBMP( void *arg )
{
	LCUI_FontBMPItem *bmp;
	bmp = (LCUI_FontBMPItem *)arg;
	if( bmp->bitmap ) {
		free( bmp->bitmap );
	}
}

static void FontLIB_DestroyData( void *arg )
{
	LCUI_FontDataItem *data;
	data = (LCUI_FontDataItem *)arg;
	Queue_Destroy( &data->font_bmp );
}

static void FontLIB_Destroy( void *arg )
{
	LCUI_FontCharItem *font;
	font = (LCUI_FontCharItem *)arg;
	Queue_Destroy( &font->data );
}

static void FontLIB_DestroyFontInfo( void *arg )
{
	
}

LCUI_API void FontLIB_DestroyAll( void )
{
	if( !database_init ) {
		return;
	}
	database_init = FALSE;
	Queue_Destroy( &font_database );
	Queue_Destroy( &fontbitmap_database );
#ifdef LCUI_FONT_ENGINE_FREETYPE
	FT_Done_FreeType( library );
#endif
}

static void FontLIB_DataInit( LCUI_FontDataItem *item )
{
	Queue_Init(	&item->font_bmp, 
			sizeof(LCUI_FontBMPItem), 
			FontLIB_DestroyBMP );
}

static void FontLIB_CharInit( LCUI_FontCharItem *item )
{
	Queue_Init(	&item->data, 
			sizeof(LCUI_FontDataItem), 
			FontLIB_DestroyData );
}

/** 初始化字体数据库 */
LCUI_API void FontLIB_Init( void )
{
	if( database_init ) {
		return;
	}
	Queue_Init(	&fontbitmap_database, 
			sizeof(LCUI_FontCharItem), 
			FontLIB_Destroy );
	Queue_Init(	&font_database, 
			sizeof(LCUI_FontInfo), 
			FontLIB_DestroyFontInfo );
#ifdef LCUI_FONT_ENGINE_FREETYPE
	/* 当初始化库时发生了一个错误 */
	if ( FT_Init_FreeType( &library ) ) {
		printf("%s: %s", __FUNCTION__, FT_INIT_ERROR);
		return;
	}
#else
	library = NULL;
#endif
	database_init = TRUE;
}

#ifdef LCUI_FONT_ENGINE_FREETYPE
/* 获取FT库句柄 */
FT_Library FontLIB_GetLibrary(void)
{
	return library;
}
#endif

/** 通过字体文件路径来查找字体信息，并获取字体ID */
LCUI_API int FontLIB_FindInfoByFilePath( const char *filepath )
{
	int i, n;
	LCUI_FontInfo *item;
	
	if( !database_init ) {
		return -1;
	}
	n = Queue_GetTotal( &font_database );
	for(i=0; i<n; ++i) {
		item = Queue_Get( &font_database, i );
		if( !item ) {
			continue;
		}
		if( strcmp( filepath, item->filepath ) == 0 ) {
			return item->id;
		}
	}
	return -2;
}

/** 获取指定字体ID的字体信息 */
static LCUI_FontInfo* FontLIB_GetFont( int font_id )
{
	int i, n;
	LCUI_FontInfo *item;
	
	if( !database_init ) {
		return NULL;
	}
	n = Queue_GetTotal( &font_database );
	for(i=0; i<n; ++i) {
		item = (LCUI_FontInfo*)Queue_Get( &font_database, i );
		if( !item ) {
			continue;
		}
		if( item->id == font_id ) {
			return item;
		}
	}
	return NULL;
}

/** 获取指定字族名的字体ID */
LCUI_API int FontLIB_GetFontIDByFamilyName( const char *family_name )
{
	int i, n;
	LCUI_FontInfo *item;
	
	if( !database_init ) {
		return -1;
	}
	n = Queue_GetTotal( &font_database );
	for(i=0; i<n; ++i) {
		item = (LCUI_FontInfo*)Queue_Get( &font_database, i );
		if( !item ) {
			continue;
		}
		/* 不区分大小写，进行对比 */
		if( LCUI_strcasecmpA( item->family_name, family_name) == 0 ) {
			return item->id;
		}
	}
	return -2;
}

/** 获取指定字体ID的字体face对象句柄 */
FT_Face FontLIB_GetFontFace( int font_id )
{
	LCUI_FontInfo *info;
	info = FontLIB_GetFont( font_id );
	if( !info ) {
		return NULL;
	}
	return info->face;
}

/** 获取默认的字体ID */
LCUI_API int FontLIB_GetDefaultFontID( void )
{
	if( !default_font ) {
		return -1;
	}
	return default_font->id;
}

/** 设定默认的字体 */
LCUI_API void FontLIB_SetDefaultFont( int id )
{
	LCUI_FontInfo *p;
	p = FontLIB_GetFont( id );
	if( p ) {
		default_font = p;
	}
}

/** 添加字体族，并返回该字族的ID */
static int FontLIB_AddFontInfo(	const char *family_name,
				const char *style_name, 
				const char *filepath,
				FT_Face face )
{
	LCUI_FontInfo *info;
	info = (LCUI_FontInfo*)malloc( sizeof(LCUI_FontInfo) );
	info->id = ++font_count;
	strncpy( info->family_name, family_name, NAME_MAX_LEN );
	strncpy( info->style_name, style_name, NAME_MAX_LEN );
	strncpy( info->filepath, filepath, PATH_MAX_LEN );
	info->face = face;
	Queue_AddPointer( &font_database, info );
	return info->id;
}

static LCUI_FontBMPItem *FontLIB_GetBMPItem(	LCUI_FontDataItem *data_item,
						int pixel_size )
{
	int i, n;
	LCUI_FontBMPItem *item;
	
	n = Queue_GetTotal( &data_item->font_bmp );
	for( i=0; i<n; ++i ) {
		item = (LCUI_FontBMPItem*)Queue_Get( &data_item->font_bmp, i );
		if( !item ) {
			continue;
		}
		if( item->pixel_size == pixel_size ) {
			return item;
		}
	}
	return NULL;
}

static LCUI_FontDataItem *FontLIB_GetDataItem(	LCUI_FontCharItem *char_item,
						int id )
{
	int i, n;
	LCUI_FontDataItem *item;
	
	n = Queue_GetTotal( &char_item->data );
	for( i=0; i<n; ++i ) {
		item = Queue_Get( &char_item->data, i );
		if( !item ) {
			continue;
		}
		if( item->font_id == id ) {
			return item;
		}
	}
	return NULL;
}

static LCUI_FontCharItem *FontLIB_GetCharItem( wchar_t char_code )
{
	int i, n;
	LCUI_FontCharItem *item;
	
	n = Queue_GetTotal( &fontbitmap_database );
	for( i=0; i<n; ++i ) {
		item = (LCUI_FontCharItem*)Queue_Get( &fontbitmap_database, i );
		if( !item ) {
			continue;
		}
		if( item->char_code == char_code ) {
			return item;
		}
	}
	return NULL;
}

/**
 * 添加一个字体位图数据至数据库中
 * @warning
 * 调用此函数后，作为参数fontbmp_buff的变量，不能被free掉，否则，数据库中记录
 * 的此数据会无效 
 * */
LCUI_API LCUI_FontBMP* FontLIB_AddFontBMP(	wchar_t char_code,
						int font_id,
						int pixel_size,
						LCUI_FontBMP *fontbmp_buff )
{
	LCUI_FontCharItem *font;
	LCUI_FontDataItem *data;
	LCUI_FontBMPItem *bmp;
	
	if( !database_init ) {
		return NULL;
	}
	/* 获取字体数据句柄，如果获取失败，则新增 */
	font = FontLIB_GetCharItem( char_code );
	if( !font ) {
		font = (LCUI_FontCharItem*)malloc( sizeof(LCUI_FontCharItem) );
		if( !font ) {
			return NULL;
		}
		FontLIB_CharInit( font );
		font->char_code = char_code; /* 记录字符的编码 */
		Queue_AddPointer( &fontbitmap_database, font );
	}
	/* 但字体ID不大于0时，使用内置字体 */
	if( font_id <= 0 ) {
		font_id = in_core_font->id;
	}
	/* 获取字体字族句柄，如果获取失败，则新增 */
	data = FontLIB_GetDataItem( font, font_id );
	if( !data ) {
		data = (LCUI_FontDataItem*)malloc( sizeof(LCUI_FontDataItem) );
		if( !data ) {
			return NULL;
		}
		FontLIB_DataInit( data );
		data->font_id = font_id; /* 记录该字符使用的字体ID */
		Queue_AddPointer( &font->data, data );
	}
	
	/* 获取字体位图句柄，如果获取失败，则新增 */
	bmp = FontLIB_GetBMPItem( data, pixel_size );
	if( !bmp ) {
		bmp = (LCUI_FontBMPItem*)malloc( sizeof(LCUI_FontBMPItem) );
		if( !bmp ) {
			return NULL;
		}
		bmp->pixel_size = pixel_size; /* 记录该字符的像素尺寸 */
		bmp->bitmap = NULL;
		Queue_AddPointer( &data->font_bmp, bmp );
	}
	
	/* 如果该指针为NULL，那么就申请一块空间 */
	if( NULL == bmp->bitmap ) {
		bmp->bitmap = (LCUI_FontBMP*)malloc( sizeof(LCUI_FontBMP) );
		if( !bmp->bitmap ) {
			return NULL;
		}
	}
	/* 拷贝数据至该空间内 */
	memcpy( bmp->bitmap, fontbmp_buff, sizeof(LCUI_FontBMP) );
	return bmp->bitmap;
}

/** 
 * 获取库中的字体位图
 * @param[in] char_code
 *	字符的Unicode码
 * @param[in] font_id
 *	字体信息的编号
 * @param[in] pixel_size
 *	字体位图的像素尺寸
 * @return
 *	获取失败返回NULL，正常则返回指向字体位图的指针
 */
LCUI_API LCUI_FontBMP* FontLIB_GetFontBMP(	wchar_t char_code,
						int font_id,
						int pixel_size )
{
	LCUI_FontCharItem *font;
	LCUI_FontDataItem *data;
	LCUI_FontBMPItem *bmp;
	
	if( !database_init ) {
		return NULL;
	}
	
	font = FontLIB_GetCharItem( char_code );
	if( !font ) {
		return NULL;
	}
	if( font_id <= 0 ) {
		font_id = in_core_font->id;
	}
	data = FontLIB_GetDataItem( font, font_id );
	if( !data ) {
		return NULL;
	}
	bmp = FontLIB_GetBMPItem( data, pixel_size );
	if( !bmp ) {
		return NULL;
	}
	return bmp->bitmap;
}

/** 
 * 获取库中现有的字体位图，若没有则载入新的
 * @param[in] font_id
 *	字体信息的编号
 * @param[in] ch
 *	字符的Unicode码
 * @param[in] pixel_size
 *	字体位图的像素尺寸
 * @return
 *	获取失败返回NULL，正常则返回指向字体位图的指针
 */
LCUI_API LCUI_FontBMP* FontLIB_GetExistFontBMP(	int font_id,
						wchar_t ch,
						int pixel_size )
{
	LCUI_FontBMP *font_bmp, bmp_buff;

	font_bmp = FontLIB_GetFontBMP( ch, font_id, pixel_size );
	if( font_bmp ) {
		return font_bmp;
	}
	FontBMP_Init( &bmp_buff );
	FontBMP_Load( &bmp_buff, font_id, ch, pixel_size );
	font_bmp = FontLIB_AddFontBMP( ch, font_id, pixel_size, &bmp_buff );
	return font_bmp;
}

/** 载入字体值数据库中 */
LCUI_API int FontLIB_LoadFontFile( const char *filepath )
{
	int id;
#ifdef LCUI_FONT_ENGINE_FREETYPE
	FT_Face face;
	int error_code;
#endif
	if( !filepath ) {
		return -1;
	}
	/* 如果有同一文件路径的字族信息 */
	id = FontLIB_FindInfoByFilePath( filepath );
	if( id >= 0 ) {
		return id;
	}
	
#ifdef LCUI_FONT_ENGINE_FREETYPE
	error_code = FT_New_Face( library, filepath , 0 , &face );
	if( error_code != 0 ) {
		printf("%s: ", __FUNCTION__);
		if ( error_code == FT_Err_Unknown_File_Format ) {
			printf( "%s", FT_UNKNOWN_FILE_FORMAT );
		} else {
			printf("%s", FT_OPEN_FILE_ERROR);
		}
		perror( filepath );
		return -2;
	}
	/* 打印字体信息 */
	printf(	"=============== font info ==============\n" 
		"family name: %s\n"
		"style name : %s\n"
		"========================================\n" ,
		face->family_name, face->style_name );

	/* 设定为UNICODE，默认的也是 */
	FT_Select_Charmap( face, FT_ENCODING_UNICODE );
	/* 记录字体信息至数据库中 */
	id = FontLIB_AddFontInfo( face->family_name, face->style_name, filepath, face );
#else
	printf("%s: %s\n", __FUNCTION__, "warning: not font engine support!");
#endif
	return id;
}

/* 移除指定ID的字体数据 */
int FontLIB_DeleteFontInfo( int id )
{
	return 0;
}

static int FontLIB_AddInCoreFontInfo( void )
{
	int font_id;
	font_id = FontLIB_AddFontInfo( "in-core.font_8x8", "default", "\0", NULL );
	in_core_font = FontLIB_GetFont( font_id );
	if( !in_core_font ) {
		return -1;
	}
	return font_id;
}

#ifdef LCUI_BUILD_IN_WIN32
#define MAX_FONTFILE_NUM	4
#else
#define MAX_FONTFILE_NUM	1
#endif

/** 初始化字体处理模块 */
void LCUIModule_Font_Init( void )
{
	char *p;
	int font_id = -1, i;
	char *fontfilelist[MAX_FONTFILE_NUM]={
#ifdef LCUI_BUILD_IN_WIN32
		"C:/Windows/Fonts/msyh.ttf",
		"C:/Windows/Fonts/msyh.ttc",
		"C:/Windows/Fonts/simsun.ttc",
		"C:/Windows/Fonts/consola.ttf"
#else
		"../fonts/msyh.ttf"
#endif
	};

	printf("loading fontfile...\n");
	FontLIB_Init(); /* 初始化字体数据库 */
	FontLIB_AddInCoreFontInfo(); /* 添加内置的字体信息 */
	/* 如果在环境变量中定义了字体文件路径，那就使用它 */
	p = getenv("LCUI_FONTFILE");
	if( p != NULL ) {
		font_id = FontLIB_LoadFontFile( p );
	}
	/* 如果载入成功，则设定该字体为默认字体 */
	if(font_id > 0) {
		FontLIB_SetDefaultFont( font_id );
		return;
	}
	/* 否则载入失败就载入其它字体文件 */
	for(i=0; i<MAX_FONTFILE_NUM; ++i) {
		font_id = FontLIB_LoadFontFile( fontfilelist[i] ); 
		if(font_id > 0) {
			FontLIB_SetDefaultFont( font_id );
			break;
		}
	}
}

/** 停用字体处理模块 */
void LCUIModule_Font_End( void )
{
	FontLIB_DestroyAll();
}
