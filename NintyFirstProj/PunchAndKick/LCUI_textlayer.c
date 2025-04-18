/* ***************************************************************************
 * textlayer.c -- text bitmap layer processing module.
 * 
 * Copyright (C) 2012-2013 by
 * Liu Chao
 * 
 * This file is part of the LCUI project, and may only be used, modified, and
 * distributed under the terms of the GPLv2.
 * 
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 * 
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *  
 * The LCUI project is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for more details.
 * 
 * You should have received a copy of the GPLv2 along with this file. It is 
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/
 
/* ****************************************************************************
 * textlayer.c -- 文本图层处理模块
 *
 * 版权所有 (C) 2012-2013 归属于
 * 刘超
 * 
 * 这个文件是LCUI项目的一部分，并且只可以根据GPLv2许可协议来使用、更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 * 
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 * 
 * LCUI 项目是基于使用目的而加以散布的，但不负任何担保责任，甚至没有适销性或特
 * 定用途的隐含担保，详情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>. 
 * ****************************************************************************/
//#define DEBUG
//#define DEBUG1
//#define DEBUG2
#include <LCUI_Build.h>

#include LC_LCUI_H 
#include LC_GRAPH_H
#include LC_FONT_H

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Special_KeyWord	Special_KeyWord;
/*************************** 特殊关键词 *****************************/
struct _Special_KeyWord
{
	LCUI_String keyword;		/* 关键词 */
	LCUI_TextStyle *data;		/* 该关键词使用的字体数据 */
	LCUI_Queue *text_source_data;	/* 关键词所属的源文本 */
};
/******************************************************************/

static void 
Destroy_Special_KeyWord( void *arg )
{
	/* 在该关键词所属的原文本中查找组成该关键词的字，并修改字所使用的字体数据 */
}

static void 
Destroy_CharData( void *arg )
{
	
}

static void 
Destroy_Text_RowData( void *arg )
{
	Text_RowData *data;
	data = (Text_RowData *)arg;
	Queue_Destroy ( &data->string );
}

static void TextLayer_Clear(	LCUI_TextLayer *layer, 
				LCUI_Pos pos, 
				int max_h, 
				LCUI_CharData *char_ptr )
{
	LCUI_Rect area;
	DEBUG_MSG("pos: %d,%d, max_h: %d\n", pos.x, pos.y, max_h);
	if( layer->password_char.char_code != 0 ) {
		char_ptr = &layer->password_char;
	}
	/* 计算区域范围 */
	area.x = pos.x + char_ptr->bitmap->left;
	area.y = pos.y + max_h-1;
	area.y -= char_ptr->bitmap->top;
	area.width = char_ptr->bitmap->width;
	area.height = char_ptr->bitmap->rows;
	/* 记录需刷新的区域 */
	RectQueue_AddToValid( &layer->clear_area, area );
	DEBUG_MSG("record area: %d,%d,%d,%d\n", area.x, area.y, area.width, area.height);
}


/** 获取字体位图，字体的样式由文本图层中记录的字体样式决定 */
static void TextLayer_GetCharBMP( LCUI_TextStyle *default_style, LCUI_CharData *data )
{
	int pixel_size;
	/* 获取字体尺寸 */
	if( !data->data ) {
		pixel_size = default_style->pixel_size;
	} else {
		/* 如果已经自定义字体尺寸 */
		if( data->data->_pixel_size ) {
			pixel_size = data->data->pixel_size; 
		} else {
			pixel_size = default_style->pixel_size;
		}
	}
	data->bitmap = FontLIB_GetExistFontBMP( default_style->font_id, 
			data->char_code, pixel_size );
}

/** 向文本图层中的文本中添加新行 */
static int TextLayer_Text_AddNewRow( LCUI_TextLayer *layer )
{
	Text_RowData data; 
	/* 单整行最大尺寸改变时，需要移动整行，目前还未支持此功能 */
	data.pos = Pos(0,0); 
	data.max_size = Size(0,0);
	data.last_char = NULL;
	Queue_Init( &data.string, sizeof(LCUI_CharData), NULL );
	/* 使用链表模式，方便数据的插入 */
	Queue_SetDataMode( &data.string, QUEUE_DATA_MODE_LINKED_LIST );
	/* 队列成员使用指针，主要是引用text_source_data里面的数据 */
	Queue_UsingPointer( &data.string );
	if( Queue_Add( &layer->rows_data, &data ) ) {
		return 0;
	}
	return -1;
}

/** 对目标行进行断行处理，也就是将目标行指定位置后面的全部文字转移到另一行 */
static void  TextLayer_Text_RowBreak(	LCUI_TextLayer *layer,		
					Text_RowData *src, 
					int break_point,
					Text_RowData *des )
{
	int i, total;
	LCUI_CharData *char_ptr;
	
	total = Queue_GetTotal( &src->string );
	for(i=break_point; i<total; ++i ) {
		char_ptr = (LCUI_CharData*)Queue_Get( &src->string, break_point );
		Queue_AddPointer( &des->string, char_ptr );
		char_ptr->need_update = TRUE;
		Queue_DeletePointer( &src->string, break_point );
	}
}

/** 将插入新行至文本图层的文本中的指定位置 */
static int TextLayer_Text_InsertNewRow( LCUI_TextLayer *layer, int row )
{
	LCUI_TextStyle *style;
	Text_RowData data;
	
	data.pos = Pos(0,0); 
	data.last_char = NULL;
	Queue_Init( &data.string, sizeof(LCUI_CharData), NULL ); 
	Queue_SetDataMode( &data.string, QUEUE_DATA_MODE_LINKED_LIST ); 
	Queue_UsingPointer( &data.string );
	/* 根据当前样式，判定当前行的尺寸 */
	style = StyleTag_GetCurrentStyle( &layer->tag_buff );
	if( style && style->_pixel_size ) {
		data.max_size.h = style->pixel_size+2;
		free(style);
	} else {
		data.max_size.h = layer->default_data.pixel_size+2;
	}
	data.max_size.w = 0;
	if( Queue_Insert( &layer->rows_data, row, &data ) ) {
		return 0;
	}
	return -1;
}

/** 获取指向当前行的指针 */
static Text_RowData *TextLayer_GetCurRowData( LCUI_TextLayer *layer )
{
	return (Text_RowData*)Queue_Get( &layer->rows_data, 
				layer->current_des_pos.y );
}

/** 更新指定行文本位图的尺寸 */
static void TextLayer_Update_RowSize( LCUI_TextLayer *layer, int row )
{
	int total, i;
	LCUI_Size size;
	LCUI_CharData *char_data;
	Text_RowData *row_data;
	LCUI_TextStyle *style;
	
	row_data = (Text_RowData*)Queue_Get( &layer->rows_data, row );
	total = Queue_GetTotal( &row_data->string ); 
	style = StyleTag_GetCurrentStyle( &layer->tag_buff ); 
	size.w = 0;
	size.h = layer->default_data.pixel_size+2;
	if( style ) {
		if(style->pixel_size > 0) {
			size.h = style->pixel_size+2;
		} 
		free( style );
	}
	for( i=0; i<total; ++i ) {
		/* 如果屏蔽字符有效，则使用该字符的数据 */
		if( layer->password_char.char_code > 0 ) {
			char_data = &layer->password_char;
		} else {
			char_data = (LCUI_CharData*)
			Queue_Get( &row_data->string, i );
		}
		if( !char_data || !char_data->bitmap || !char_data->display ) {
			continue;
		}
		size.w += char_data->bitmap->advance.x;
		if( !char_data->data || !char_data->data->_pixel_size ) {
			if( size.h < layer->default_data.pixel_size+2 ) {
				size.h = layer->default_data.pixel_size+2;
			}
			continue;
		}
		if( size.h < char_data->data->pixel_size + 2 ) {
			size.h = char_data->data->pixel_size + 2;
		}
	}
	row_data->max_size = size;
}

/** 初始化文本图层相关数据 */
LCUI_API void TextLayer_Init( LCUI_TextLayer *layer )
{
	layer->read_only = FALSE;
	layer->using_code_mode = FALSE; 
	layer->using_style_tags = FALSE; 
	layer->enable_word_wrap = FALSE; 
	layer->enable_multiline = FALSE;
	layer->need_scroll_layer = FALSE;
	layer->have_select = FALSE;
	layer->auto_wrap = FALSE;
	layer->start = 0;
	layer->end = 0;
	layer->offset_pos = Pos(0,0);
	layer->old_offset_pos = Pos(0,0);
	
	Queue_Init( &layer->color_keyword, sizeof(Special_KeyWord), Destroy_Special_KeyWord );
	/* 队列中使用链表储存这些数据 */
	Queue_Init( &layer->text_source_data, sizeof(LCUI_CharData), Destroy_CharData );
	Queue_SetDataMode( &layer->text_source_data, QUEUE_DATA_MODE_LINKED_LIST ); 
	Queue_Init( &layer->rows_data, sizeof(Text_RowData), Destroy_Text_RowData ); 
	StyleTag_Init( &layer->tag_buff );
	RectQueue_Init( &layer->clear_area );
	Graph_Init( &layer->graph );
	layer->graph.color_type = COLOR_TYPE_RGBA;
	/* 初始化屏蔽符的数据 */
	layer->password_char.display = TRUE;
	layer->password_char.need_update = FALSE;
	layer->password_char.data = NULL;
	layer->password_char.char_code = 0;
	layer->password_char.bitmap = NULL;
	
	layer->default_data.pixel_size = 12;
	layer->current_src_pos = 0;
	layer->current_des_pos = Pos(0,0);
	layer->max_text_len = 512000;
	TextStyle_Init( &layer->default_data );
	//TextLayer_Text_AddNewRow ( layer );/* 添加新行 */
}

/** 销毁文本图层占用的资源 */
LCUI_API void Destroy_TextLayer( LCUI_TextLayer *layer )
{
	Queue_Destroy( &layer->text_source_data );
	Queue_Destroy( &layer->rows_data );
	Queue_Destroy( &layer->tag_buff );
	Graph_Free( &layer->graph );
	RectQueue_Destroy( &layer->clear_area );
}

/** 对指定文本行进行自动换行处理 */
static void TextLayer_TextRowTypeset( LCUI_TextLayer *layer, int i_row )
{
	LCUI_Size row_size={0,0};
	Text_RowData *p_row, *p_next_row;
	LCUI_CharData *p_char;
	int i_col, n_cols, char_h;

	/* 获取本行的指针 */
	p_row = (Text_RowData*)Queue_Get( &layer->rows_data, i_row );
	/* 获取本行文本的字符列数 */
	n_cols = Queue_GetTotal( &p_row->string );
	/* 遍历当前行的各个文字 */
	for(i_col=0; i_col<n_cols; ++i_col) {
		p_char = (LCUI_CharData*)
		Queue_Get( &p_row->string, i_col );
		/* 忽略无效、无字体位图、不可显的文字 */
		if( !p_char || !p_char->bitmap || !p_char->display ) {
			continue;
		}
		/* 获取该字的高度 */
		if( p_char->data && p_char->data->_pixel_size ) {
			char_h = p_char->data->pixel_size+2;
		} else {
			char_h = layer->default_data.pixel_size+2;
		}
		row_size.w += p_char->bitmap->advance.x;
		/* 如果是当前行的第一个字符，或者行宽度没有超过宽度限制 */
		if( i_col < 1 || !layer->auto_wrap
		 || row_size.w <= layer->graph.w ) {
			if( row_size.h < char_h ) {
				row_size.h = char_h;
			}
			continue;
		}
		row_size.w -= p_char->bitmap->advance.x;
		/* 更新本行的尺寸 */
		p_row->max_size.w = row_size.w;
		p_row->max_size.h = row_size.h;
		/* 如果本行末尾有换行符，则插入新行 */
		if( p_row->last_char ) {
			TextLayer_Text_InsertNewRow( layer, i_row+1 );
			p_next_row = (Text_RowData*)
			Queue_Get( &layer->rows_data, i_row+1 );
			/* 将本行的换行符转移至新行 */
			p_next_row ->last_char = p_row->last_char;
			p_row->last_char = NULL;
		}
		/* 获取下一行的指针 */
		p_next_row = (Text_RowData*)
		Queue_Get( &layer->rows_data, i_row+1 );
		/* 若没有下一行，则插入新行 */
		if( !p_next_row ) {
			TextLayer_Text_InsertNewRow( layer, i_row+1 );
			p_next_row = (Text_RowData*)
			Queue_Get( &layer->rows_data, i_row+1 );
		}
		row_size.w = 0;
		row_size.h = 0;
		/* 将本行剩余文字转移至下一行 */
		for(n_cols-=1; n_cols>=i_col; --n_cols) {
			p_char = (LCUI_CharData*)
			Queue_Get( &p_row->string, n_cols );
			/* 标记本字需要刷新 */
			p_char->need_update = TRUE;
			/* 插入至新行 */
			Queue_InsertPointer( &p_next_row->string, 0, p_char );
			if( p_char->data && p_char->data->_pixel_size ) {
				char_h = p_char->data->pixel_size+2;
			} else {
				char_h = layer->default_data.pixel_size+2;
			}
			if( row_size.h < char_h ) {
				row_size.h = char_h;
			}
			row_size.w += p_char->bitmap->advance.x;
			/* 移除该字在本行的记录 */
			Queue_DeletePointer( &p_row->string, n_cols );
		}
		/* 更新本行的尺寸 */
		p_next_row->max_size = row_size;
		return;
	}
	if( n_cols <= 0 ) {
		TextLayer_Update_RowSize( layer, i_row );
	}
	else if( i_col >= n_cols ) {
		p_row->max_size = row_size;
	}
	/* 如果本行有结束符（换行符） */
	if( p_row->last_char ) {
		return;
	}
	/* 获取本行尺寸，以便下面继续累计本行的尺寸 */
	row_size = p_row->max_size;
	/* 本行内容的宽度未达到限制宽度，需要将下行的文本转移至本行 */
	while(1) {
		p_next_row = (Text_RowData*)
		Queue_Get( &layer->rows_data, i_row+1 );
		/* 若没有下行 */
		if( !p_next_row ) {
			p_row->max_size = row_size;
			return;
		}
		while(1) {
			p_char = (LCUI_CharData*)
			Queue_Get( &p_next_row->string, 0 );
			if( !p_char ) {
				p_row->max_size = row_size;
				break;
			}
			/* 忽略无字体位图、不可显的文字 */
			if( !p_char->bitmap || !p_char->display ) {
				Queue_AddPointer( &p_row->string, p_char );
				Queue_DeletePointer( &p_next_row->string, 0 );
				continue;
			}
			/* 获取该字的高度 */
			if( p_char->data && p_char->data->_pixel_size ) {
				char_h = p_char->data->pixel_size+2;
			} else {
				char_h = layer->default_data.pixel_size+2;
			}
			row_size.w += p_char->bitmap->advance.x;
			/* 如果没有超过宽度限制 */
			if( !layer->auto_wrap || row_size.w < layer->graph.w ) {
				if( row_size.h < char_h ) {
					row_size.h = char_h;
				}
				/* 标记本字需要刷新 */
				p_char->need_update = TRUE;
				Queue_AddPointer( &p_row->string, p_char );
				Queue_DeletePointer( &p_next_row->string, 0 );
				continue;
			}
			/* 超过了就更新本行的尺寸，然后退出 */
			row_size.w -= p_char->bitmap->advance.x;
			p_row->max_size = row_size;
			return;
		}
		/* 如果这一行有结束符 */
		if( p_next_row->last_char ) {
			/* 将结束符转移至当前行 */
			p_row->last_char = p_next_row->last_char;
			p_next_row->last_char = NULL;
			/* 然后删除这一行 */
			Queue_DeletePointer( &layer->rows_data, i_row+1 );
			return;
		}
		/* 删除这一行，因为这一行的内容已经转移至当前行 */
		Queue_DeletePointer( &layer->rows_data, i_row+1 );
	}
}

/** 对文本图层中的文本进行排版 */
static void TextLayer_TextTypeset( LCUI_TextLayer *layer, int start_row )
{
	int i_row, n_rows;
	n_rows = Queue_GetTotal( &layer->rows_data );
	/* 从第start_row 行开始，对后面所有行进行自动换行处理 */
	for(i_row=start_row; i_row<n_rows; ++i_row) {
		TextLayer_TextRowTypeset( layer, i_row );
		/* 更新总行数 */
		n_rows = Queue_GetTotal( &layer->rows_data );
	}
}

/** 更新文本图层中的内容 */
LCUI_API void TextLayer_Update(	LCUI_TextLayer *layer,
				LCUI_Queue *dirty_rect_list )
{
	LCUI_Rect area;
	LCUI_Pos pos, mix_pos;
	LCUI_BOOL draw_all = FALSE, redraw_row;
	int i, j, n, rows;
	LCUI_RGB color;
	LCUI_Graph slot;
	LCUI_CharData *p_data;
	Text_RowData *p_row;
	
	if( !layer ) {
		return;
	}
	/* 如果需要滚动图层 */
	if( layer->need_scroll_layer ) {
		/* 直接刷新整个文本图层 */
		area.x = 0 - layer->offset_pos.x;
		area.y = 0 - layer->offset_pos.y;
		area.width = layer->graph.w;
		area.height = layer->graph.h;
		RectQueue_AddToValid( &layer->clear_area, area );
		draw_all = TRUE;
		layer->need_scroll_layer = FALSE;
	}
	
	Graph_Init( &slot );
	/* 切换可用队列为当前占用的队列 */
	RectQueue_Switch( &layer->clear_area );
	/* 先处理需要清空的区域 */
	while( RectQueue_GetFromCurrent(&layer->clear_area, &area) ) { 
		area.x += layer->offset_pos.x;
		area.y += layer->offset_pos.y;
		Graph_Quote( &slot, &layer->graph, area );
		/* 将该区域的alpha通道填充为0 */
		Graph_FillAlpha( &slot, 0 );
		if( dirty_rect_list ) {
			Queue_Add( dirty_rect_list, &area ); 
		}
	}
	/* 开始绘制文本位图至目标图层上 */
	rows = Queue_GetTotal( &layer->rows_data );
	DEBUG_MSG("tip1, rows: %d\n", rows);
	for(pos.y=layer->offset_pos.y,i=0; i<rows; ++i) {
		redraw_row = FALSE;
		p_row = (Text_RowData*)Queue_Get( &layer->rows_data, i );
		if( !p_row ) {
			DEBUG_MSG("tip1.1\n");
			continue;
		}
		/* 如果当前字的位图的Y轴跨距不在有效绘制区域内 */
		if( pos.y + p_row->max_size.h <= 0 ) {
			pos.y += p_row->max_size.h;
			DEBUG_MSG("tip1.2, pos.y: %d, p_row->max_size.h: %d\n", pos.y, p_row->max_size.h);
			continue;
		}
		n = Queue_GetTotal( &p_row->string );
		DEBUG_MSG("tip2, row %d, len: %d\n", i, n);
		for(pos.x=layer->offset_pos.x,j=0; j<n; ++j) {
			p_data = (LCUI_CharData*)Queue_Get( &p_row->string, j ); 
			if( !p_data || !p_data->bitmap ) {
				continue;
			}
			/* 如果设置了屏蔽符 */
			if( layer->password_char.char_code > 0 ) {
				layer->password_char.need_update = p_data->need_update;
				p_data = &layer->password_char;
			}
			/* 如果当前字的位图的X轴跨距不在有效绘制区域内 */
			if( pos.x + p_data->bitmap->advance.x <= 0) {
				pos.x += p_data->bitmap->advance.x;
				continue;
			}
			/* 获取该字体位图的颜色 */
			if( p_data->data && p_data->data->_fore_color ) {
				color = p_data->data->fore_color;
			} else {
				color = layer->default_data.fore_color;
			}
			DEBUG_MSG("char: %d, pos: %d,%d, redraw: %d\n", 
				j, pos.x, pos.y, p_data->need_update || draw_all);
			/* 如果字体位图已标记更新，则绘制它 */
			if( p_data->need_update || draw_all ) {
				p_data->need_update = FALSE;
				if( !redraw_row ) {
					area.x = pos.x;
					area.height = p_row->max_size.h;
					redraw_row = TRUE;
				}
				mix_pos.x = pos.x + p_data->bitmap->left;
				mix_pos.y = pos.y + p_row->max_size.h-1;
				mix_pos.y -= p_data->bitmap->top;
				/* 贴上字体位图 */
				FontBMP_Mix(	&layer->graph, 
						mix_pos, 
						p_data->bitmap,
						color, 
						GRAPH_MIX_FLAG_REPLACE );
			}
			pos.x += p_data->bitmap->advance.x;
			if( pos.x > layer->graph.w ) {
				break;
			}
		}
		if(redraw_row) {
			area.y = pos.y;
			area.width = pos.x - area.x;
			//_DEBUG_MSG("area:%d,%d,%d,%d\n", 
			//area.x, area.y, area.width, area.height);
			Queue_Add( dirty_rect_list, &area );
		}
		pos.y += p_row->max_size.h;
		if( pos.y > layer->graph.h ) {
			break;
		}
	}
	DEBUG_MSG("tip3\n");
}

/* 标记需要重绘整个文本图层 */
LCUI_API void TextLayer_Redraw( LCUI_TextLayer *layer )
{
	int i, j;
	int rows, len;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;
	LCUI_Rect area;

	area.x = 0;
	area.y = 0;
	area.width = layer->graph.w;
	area.height = layer->graph.h;
	/* 直接清除整个图层内容 */
	RectQueue_AddToValid( &layer->clear_area, area );

	rows = Queue_GetTotal( &layer->rows_data );
	for(i=0; i<rows; ++i) {
		row_ptr = (Text_RowData*)
		Queue_Get( &layer->rows_data, i );
		len = Queue_GetTotal( &row_ptr->string );
		for(j=0; j<len; ++j) {
			char_ptr = (LCUI_CharData*)
			Queue_Get( &row_ptr->string, j );
			if( !char_ptr ) {
				continue;
			}
			char_ptr->need_update = TRUE; 
		}
	}
}

/* 刷新整个文本图层，让各个字体位图得到更新 */
LCUI_API void TextLayer_Refresh( LCUI_TextLayer *layer )
{
	int i, j;
	int rows, len;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;
	LCUI_Rect area={0,0,0,0};
	
	rows = Queue_GetTotal( &layer->rows_data );
	for(i=0; i<rows; ++i) {
		row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, i );
		len = Queue_GetTotal( &row_ptr->string );
		for(j=0; j<len; ++j) {
			char_ptr = (LCUI_CharData*)Queue_Get( &row_ptr->string, j );
			if( !char_ptr ) {
				continue;
			}
			/* 重新获取字体位图 */
			TextLayer_GetCharBMP( &layer->default_data, char_ptr );
			char_ptr->need_update = TRUE; 
		}
		TextLayer_Update_RowSize( layer, i );
		area.height += row_ptr->max_size.h;
		/* 稍微扩大一点区域，区域高度再+2 */
		area.height += 2;
		if( area.width < row_ptr->max_size.w ) {
			area.width = row_ptr->max_size.w;
		}
	}
	/* 记录整个文本图层的内容需要清除 */
	RectQueue_AddToValid( &layer->clear_area, area );
}

/* 
 * 功能：设定文本图层的偏移位置
 * 返回值：需要对图层进行重绘时返回0，否则返回1
 *  */
LCUI_API int
TextLayer_SetOffset( LCUI_TextLayer *layer, LCUI_Pos offset_pos )
{
	/* 如果之前已经标记不需要滚动，那么就记录当前的偏移位置 */
	if( !layer->need_scroll_layer ) {
		layer->old_offset_pos = layer->offset_pos;
	}
	if( layer->offset_pos.x == offset_pos.x
	&& layer->offset_pos.y == offset_pos.y ) {
		return 1;
	}
	layer->offset_pos = offset_pos;
	layer->need_scroll_layer = TRUE;
	return 0;
}

/** 设置文本图层的图像尺寸 */
LCUI_API int TextLayer_SetGraphSize(	LCUI_TextLayer *layer, 
					LCUI_Size new_size )
{
	int ret, old_w;
	/* 如果尺寸有变化，则标记需要重绘文本位图 */
	if( layer->graph.w != new_size.w || layer->graph.h != new_size.h ) {
		TextLayer_Redraw( layer );
	}
	old_w = layer->graph.w;
	ret = Graph_Create( &layer->graph, new_size.w, new_size.h );
	/* 如果宽度有变化，并启用了自动换行功能 */
	if( old_w != new_size.w && layer->auto_wrap ) {
		TextLayer_TextTypeset( layer, 0 );
	}
	return ret;
}

/** 计算文本图层的尺寸 */
LCUI_API int TextLayer_GetSize( LCUI_TextLayer *layer, LCUI_Size *layer_size )
{
	int i, cols, max_cols, rows;
	LCUI_Size size={0,0};
	Text_RowData *p_row; 
	
	if( !layer ) {
		return -1;
	}
	rows = Queue_GetTotal( &layer->rows_data );
	if( layer->password_char.char_code > 0 ) {
		max_cols = 0;
		for(i=0; i<rows; ++i) {
			p_row = (Text_RowData*)
			Queue_Get( &layer->rows_data, i );
			cols = Queue_GetTotal( &p_row->string );
			if( max_cols < cols ) {
				max_cols = cols;
			}
		}
		size.w = layer->password_char.bitmap->advance.x;
		size.w *= max_cols;
		size.h = layer->default_data.pixel_size + 4;
		size.h *= rows;
	} else {
		for(size.w=0,size.h=0,i=0; i<rows; ++i) {
			p_row = (Text_RowData*)
			Queue_Get( &layer->rows_data, i );
			if( !p_row ) {
				continue;
			}
			if( size.w < p_row->max_size.w ) {
				size.w = p_row->max_size.w;
			}
			size.h += p_row->max_size.h;
		}
	}
	/* 尺寸稍微搞大一点，因为显示文本光标需要一定空间 */
	size.w += 2;
	size.h += 2;
	*layer_size = size;
	return 0;
}

/** 获取文本图层的图像数据 */
LCUI_API LCUI_Graph *TextLayer_GetGraph( LCUI_TextLayer *layer )
{
	return &layer->graph;
}

/* 获取文本图层中的文本内容 */
LCUI_API size_t
TextLayer_GetText( LCUI_TextLayer *layer, wchar_t *buff, size_t max_len )
{
	unsigned int i, text_len;
	LCUI_CharData *char_p;

	if( max_len == 0 ) {
		return 0;
	}
	text_len = Queue_GetTotal( &layer->text_source_data );
	if( text_len <= 0 ) {
		return 0;
	}
	max_len -= 1; /* 留一个位置保存结束符 */
	if( max_len > text_len ) {
		max_len = text_len;
	}
	for( i=0; i<max_len; ++i ) {
		char_p = (LCUI_CharData *)Queue_Get( &layer->text_source_data, i );
		buff[i] = char_p->char_code;
	}
	buff[i] = '\0';
	return i;
}

/** 刷新指定行中指定字以及后面的字的区域 */
LCUI_API void TextLayer_RefreshCharBehind( LCUI_TextLayer *layer, LCUI_Pos char_pos )
{
	int i, len;
	LCUI_Rect area;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;

	row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, char_pos.y );
	len = Queue_GetTotal( &row_ptr->string );
	/* 如果设置了屏蔽符，则直接计算出坐标 */
	if( layer->password_char.char_code > 0
	 && layer->password_char.bitmap ) {
		area.y = layer->password_char.bitmap->advance.y + 2;
		area.y *= char_pos.y;
		area.x = layer->password_char.bitmap->advance.x;
		area.x *= char_pos.x;
		area.width = layer->password_char.bitmap->advance.x;
		area.width *= len;
		area.width -= area.x;
		area.height = layer->password_char.bitmap->advance.y;
	} else {
		/* 获取该行文字的起点Y轴坐标 */
		for( area.y=0,i=0; i<char_pos.y; ++i ) {
			row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, i );
			if( !row_ptr ) {
				continue;
			}
			area.y += row_ptr->max_size.h;
		}
		/* 遍历各个文字，累计该字的X轴坐标 */
		for( area.x=0,i=0; i<char_pos.x; ++i ) {
			char_ptr = (LCUI_CharData*)
			Queue_Get( &row_ptr->string, i ); 
			if( !char_ptr || !char_ptr->bitmap ) {
				continue;
			}
			area.x += char_ptr->bitmap->advance.x;
		}
		area.width = row_ptr->max_size.w - area.x;
		area.height = row_ptr->max_size.h;
	}
	/* 该字后面的字都需要重绘 */
	for( i=char_pos.x; i<len; ++i ) {
		char_ptr = (LCUI_CharData*)
		Queue_Get( &row_ptr->string, i );
		if( !char_ptr ) {
			continue;
		}
		/* 标记该字的位图需要重绘 */
		char_ptr->need_update = TRUE;
	}
	/* 记录需刷新的区域 */
	RectQueue_AddToValid( &layer->clear_area, area );
	DEBUG_MSG("record area: %d,%d,%d,%d\n", area.x, area.y, area.width, area.height);
}

LCUI_API void
TextLayer_Text_SetDefaultStyle( LCUI_TextLayer *layer, LCUI_TextStyle style )
/* 设定默认的文本样式，需要调用TextLayer_Draw函数进行文本位图更新 */
{
	layer->default_data = style; 
	TextLayer_Refresh( layer );
}

LCUI_API void
TextLayer_ReadOnly( LCUI_TextLayer *layer, LCUI_BOOL flag )
/* 指定文本图层中的文本是否为只读 */
{
	layer->read_only = flag;
}

LCUI_API void
TextLayer_Text_Clear( LCUI_TextLayer *layer )
/* 清空文本内容 */
{
	if( layer == NULL ) {
		return;
	}
	/* 刷新文本图层，记录当前文字的区域，以在绘制时抹去残余文字位图 */
	TextLayer_Refresh( layer );
	/* 销毁现有数据 */
	Queue_Destroy( &layer->text_source_data );
	Queue_Destroy( &layer->rows_data );
	Queue_Destroy( &layer->tag_buff );
	/* 再初始化 */
	Queue_Init( &layer->text_source_data, sizeof(LCUI_CharData), Destroy_CharData );
	Queue_SetDataMode( &layer->text_source_data, QUEUE_DATA_MODE_LINKED_LIST ); 
	Queue_Init( &layer->rows_data, sizeof(Text_RowData), Destroy_Text_RowData ); 
	StyleTag_Init( &layer->tag_buff );
	/* 重置相关变量 */
	layer->offset_pos.x = 0;
	layer->offset_pos.y = 0;
	layer->current_src_pos = 0;
	layer->current_des_pos.x = 0;
	layer->current_des_pos.y = 0;
}

LCUI_API void
TextLayer_SetRowEnd( LCUI_TextLayer *layer, uint_t row, uint_t start_cols )
/* 为指定行设定结束点，结束点及后面的数据将被删除，但不记录残余文本位图区域 */
{
	uint_t total, i;
	Text_RowData *row_data; 
	
	row_data = Queue_Get( &layer->rows_data, row );
	total = Queue_GetTotal( &row_data->string );
	/* 移除多余的数据 */
	for(i=start_cols; i<total; ++i) {
		Queue_Delete( &row_data->string, start_cols ); 
	}
}

LCUI_API int
TextLayer_Text_GetTotalLength( LCUI_TextLayer *layer )
/* 获取文本位图中的文本长度 */
{
	return Queue_GetTotal( &layer->text_source_data );
}

LCUI_API void
TextLayer_Text_Set_MaxLength( LCUI_TextLayer *layer, int max )
/* 设定文本位图中的文本长度 */
{
	if( max > 0 ) {
		layer->max_text_len = max;
	}
	// 对现有文本进行截断处理，暂不添加添加
}

LCUI_API void
TextLayer_Text_SetPasswordChar( LCUI_TextLayer *layer, wchar_t ch )
/* 
 * 设置屏蔽字符，设置后，文本框内的文本都会显示成该字符
 * 如果ch的值为0，则不对文本框里的文本进行屏蔽 
 * */
{
	layer->password_char.char_code = ch;
	TextLayer_GetCharBMP( &layer->default_data, &layer->password_char );
	//暂时不进行其它处理
}

/** 对文本进行预处理，处理后的数据保存至layer里 */ 
LCUI_API void TextLayer_Text_Process(	LCUI_TextLayer *layer,
					int pos_type,
					wchar_t *new_text )
{
	LCUI_BOOL need_refresh_row = FALSE;
	LCUI_Pos cur_pos, des_pos;
	int total, cur_len, row, src_pos, total_row, n_ignore = 0;
	const wchar_t *finish, *p, *q;
	
	LCUI_Pos tmp_pos;
	LCUI_CharData *char_ptr, char_data; 
	Text_RowData *cur_row_ptr, *tmp_row_ptr;

	int char_h, char_pos_x, i;
	LCUI_Size row_size;
	
	if( new_text == NULL ) {
		return;
	}
	/* 如果有选中的文本，那就删除 */
	//......  
	DEBUG_MSG1("enter\n");
	/* 如果是将文本追加至文本末尾 */
	if( pos_type == AT_TEXT_LAST ) {
		cur_pos.y = Queue_GetTotal( &layer->rows_data );
		if( cur_pos.y > 0 ) {
			--cur_pos.y;
		}
		cur_row_ptr = (Text_RowData*)
		Queue_Get( &layer->rows_data, cur_pos.y );
		if( !cur_row_ptr ) {
			TextLayer_Text_AddNewRow( layer );
			cur_row_ptr = (Text_RowData*)
			Queue_Get( &layer->rows_data, cur_pos.y );
		}
		cur_pos.x = Queue_GetTotal( &cur_row_ptr->string );
		src_pos = Queue_GetTotal( &layer->text_source_data );
		des_pos = cur_pos;
	} else {/* 否则，是将文本插入至光标所在位置 */
		cur_pos = TextLayer_Cursor_GetPos( layer );
		DEBUG_MSG1( "cur_pos: %d,%d\n", cur_pos.x, cur_pos.y );
		cur_row_ptr = (Text_RowData*)
		Queue_Get( &layer->rows_data, cur_pos.y );
		DEBUG_MSG1( "cur_row_ptr: %p\n", cur_row_ptr );
		if( !cur_row_ptr ) {
			TextLayer_Text_AddNewRow( layer );
			cur_row_ptr = (Text_RowData*)
			Queue_Get( &layer->rows_data, cur_pos.y );
		}
		src_pos = layer->current_src_pos;
		des_pos = layer->current_des_pos;
		DEBUG_MSG1( "src_pos: %d\n", src_pos );
		DEBUG_MSG1( "des_pos: %d,%d\n", des_pos.x, des_pos.y );
	}
	row = cur_pos.y;
	total = wcslen( new_text );
	total_row = TextLayer_GetRows( layer );
	/* 记录当前行的尺寸 */
	row_size = cur_row_ptr->max_size;
	/* 判断当前要添加的字符的总数是否超出最大限制 */
	cur_len = Queue_GetTotal( &layer->text_source_data );
	if( (uint32_t)(total+cur_len) > layer->max_text_len ) {
		total = layer->max_text_len - cur_len;
	}
	if( total < 0 ) {
		total = 0;
	}
	/* 先计算当前插入点的X轴坐标 */
	for(char_pos_x=0,i=0; i<des_pos.x; ++i) {
		char_ptr = (LCUI_CharData*)
		Queue_Get( &cur_row_ptr->string, i );
		if( char_ptr && char_ptr->bitmap && char_ptr->display ) {
			char_pos_x += char_ptr->bitmap->advance.x;
		}
	}
	//_DEBUG_MSG( "layer: %p, cur total: %d, max_len: %d\n", 
	// layer, cur_len, layer->max_text_len );
	DEBUG_MSG1( "total char: %d\n", total );
	DEBUG_MSG1( "total row: %d\n", total_row );
	/* 先记录这一行需要刷新的区域，起点为光标所在位置 */
	TextLayer_RefreshCharBehind( layer, cur_pos );
	for(p=new_text, finish=new_text+total; p<finish; ++p) {
		n_ignore = -1;
		DEBUG_MSG2( "1, char: %c\n", *p );
		if( layer->using_style_tags ) {
			/* 处理样式的结束标签 */ 
			q = StyleTag_ProcessEndingTag( &layer->tag_buff, p );
			if( q ) {
				/* 计算需忽略的字符数 */
				n_ignore = q-p;
			} else {
				/* 处理样式标签 */
				q = StyleTag_ProcessTag( &layer->tag_buff, p );
				if( q ) {
					n_ignore = q-p;
				}
			}
		}
		/* 针对换行符模式为Win(CR/LF)的文本，进行处理 */
		if( *p == '\n' || *p == '\r' ) { 
			/* 计算需要忽略的换行符的数量 */
			for( n_ignore=0,q=p; *q=='\n' || *q=='\r'; ++q,++n_ignore);
		} 
		if( n_ignore > 0 ) {
			/* 被忽略的字符的属性都一样，所以只需赋一次值 */
			char_data.data = NULL;
			char_data.display = FALSE; 
			char_data.need_update = FALSE;
			char_data.bitmap = NULL;
			tmp_pos.x = des_pos.x;
			tmp_pos.y = des_pos.y;
		}
		for( ;n_ignore>0; --n_ignore,++src_pos ) {
			char_data.char_code = *p++;
			char_ptr = (LCUI_CharData*)Queue_Insert( 
			&layer->text_source_data, src_pos, &char_data ); 
			/* 如果未启用多行文本，或未遇到换行符 */
			if( !layer->enable_multiline
			 || char_data.char_code != '\n' ) {
				continue;
			}
			++row;
			if( !need_refresh_row ) {
				/* 记录当前行的起点位置 */
				tmp_pos.x = 0;
				tmp_pos.y = row;
				/* 标记需要刷新该行后面所有行 */
				need_refresh_row = TRUE;
			}
			/* 换行符的数据单独保存 */
			cur_row_ptr->last_char = char_ptr;
			/* 在该行插入新行 */
			TextLayer_Text_InsertNewRow( layer, row );
			/* 获取新行的行指针 */
			tmp_row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, row );
			/* 更新当前行尺寸 */
			cur_row_ptr->max_size = row_size;
			/* 记录新行的尺寸 */
			row_size = tmp_row_ptr->max_size;
			/* 当前位置后面的字符转移到新行里 */
			TextLayer_Text_RowBreak( layer, cur_row_ptr, des_pos.x, tmp_row_ptr );
			/* 行指针切换到下一行 */
			cur_row_ptr = tmp_row_ptr;
			/* 更新当前字符坐标 */
			des_pos.x = 0;
			des_pos.y = row;
		}
		if( n_ignore == 0 ) {
			/* 更新总行数 */
			total_row = TextLayer_GetRows( layer );
			n_ignore = -1;
			--p;
			continue;
		}
		DEBUG_MSG2( "2, char: %c\n", *p );
		char_data.char_code = *p;
		char_data.display = TRUE; 
		char_data.need_update = TRUE; 
		/* 获取当前文本样式数据 */
		char_data.data = StyleTag_GetCurrentStyle( &layer->tag_buff );
		/* 获取本字的字体位图 */
		TextLayer_GetCharBMP( &layer->default_data, &char_data );
		/* 插入至源文本中 */
		char_ptr = (LCUI_CharData*)
		Queue_Insert( &layer->text_source_data, src_pos, &char_data );
		/* 将该指针添加至行数据队列中 */
		cur_row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, des_pos.y );
		Queue_InsertPointer( &cur_row_ptr->string, des_pos.x, char_ptr );
		
		/* 累计当前行的宽度 */
		row_size.w += char_ptr->bitmap->advance.x;
		/* 获取该字的高度 */
		if( char_ptr->data && char_ptr->data->_pixel_size ) {
			char_h = char_ptr->data->pixel_size+2;
		} else {
			char_h = layer->default_data.pixel_size+2;
		}
		if( row_size.h < char_h ) {
			row_size.h = char_h;
		}

		++src_pos; 
		++des_pos.x;
	}
	/** 如果需要刷新后面所有行的文本 */
	if( need_refresh_row ) {
		/* 刷新该行后面所有行的字符 */
		for( ; tmp_pos.y<total_row; ++tmp_pos.y ) {
			TextLayer_RefreshCharBehind( layer, tmp_pos );
		}
	}
	/** 如果还在同一行内 */
	if( des_pos.y == cur_pos.y ) {
		total = Queue_GetTotal( &cur_row_ptr->string );
		/* 继续累计当前行剩余字符的尺寸 */
		for(i=des_pos.x; i<total; ++i) {
			char_ptr = (LCUI_CharData*)
			Queue_Get( &cur_row_ptr->string, i );
			row_size.w += char_ptr->bitmap->advance.x;
			if( char_ptr->data && char_ptr->data->_pixel_size ) {
				char_h = char_ptr->data->pixel_size+2;
			} else {
				char_h = layer->default_data.pixel_size+2;
			}
			if( row_size.h < char_h ) {
				row_size.h = char_h;
			}
		}
		/* 更新该行的尺寸 */
		cur_row_ptr->max_size = row_size;
	}
	if( pos_type == AT_CURSOR_POS ) {
		layer->current_des_pos = des_pos;
		layer->current_src_pos = src_pos;
	}
	/* 若启用了自动换行 */
	if( layer->auto_wrap ) {
		TextLayer_TextTypeset( layer, cur_pos.y );
	}
	DEBUG_MSG1("quit\n");
}

LCUI_API void
TextLayer_PrintInfo( LCUI_TextLayer *layer )
/* 打印文本图层信息 */
{
	int32_t i, j, len, rows;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;
	
	printf( "layer: %p\n", layer );
	rows = Queue_GetTotal( &layer->rows_data );
	for(j=0; j<rows; ++j) {
		row_ptr = Queue_Get( &layer->rows_data, j );
		len = Queue_GetTotal( &row_ptr->string );
		printf( "row[%d/%d], len: %d\n", j, rows, len );
		for(i=0; i<len; ++i) {
			char_ptr = Queue_Get( &row_ptr->string, i );
			printf( "char code: %lu, display: %d\n", 
			char_ptr->char_code, char_ptr->display );
			FontBMP_PrintInfo( char_ptr->bitmap );
		}
	}
	printf("\n\n");
}

/* 
 * 功能：设定指定的宽字符串作为文本图层中显示的文本
 * 说明：文本将被储存至缓冲区，等待绘制文本位图时再处理缓冲区内的文本
 *  */
LCUI_API void
TextLayer_TextW( LCUI_TextLayer *layer, wchar_t *wchar_text )
{
	if( !layer ) {
		return;
	}
	TextLayer_Text_Clear( layer );
	TextLayer_Text_AddW( layer, wchar_text );
}

LCUI_API void
TextLayer_Text( LCUI_TextLayer *layer, const char *utf8_text )
{
	wchar_t *unicode_text;
	LCUICharset_UTF8ToUnicode( utf8_text, &unicode_text );
	TextLayer_TextW( layer, unicode_text );
	free( unicode_text );
}

LCUI_API void
TextLayer_TextA( LCUI_TextLayer *layer, const char *ascii_text )
{
	wchar_t *unicode_text;
	LCUICharset_GB2312ToUnicode( ascii_text, &unicode_text );
	TextLayer_TextW( layer, unicode_text );
	free( unicode_text );
}

/* 在文本末尾追加文本，不移动光标，不删除原有选中文本 */
LCUI_API int
TextLayer_Text_AppendW( LCUI_TextLayer *layer, wchar_t *new_text )
{
	TextLayer_Text_Process( layer, AT_TEXT_LAST, new_text );
	return 0;
}

LCUI_API int
TextLayer_Text_AppendA( LCUI_TextLayer *layer, char *new_text )
{
	wchar_t *unicode_text;
	LCUICharset_GB2312ToUnicode( new_text, &unicode_text );
	TextLayer_Text_AppendW( layer, unicode_text );
	free( unicode_text );
	return 0;
}

LCUI_API int
TextLayer_Text_Append( LCUI_TextLayer *layer, char *new_text )
{
	wchar_t *unicode_text;
	LCUICharset_UTF8ToUnicode( new_text, &unicode_text );
	TextLayer_Text_AppendW( layer, unicode_text );
	free( unicode_text );
	return 0;
}

/* 在光标处添加文本，如有选中文本，将被删除 */
LCUI_API int
TextLayer_Text_AddW( LCUI_TextLayer *layer, wchar_t *unicode_text )
{
	TextLayer_Text_Process( layer, AT_CURSOR_POS, unicode_text );
	return 0;
}

LCUI_API int
TextLayer_Text_Add( LCUI_TextLayer *layer, char *utf8_text )
{
	wchar_t *unicode_text;
	LCUICharset_UTF8ToUnicode( utf8_text, &unicode_text );
	TextLayer_Text_AddW( layer, unicode_text );
	free( unicode_text );
	return 0;
}

LCUI_API int
TextLayer_Text_AddA( LCUI_TextLayer *layer, char *ascii_text )
{
	wchar_t *unicode_text;
	LCUICharset_GB2312ToUnicode( ascii_text, &unicode_text );
	TextLayer_Text_AddW( layer, unicode_text );
	free( unicode_text );
	return 0;
}

LCUI_API int
TextLayer_Text_Paste( LCUI_TextLayer *layer )
/* 将剪切板的内容粘贴至文本图层 */
{
	return 0;
}

/** 获取指定字符数据在源文本中的位置 */
static int TextLayer_GetCharPos(	LCUI_TextLayer *layer, 
					LCUI_CharData *p_cdata, 
					LCUI_BOOL left_search )
{
	int n, src_pos, total;
	LCUI_CharData *p_exist_cdata;
	
	if( !p_cdata ) {
		return -1;
	}
	src_pos = layer->current_src_pos;
	total = Queue_GetTotal( &layer->text_source_data );
	//printf( "source text len: %d\n", total );
	/* 确保起点位置有效 */
	if( src_pos >= total ) {
		src_pos = total-1;
	}
	if( src_pos < 0 ) {
		src_pos = 0;
	}
	/* 如果是要向左搜索 */
	if( left_search ) {
		goto left_search;
	} else {
		goto right_search;
	}
	/* 确定该字在源文本中的位置 */
left_search:
	//printf( "left, char_ptr: %p, char_code: %c\n", 
	//	char_ptr, char_ptr?char_ptr->char_code:'?' );
	for( n=src_pos; n>=0 ;--n ) {
		p_exist_cdata = (LCUI_CharData*)
		Queue_Get( &layer->text_source_data, n );
		//printf( "get, char_ptr: %p, char_code: %c\n", 
		//	tmp_ptr, tmp_ptr?tmp_ptr->char_code:'?' );
		if( p_exist_cdata == p_cdata ) {
			break;
		}
	}
	goto end_search;
	
right_search:
	//printf( "right, char_ptr: %p, char_code: %c\n", 
	//	char_ptr, char_ptr?char_ptr->char_code:'?' );
	for( n=src_pos; n<total; ++n ) {
		p_exist_cdata = (LCUI_CharData*)
		Queue_Get( &layer->text_source_data, n );
		//printf( "get, char_ptr: %p, char_code: %c\n", 
		//	tmp_ptr, tmp_ptr?tmp_ptr->char_code:'?' );
		if( p_exist_cdata == p_cdata ) {
			break;
		}
	}
	goto end_search;
	
end_search:
	/* 检测遍历结果 */
	if( left_search ) {
		if( n < 0 ) {
			goto right_search;
		}
		/* 未找到则返回-1 */
		if( n >= total ) {
			return -1;
		}
	} else {
		if( n < 0 ) {
			return -1;
		}
		if( n >= total ) {
			goto left_search;
		}
	}
	return n;
}


static LCUI_CharData *
TextLayer_GetCurChar( LCUI_TextLayer *layer )
/* 获取光标附近的字符数据的指针 */
{
	int total;
	LCUI_Pos pos;
	LCUI_CharData *char_ptr;
	Text_RowData *row_ptr;
	
	pos = TextLayer_Cursor_GetPos( layer );
	row_ptr = Queue_Get( &layer->rows_data, pos.y );
	if( !row_ptr ) {
		return NULL;
	}
	char_ptr = Queue_Get( &row_ptr->string, pos.x );
	total = Queue_GetTotal( &row_ptr->string );
	if( !char_ptr ) {
		/* 如果当前光标在这行行尾 */
		if( pos.x == total ) {
			char_ptr = row_ptr->last_char;
		} else {
			return NULL;
		}
	}
	return char_ptr;
}


static int
TextLayer_GetCharPosUnderCursor( LCUI_TextLayer *layer, int left_or_right )
/* 根据光标当前所在位置，获取对于源字符串中的位置 */
{
	LCUI_CharData *char_ptr;
	
	char_ptr = TextLayer_GetCurChar( layer );
	return TextLayer_GetCharPos( layer, char_ptr, left_or_right );
}

static int
TextLayer_Update_CurSrcPos( LCUI_TextLayer *layer, int left_or_right )
/* 更新当前光标所在的字符 对应于源文本中的位置 */
{
	int pos, max;
	pos = TextLayer_GetCharPosUnderCursor( layer, left_or_right );
	max = Queue_GetTotal( &layer->text_source_data );
	//printf("pos: %d, max: %d\n", pos, max );
	if( pos == -1 || pos > max ) {
		pos = max;
	}
	if( pos >= 0 ) {
		layer->current_src_pos = pos;
	}
	//printf( "layer->current_src_pos: %d\n", layer->current_src_pos );
	return layer->current_src_pos;
}

/* 
 * 功能：根据传入的二维坐标，设定光标在的文本图层中的位置
 * 说明：该位置会根据当前位置中的字体位图来调整，确保光标显示在字体位图边上，而不
 * 会遮挡字体位图；光标在文本图层中的位置改变后，在字符串中的位置也要做相应改变，
 * 因为文本的添加，删减，都需要以光标当前所在位置对应的字符为基础。
 * 返回值：文本图层中对应字体位图的坐标，单位为像素
 *  */
LCUI_API LCUI_Pos TextLayer_Cursor_SetPixelPos(	LCUI_TextLayer *layer, 
						LCUI_Pos pixel_pos )
{
	LCUI_Pos new_pos, pos;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;
	int i, n, rows, cols, adv_x;
	
	pos.x = pos.y = 0;
	rows = Queue_GetTotal( &layer->rows_data );
	/* 减去偏移坐标 */
	pixel_pos = Pos_Sub( pixel_pos, layer->offset_pos );
	for( new_pos.y=0,i=0; i<rows; ++i ) {
		row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, i );
		if( pixel_pos.y < row_ptr->max_size.h ) {
			break;
		}
		pixel_pos.y -= row_ptr->max_size.h;
		if(i < rows-1 ) {
			new_pos.y += row_ptr->max_size.h;
		}
	}
	pos.y = i;
	row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, i );
	if( !row_ptr ) {
		cols = 0;
	} else {
		cols = Queue_GetTotal( &row_ptr->string );
	}
	for( new_pos.x=0,n=0; n<cols; ++n ) {
		char_ptr = (LCUI_CharData*)Queue_Get( &row_ptr->string, n );
		if( !char_ptr || !char_ptr->bitmap || !char_ptr->display ) {
			continue;
		}
		/* 若启用了屏蔽符 */
		if( layer->password_char.char_code > 0 
		 && layer->password_char.bitmap ) {
			adv_x = layer->password_char.bitmap->advance.x;
		} else {
			adv_x = char_ptr->bitmap->advance.x;
		}
		/* 若坐标在当前字符的右边 */
		if( pixel_pos.x >= adv_x/2 ) {
			pixel_pos.x -= adv_x;
			new_pos.x += adv_x;
			if(n < cols-1 || (n == cols-1 && pixel_pos.x >= 0)) {
				continue;
			}
		}
		break;
	}
	pos.x = n;
	/* 判断光标位置变化，以快速确定当前字符在源文本中的位置并更新 */
	if( layer->current_des_pos.y > pos.y 
	 || (layer->current_des_pos.y == pos.y 
	 && layer->current_des_pos.x > pos.x)) {
		layer->current_des_pos = pos;
		 /* 优先向左边遍历 */
		TextLayer_Update_CurSrcPos( layer, 0 );
	} 
	else if( layer->current_des_pos.y == pos.y 
		&& layer->current_des_pos.x == pos.x );
	else { 
		layer->current_des_pos = pos;
		/* 优先向右边遍历 */
		TextLayer_Update_CurSrcPos( layer, 1 );
	}
	/* 加上偏移坐标 */
	new_pos = Pos_Add( new_pos, layer->offset_pos );
	return new_pos;
}

LCUI_API LCUI_Pos
TextLayer_Cursor_SetPos( LCUI_TextLayer *layer, LCUI_Pos pos )
/* 设定光标在文本框中的位置，并返回该光标的坐标，单位为像素 */
{
	LCUI_Pos pixel_pos;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;
	int rows, cols, total;
	
	pixel_pos.x = pixel_pos.y = 0;
	total = Queue_GetTotal( &layer->rows_data );
	if( pos.y >= total ) {
		pos.y = total-1;
	}
	if( pos.y < 0 ) {
		pos.y = 0;
	}
	/* 累加pos.y行之前几行的高度 */
	for( pixel_pos.y=0,rows=0; rows<pos.y; ++rows ) {
		row_ptr = Queue_Get( &layer->rows_data, rows );
		if( !row_ptr ) {
			continue;
		}
		pixel_pos.y += row_ptr->max_size.h;
	}
	/* 获取当前行的指针 */
	row_ptr = Queue_Get( &layer->rows_data, rows );
	if( !row_ptr ) {
		return pixel_pos;
	}
	/* 获取当前行的文字数 */
	total = Queue_GetTotal( &row_ptr->string ); 
	if( pos.x > total ) {
		pos.x = total;
	}
	if( pos.x < 0 ) {
		pos.x = 0;
	}
	/* 累计宽度 */
	for( pixel_pos.x=0,cols=0; cols<pos.x; ++cols ) {
		char_ptr = Queue_Get( &row_ptr->string, cols );
		if( !char_ptr || !char_ptr->bitmap || !char_ptr->display ) {
			continue;
		}
		pixel_pos.x += char_ptr->bitmap->advance.x;
		//printf("TextLayer_Cursor_SetPos(): pixel pos x: %d, total: %d, cols: %d, pos.x: %d\n", 
		//pixel_pos.x, total, cols, pos.x);
	}
	//printf("layer->current_des_pos: %d,%d,  pos: %d,%d\n",
	//layer->current_des_pos.x, layer->current_des_pos.y, pos.x, pos.y );
	if( layer->current_des_pos.y > pos.y 
	 || (layer->current_des_pos.y == pos.y 
	 && layer->current_des_pos.x > pos.x)) {
		layer->current_des_pos = pos;
		TextLayer_Update_CurSrcPos( layer, 0 );
	} 
	else if( layer->current_des_pos.y == pos.y 
		&& layer->current_des_pos.x == pos.x );
	else {
		layer->current_des_pos = pos;
		TextLayer_Update_CurSrcPos( layer, 1 );
	}
	layer->current_des_pos = pos;
	/* 加上偏移坐标 */
	pixel_pos = Pos_Add( pixel_pos, layer->offset_pos );
	/* 微调位置 */
	pixel_pos.y += 2;
	return pixel_pos;
}

LCUI_API int
TextLayer_Text_Delete( LCUI_TextLayer *layer, int n )
/* 删除光标右边处n个字符 */
{
	return 0;
}

static int
TextLayer_Text_DeleteChar( 
		LCUI_TextLayer *layer, 
		LCUI_CharData *char_ptr, 
		int left_or_right )
/* 
 * 功能：将字符数据从源文本中移除
 * 参数：
 * layer         操作对象
 * char_ptr      指向所要删除的字符数据的指针
 * left_or_right 指定在源文本中遍历的方向，值为0时是向左遍历，其它值则像右遍历
 *  */
{
	int n;
	n = TextLayer_GetCharPos( layer, char_ptr, left_or_right );
	/* 将该字从源文本中移除 */
	Queue_Delete( &layer->text_source_data, n );
	return 0;
}


static int 
_TextLayer_Text_Delete ( LCUI_TextLayer *layer, LCUI_Pos start_pos, int len )
/* 以start_pos为起点，删除n个文字 */
{
	LCUI_BOOL refresh = TRUE;
	LCUI_CharData *char_ptr;
	LCUI_Pos tmp_pos, pixel_pos;
	int left_or_right, rows, cols;
	Text_RowData *row_ptr, *tmp_row;
	
	if( start_pos.x < 0 ) {
		len += start_pos.x;
		start_pos.x = 0;
	}
	if( start_pos.y < 0 ) {
		start_pos.y = 0;
	}
	if( len <= 0 ) {
		return -1;
	}
	/* 确定起点位置的XY轴坐标 */
	pixel_pos = TextLayer_Char_GetPixelPos( layer, start_pos );
	
	rows = Queue_GetTotal( &layer->rows_data );
	row_ptr = (Text_RowData*)Queue_Get( &layer->rows_data, start_pos.y );
	if( !row_ptr ) {
		return -1;
	}
	cols = Queue_GetTotal( &row_ptr->string );
	
	/* 根据光标所在位置，确定遍历方向 */
	if( layer->current_des_pos.y > start_pos.y 
	 || (layer->current_des_pos.y == start_pos.y 
	 && layer->current_des_pos.x > start_pos.x)) {
		left_or_right = 0;
	}
	else {
		left_or_right = 1;
	}
	
	/* 如果需删除的字符只在当前行 */
	if( start_pos.x + len <= cols ) {
		/* 标记后面的文字位图需要刷新 */
		TextLayer_RefreshCharBehind( layer, start_pos );
	}
	for( ; start_pos.x<=cols && len>0; --len ) {
		/* 如果到了行尾 */
		if( start_pos.x == cols ) {
			/* 如果当前行是最后一行 */
			if( start_pos.y >= rows-1 ) {
				break;
			}
			if( refresh ) {
				tmp_pos.x = 0;
				tmp_pos.y=start_pos.y+1;
				/* 刷新该行后面所有行的字符 */
				for( ; tmp_pos.y<rows; ++tmp_pos.y ) {
					TextLayer_RefreshCharBehind( layer, tmp_pos );
				}
				refresh = FALSE;
			}
			
			/* 将当前行行尾的换行符'\n'从源文本中移除 */
			TextLayer_Text_DeleteChar( layer, row_ptr->last_char, left_or_right );
			/* 获取指向下一行文本的指针 */
			tmp_row = (Text_RowData*)Queue_Get( &layer->rows_data, start_pos.y+1 );
			/* 将下一行的文本拼接至当前行行尾 */
			Queue_Cat( &row_ptr->string, &tmp_row->string );
			/* 将下一行的行尾字符数据转移至当前行 */
			row_ptr->last_char = tmp_row->last_char;
			/* 销毁下一行的文本 */
			Queue_Destroy( &tmp_row->string ); 
			Queue_Delete( &layer->rows_data, start_pos.y+1 );
			/* 更新当前行的总字符数 */
			cols = Queue_GetTotal( &row_ptr->string );
			/* 更新总行数 */
			rows = Queue_GetTotal( &layer->rows_data );
			/* 更新当前行的尺寸 */
			TextLayer_Update_RowSize( layer, start_pos.y );
			continue;
		}
		char_ptr = (LCUI_CharData*)Queue_Get( &row_ptr->string, start_pos.x );
		if( !char_ptr ) {
			continue;
		}
		TextLayer_Clear( layer, pixel_pos, row_ptr->max_size.h, char_ptr );
		pixel_pos.x += char_ptr->bitmap->advance.x;
		/* 将该字从源文本中移除 */
		TextLayer_Text_DeleteChar( layer, char_ptr, left_or_right );
		/* 该字在这行的字体位图也需要删除 */
		cols = Queue_GetTotal( &row_ptr->string );
		Queue_DeletePointer( &row_ptr->string, start_pos.x );
		cols = Queue_GetTotal( &row_ptr->string );
		char_ptr = (LCUI_CharData*)Queue_Get( &row_ptr->string, start_pos.x );
		cols = Queue_GetTotal( &row_ptr->string );
	}
	/* 更新当前行的尺寸 */
	TextLayer_Update_RowSize( layer, start_pos.y );
	return 0;
}

LCUI_API int
TextLayer_Text_Backspace( LCUI_TextLayer *layer, int n )
/* 删除光标左边处n个字符 */
{
	int i, row_len;
	LCUI_Pos char_pos;
	Text_RowData *row_ptr;
	
	if( layer->read_only ) {
		return -1;
	}
	
	if( n <= 0 ) {
		return -2;
	}
	/* 计算当前光标所在字的位置 */
	char_pos = TextLayer_Cursor_GetPos( layer );
	DEBUG_MSG2( "before: %d,%d\n", char_pos.x, char_pos.y );
	for( i=n; char_pos.y>=0; --char_pos.y ) {
		row_ptr = Queue_Get( &layer->rows_data, char_pos.y );
		if(row_ptr == NULL) {
			continue;
		}
		row_len = Queue_GetTotal( &row_ptr->string );
		
		if( char_pos.x == -1 ) {
			char_pos.x = row_len;
		}
		for( ; char_pos.x>=0 && i>0; --char_pos.x,--i );
		
		if( i<=0 && char_pos.x >= 0 ) {
			break;
		}
	}
	DEBUG_MSG2( "after: %d,%d\n", char_pos.x, char_pos.y );
	if( i>0 ) {
		n -= i;
	}
	DEBUG_MSG2("start_pos: %d,%d, len: %d\n", char_pos.x, char_pos.y, n);
	/* 开始删除文字 */
	_TextLayer_Text_Delete( layer, char_pos, n );
	/* 删除完后，需要将光标向左移动一个位置 */
	TextLayer_Cursor_SetPos( layer, char_pos );
	return 0;
}

/** 获取显示出来的文字相对于文本图层的坐标，单位为像素 */
LCUI_API LCUI_Pos TextLayer_Char_GetPixelPos(	LCUI_TextLayer *layer,
						LCUI_Pos char_pos )
{
	LCUI_Pos pixel_pos;
	Text_RowData *row_ptr;
	LCUI_CharData *char_ptr;
	int rows, cols, total;
	
	pixel_pos.x = pixel_pos.y = 0;
	char_pos = TextLayer_Cursor_GetPos( layer );
	total = Queue_GetTotal( &layer->rows_data );
	if( char_pos.y >= total ) {
		char_pos.y = total-1;
	}
	if( char_pos.y < 0 ) {
		char_pos.y = 0;
	}
	/* 累加pos.y行之前几行的高度 */
	for( pixel_pos.y=0,rows=0; rows<char_pos.y; ++rows ) {
		row_ptr = Queue_Get( &layer->rows_data, rows );
		if( !row_ptr ) {
			continue;
		}
		pixel_pos.y += row_ptr->max_size.h;
	}
	/* 获取当前行的指针 */
	row_ptr = Queue_Get( &layer->rows_data, rows );
	if( !row_ptr ) {
		pixel_pos.y += 2;
		return pixel_pos;
	}
	/* 获取当前行的文字数 */
	total = Queue_GetTotal( &row_ptr->string ); 
	if( char_pos.x > total ) {
		char_pos.x = total;
	}
	if( char_pos.x < 0 ) {
		char_pos.x = 0;
	}
	/* 累计宽度 */
	for( pixel_pos.x=0,cols=0; cols<char_pos.x; ++cols ) {
		char_ptr = Queue_Get( &row_ptr->string, cols );
		if( !char_ptr || !char_ptr->bitmap || !char_ptr->display ) {
			continue;
		}
		/* 如果设定了屏蔽字符 */
		if( layer->password_char.char_code > 0 ) {
			pixel_pos.x += layer->password_char.bitmap->advance.x;
		} else {
			pixel_pos.x += char_ptr->bitmap->advance.x;
		}
	}
	/* 微调位置 */
	pixel_pos.y += 2;
	return pixel_pos;
}

LCUI_API LCUI_Pos
TextLayer_Cursor_GetPos( LCUI_TextLayer *layer )
/* 获取光标在文本框中的位置，也就是光标在哪一行的哪个字后面 */
{
	return layer->current_des_pos;
}

LCUI_API LCUI_Pos
TextLayer_Cursor_GetFixedPixelPos( LCUI_TextLayer *layer )
/* 获取文本图层的光标位置，单位为像素 */
{
	LCUI_Pos pos;
	pos = TextLayer_Cursor_GetPos( layer );
	pos = TextLayer_Char_GetPixelPos( layer, pos );
	return pos;
}

LCUI_API LCUI_Pos
TextLayer_Cursor_GetPixelPos( LCUI_TextLayer *layer )
/* 获取文本图层的光标相对于容器位置，单位为像素 */
{
	LCUI_Pos pos;
	pos = TextLayer_Cursor_GetFixedPixelPos( layer );
	/* 加上偏移坐标 */
	pos = Pos_Add( pos, layer->offset_pos );
	return pos;
}

LCUI_API int
TextLayer_GetRowLen( LCUI_TextLayer *layer, int row )
/* 获取指定行显式文字数 */
{
	int total;
	Text_RowData *row_ptr;
	
	total = Queue_GetTotal( &layer->rows_data );
	if( row > total ) {
		row = total;
	}
	/* 获取当前行的指针 */
	row_ptr = Queue_Get( &layer->rows_data, row );
	if( !row_ptr ) {
		return 0;
	}
	return Queue_GetTotal( &row_ptr->string ); 
}

LCUI_API int
TextLayer_CurRow_GetMaxHeight( LCUI_TextLayer *layer )
/* 获取当前行的最大高度 */
{
	Text_RowData *row_ptr;
	row_ptr = TextLayer_GetCurRowData( layer );
	if( !row_ptr ) {
		return layer->default_data.pixel_size + 2;
	}
	return row_ptr->max_size.h;
}

LCUI_API int
TextLayer_GetRows( LCUI_TextLayer *layer )
/* 获取文本行数 */
{
	return Queue_GetTotal( &layer->rows_data );
}

LCUI_API int
TextLayer_GetSelectedText( LCUI_TextLayer *layer, char *out_text )
/* 获取文本图层内被选中的文本 */
{ 
	/* 如果选择了文本 */
	if( layer->have_select ) {
		/* 获取选中的文本内容 */
		//......
		return 0;
	}
	return -1;
}

LCUI_API int
TextLayer_CopySelectedText( LCUI_TextLayer *layer )
/* 复制文本图层内被选中的文本 */
{
	/* 如果选择了文本 */
	if( layer->have_select ) {
		/* 将选中的文本内容添加至剪切版 */
		//......
		return 0;
	}
	return -1;
}

LCUI_API int
TextLayer_CutSelectedText( LCUI_TextLayer *layer )
/* 剪切文本图层内被选中的文本 */
{
	/* 如果选择了文本 */
	if( layer->have_select ) {
		/* 将选中的文本内容添加至剪切版，并删除被选中的文本 */
		//......
		return 0;
	}
	return -1;
}

LCUI_API void
TextLayer_UsingStyleTags( LCUI_TextLayer *layer, LCUI_BOOL flag )
/* 指定文本图层是否处理样式标签 */
{
	layer->using_style_tags = flag;
}

/** 指定文本图层是否启用多行文本显示 */
LCUI_API void TextLayer_SetMultiline( LCUI_TextLayer *layer, LCUI_BOOL flag )
{
	layer->enable_multiline = flag;
}

/** 设置文本图层中的文本是否自动换行 */
LCUI_API void TextLayer_SetAutoWrap( LCUI_TextLayer *layer, LCUI_BOOL flag )
{
	if( layer->auto_wrap == flag ) {
		return;
	}
	layer->auto_wrap = flag;
	/* 更新文本图层中的文本排版 */
	TextLayer_TextTypeset( layer, 0 );
}
