#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include LC_DRAW_BORDER_H
#include LC_DRAW_LINE_H
#include <math.h>

LCUI_API void Border_Init( LCUI_Border *border )
/* 初始化边框数据 */
{
	border->top_width = 0;
	border->bottom_width = 0;
	border->left_width = 0;
	border->right_width = 0;
	border->top_style = BORDER_STYLE_NONE;
	border->bottom_style = BORDER_STYLE_NONE;
	border->left_style = BORDER_STYLE_NONE;
	border->right_style = BORDER_STYLE_NONE;
	border->top_color = RGB(0,0,0);
	border->bottom_color = RGB(0,0,0);
	border->left_color = RGB(0,0,0);
	border->right_color = RGB(0,0,0);
	border->top_left_radius = 0;
	border->top_right_radius = 0;
	border->bottom_left_radius = 0;
	border->bottom_right_radius = 0;
}

LCUI_API LCUI_Border Border( unsigned int width_px, BORDER_STYLE style, LCUI_RGB color )
/* 简单的设置边框样式，并获取该样式数据 */
{
	LCUI_Border border;
	border.top_width = width_px;
	border.bottom_width = width_px;
	border.left_width = width_px;
	border.right_width = width_px;
	border.top_style = style;
	border.bottom_style = style;
	border.left_style = style;
	border.right_style = style;
	border.top_color = color;
	border.bottom_color = color;
	border.left_color = color;
	border.right_color = color;
	border.top_left_radius = 0;
	border.top_right_radius = 0;
	border.bottom_left_radius = 0;
	border.bottom_right_radius = 0;
	return border; 
}

LCUI_API void Border_Radius( LCUI_Border *border, unsigned int radius )
/* 设置边框的圆角半径 */
{
	border->top_left_radius = radius;
	border->top_right_radius = radius;
	border->bottom_left_radius = radius;
	border->bottom_right_radius = radius;
}

#ifdef LCUI_BUILD_IN_LINUX
extern inline void 
fill_pixel( uchar_t **buff, int pos, LCUI_RGB color ) 
__attribute__((always_inline));

extern inline void 
mix_pixel( uchar_t **buff, int pos, LCUI_RGB color, uchar_t alpha ) 
__attribute__((always_inline));
#endif

#if defined(WIN32) && !defined(__cplusplus)
#define inline __inline
#endif

extern inline void 
fill_pixel( uchar_t **buff, int pos, LCUI_RGB color ) 
{
	buff[0][pos] = color.red;
	buff[1][pos] = color.green;
	buff[2][pos] = color.blue;
}

extern inline void
mix_pixel( uchar_t **buff, int pos, LCUI_RGB color, uchar_t alpha ) 
{
	ALPHA_BLEND( color.red, buff[0][pos], alpha );
	ALPHA_BLEND( color.blue, buff[1][pos], alpha );
	ALPHA_BLEND( color.green, buff[2][pos], alpha );
}

static int
Graph_Draw_RoundBorder_LeftTop( 
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制左上角的圆角，从左边框的上端到上边框的左端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	/* 预先计算xy轴坐标的有效范围 */
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	/* 预先计算圆心的线性坐标 */
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;

	/* 根据y轴计算各点的x轴坐标并填充点 */
	for( y=0; y<=radius; ++y, center_pos -= des->w ) {
		if( radius-y >= max_y || radius-y < min_y ) {
			continue;
		}
		/* 计算出x轴整数坐标 */
		x = (int)sqrt( pow(radius, 2) - y*y );
		
		if( line_width > 0 && radius-x >= min_x 
		 && radius-x <= max_x ) {
			pos = center_pos - x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			/* 计算起点坐标 */
			pos = center_pos - center.x;
			if( radius-x > max_x ) {
				n = max_x - min_x;
			} else {
				n = radius-x-min_x;
			}
			/* 加上圆与背景图的左边距 */
			n += (center.x-radius);
			for(i=0; i<n; ++i) {
				des->rgba[3][pos++]=0;
			}
		}
		/* 计算需要向右填充的像素点的个数n */
		n = radius-x+line_width;
		n = n>radius ? x:line_width;
		/* 如果该点x轴坐标小于最小x轴坐标 */
		if( radius-x < min_x ) {
			/* 重新确定起点坐标pos和填充的像素点的个数n */
			pos = center_pos - radius+min_x-1;
			n -= (min_x-radius+x);
		} else {
			pos = center_pos - x;
		}
		/* 从下一个像素点开始 */
		++pos;
		/* 如果填充的像素点超出了最大x轴范围 */
		if( radius-x + n > max_x ) {
			/* 重新确定需要填充的像素点的个数n */
			n = max_x - radius + x;
		}
		/* 开始填充当前点右边的n-1个像素点 */
		for(i=0; i<n-1; ++i,++pos) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}


static int
Graph_Draw_RoundBorder_TopLeft( 
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制左上角的圆角，从上边框的左端到左边框的上端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	double tmp_pos;
	uchar_t alpha[2];
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	/* 预先计算xy轴坐标的有效范围 */
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;

	for( x=0; x<=radius; ++x ) {
		if( radius-x >= max_x || radius-x < min_x ) {
			continue;
		}
		tmp_pos = sqrt( pow(radius, 2) - x*x );
		y = (int)tmp_pos;
		alpha[1] =(uchar_t)( 255/(tmp_pos - y));
		alpha[0] = 255-alpha[1];
		
		if( line_width > 0 && radius-y >= min_y 
		 && radius-y <= max_y ) {
			pos = center_pos - y * des->w - x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}

		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			/* 计算起点坐标 */
			if( radius-y < min_y ) {
				pos = center_pos;
				pos -= ((radius+min_y-1)*des->w);
			} else {
				pos = center_pos - y * des->w - x;
			}
			if( radius-y > max_y ) {
				n = max_y - min_y;
			} else {
				n = radius-y-min_y;
			}
			/* 加上圆与背景图的上边距 */
			n += (center.y-radius);
			pos-=des->w;
			for(i=0; i<n; ++i,pos-=des->w) {
				des->rgba[3][pos]=0;
			}
		}
		
		/* 计算需要向下填充的像素点的个数n */
		n = radius-y+line_width;
		n = n>radius ? y:line_width;
		if( radius-y < min_y ) {
			/* 重新确定起点坐标pos和填充的像素点的个数n */
			pos = center_pos - (radius+min_y-1)*des->w;
			n -= (min_y-radius+y);
		} else {
			pos = center_pos - y * des->w - x;
		}
		/* 从下一行像素点开始 */
		pos += des->w;
		/* 如果填充的像素点超出了最大y轴范围 */
		if( radius-y + n > max_y ) {
			/* 重新确定需要填充的像素点的个数n */
			n = max_y - radius + y;
		}
		/* 开始填充当前点下边的n-2个像素点 */
		for(i=0; i<n-1; ++i,pos+=des->w) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}

static int
Graph_Draw_RoundBorder_RightTop( 
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制右上角的圆角，从右边框的上端到上边框的右端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;
	
	for( y=0; y<=radius; ++y, center_pos -= des->w ) {
		if( radius-y >= max_y || radius-y < min_y ) {
			continue;
		}
		x = (int)sqrt( pow(radius, 2) - y*y );
		
		if( line_width > 0 && radius+x >= min_x 
		 && radius+x <= max_x ) {
			pos = center_pos + x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			n = center_pos + max_x - radius;
			if( radius+x < min_x ) {
				pos = center_pos + min_x - radius;
			} else {
				pos = center_pos + x;
			}
			for( ++pos; pos<=n; ++pos ) {
				des->rgba[3][pos] = 0;
			}
		}
		
		/* 计算需要向左填充的像素点的个数n */
		n = radius+x-line_width;
		n = n<min_x ? x+radius-min_x:line_width;
		
		if( radius+x > max_x ) {
			pos = center_pos - radius+max_x-1;
			n -= (radius+x-max_x);
		} else {
			pos = center_pos + x;
		}
		for(i=0; i<n; ++i,--pos) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}


static int
Graph_Draw_RoundBorder_TopRight( 
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制右上角的圆角，从上边框的右端到右边框的上端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;

	for( x=0; x<=radius; ++x ) {
		if( radius+x >= max_x || radius+x < min_x ) {
			continue;
		}
		
		y = (int)sqrt( pow(radius, 2) - x*x );
		
		if( line_width > 0 && radius-y >= min_y 
		 && radius-y <= max_y ) {
			pos = center_pos - y * des->w + x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			if( radius-y < min_y ) {
				pos = center_pos;
				pos -= ((radius+min_y-1)*des->w);
			} else {
				pos = center_pos - y * des->w + x;
			}
			if( radius-y > max_y ) {
				n = max_y - min_y;
			} else {
				n = radius-y-min_y;
			}
			n += (center.y-radius);
			pos-=des->w;
			for(i=0; i<n; ++i,pos-=des->w) {
				des->rgba[3][pos]=0;
			}
		}
		
		n = radius-y+line_width;
		n = n>radius ? y:line_width;
		if( radius-y < min_y ) {
			pos = center_pos - (radius+min_y-1)*des->w;
			n -= (min_y-radius+y);
		} else {
			pos = center_pos - y * des->w + x;
		}
		pos += des->w;
		if( radius-y + n > max_y ) {
			n = max_y - radius + y;
		}
		for(i=0; i<n-1; ++i,pos+=des->w) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}

static int
Graph_Draw_RoundBorder_LeftBottom(
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制左下角的圆角，从左边框的下端到下边框的左端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;

	if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
		
	}
	for( y=0; y<=radius; ++y, center_pos += des->w ) {
		if( radius+y >= max_y || radius+y < min_y ) {
			continue;
		}
		x = (int)sqrt( pow(radius, 2) - y*y );
		
		if( line_width > 0 && radius-x >= min_x 
		 && radius-x <= max_x ) {
			pos = center_pos - x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			pos = center_pos - center.x;
			if( radius-x > max_x ) {
				n = max_x - min_x;
			} else {
				n = radius-x-min_x;
			}
			n += (center.x-radius);
			for(i=0; i<n; ++i) {
				des->rgba[3][pos++]=0;
			}
		}
		n = radius-x+line_width;
		n = n>radius ? x:line_width;
		if( radius-x < min_x ) {
			pos = center_pos - radius+min_x-1;
			n -= (min_x-radius+x);
		} else {
			pos = center_pos - x;
		}
		++pos;
		if( radius-x + n > max_x ) {
			n = max_x - radius + x;
		}
		for(i=0; i<n-1; ++i,++pos) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}

static int
Graph_Draw_RoundBorder_BottomLeft(
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制左下角的圆角，从下边框的左端到左边框的下端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int tmp_pos, pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;

	for( x=0; x<=radius; ++x ) {
		if( radius-x >= max_x || radius-x < min_x ) {
			continue;
		}
		
		y = (int)sqrt( pow(radius, 2) - x*x );
		
		if( radius+y > max_y ) {
			pos = center_pos;
			pos += (max_y-radius)*des->w;
		} else {
			pos = center_pos + y * des->w - x;
		}
		
		if( line_width > 0 && radius+y >= min_y 
		 && radius+y <= max_y ) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			tmp_pos = pos;
			if( radius+y < min_y ) {
				n = max_y - min_y;
			} else {
				n = max_y-radius-y;
			}
			/* 加上圆与背景图的下边距 */
			n += (real_rect.height-center.y-radius);
			tmp_pos+=des->w;
			for(i=0; i<n; ++i,tmp_pos+=des->w) {
				des->rgba[3][tmp_pos]=0;
			}
		}
		/* 计算需要向上填充的像素点的个数n */
		n = radius+y-line_width;
		/* 判断是否超过圆的x轴对称线 */
		n = n<radius ? y:line_width;
		if( radius+y > max_y ) {
			n -= (radius+y-max_y);
		}
		/* 从上一行像素点开始 */
		pos -= des->w;
		if( radius+y - n < min_y ) {
			n = min_y - radius - y;
		}
		/* 开始填充当前点下边的n-1个像素点 */
		for(i=0; i<n-1; ++i,pos-=des->w) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}

static int
Graph_Draw_RoundBorder_RightBottom( 
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制右下角的圆角，从右边框的下端到下边框的右端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;
	
	for( y=0; y<=radius; ++y, center_pos += des->w ) {
		if( radius+y >= max_y || radius+y < min_y ) {
			continue;
		}
		x = (int)sqrt( pow(radius, 2) - y*y );
		
		if( line_width > 0 && radius+x >= min_x 
		 && radius+x < max_x ) {
			pos = center_pos + x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			n = center_pos + max_x - radius;
			if( radius+x < min_x ) {
				pos = center_pos + min_x - radius;
			} else {
				pos = center_pos + x;
			}
			for( ++pos; pos<=n; ++pos ) {
				des->rgba[3][pos] = 0;
			}
		}
		
		n = radius+x-line_width;
		n = n<min_x ? x+radius-min_x:line_width;
		
		if( radius+x > max_x ) {
			pos = center_pos - radius+max_x-1;
			n -= (radius+x-max_x);
		} else {
			pos = center_pos + x;
		}
		for(i=0; i<n; ++i,--pos) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}

static int
Graph_Draw_RoundBorder_BottomRight(
	LCUI_Graph *des,	LCUI_Pos center,
	int radius,		int line_width,
	LCUI_RGB line_color,	LCUI_BOOL hide_outarea )
/* 绘制右下角的圆角，从下边框的右端到右边框的下端绘制一条圆角线 */
{
	LCUI_Rect real_rect;
	int pos, center_pos, y, x, i, n;
	int max_x, max_y, min_x, min_y;
	
	if( line_width <= 0 && !hide_outarea ) {
		return 1;
	}
	
	real_rect = Graph_GetValidRect( des );
	des = Graph_GetQuote( des );
	if( !Graph_IsValid( des ) ) {
		return -1;
	}
	
	max_x = radius*2;
	if( center.x+radius > real_rect.x + real_rect.width ) {
		max_x -= (center.x+radius-real_rect.x-real_rect.width);
	}
	if( max_x < 0 ) {
		max_x = 0;
	}
	min_x = center.x>radius? 0:radius-center.x;
	if( min_x < 0 ) {
		min_x = 0;
	}
	max_y = radius*2;
	if( center.y+radius > real_rect.y + real_rect.height ) {
		max_y -= (center.y+radius-real_rect.y-real_rect.height);
	}
	min_y = center.y>radius? 0:radius-center.y;
	if( min_y < 0 ) {
		min_y = 0;
	}
	
	center_pos = (real_rect.y + center.y) * des->w;
	center_pos = center_pos + center.x + real_rect.x;

	for( x=0; x<=radius; ++x ) {
		if( radius+x >= max_x || radius+x < min_x ) {
			continue;
		}
		
		y = (int)sqrt( pow(radius, 2) - x*x );
		
		if( line_width > 0 && radius+y >= min_y 
		 && radius+y < max_y ) {
			pos = center_pos + y * des->w + x;
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
		if( hide_outarea && des->color_type == COLOR_TYPE_RGBA ) {
			if( radius+y > max_y ) {
				pos = center_pos;
				pos += (max_y-radius)*des->w;
			} else {
				pos = center_pos + y * des->w + x;
			}
			if( radius+y < min_y ) {
				n = max_y - min_y;
			} else {
				n = max_y-radius-y;
			}
			n += (real_rect.height-center.y-radius);
			pos+=des->w;
			for(i=0; i<n; ++i,pos+=des->w) {
				des->rgba[3][pos]=0;
			}
		}
		
		n = radius+y-line_width;
		n = n<radius ? y:line_width;
		if( radius+y > max_y ) {
			pos = center_pos + (max_y-radius)*des->w;
			n -= (radius+y-max_y);
		} else {
			pos = center_pos + y * des->w + x;
		}
		if( radius+y - n < min_y ) {
			n = min_y - radius - y;
		}
		pos -= des->w;
		for(i=0; i<n-1; ++i,pos-=des->w) {
			fill_pixel( des->rgba, pos, line_color );
			if( des->color_type == COLOR_TYPE_RGBA ) {
				des->rgba[3][pos] = 255;
			}
		}
	}
	return 0;
}

LCUI_API int Graph_DrawBorder( LCUI_Graph *des, LCUI_Border border )
/* 简单的为图形边缘绘制边框 */
{
	int  radius;
	LCUI_Rect rect;
	LCUI_Pos start, end;
	LCUI_Graph des_area;
	
	if( !Graph_IsValid(des) ) {
		return -1;
	}
	
	/* 绘制左上角的圆角，先引用左上角区域，再将圆绘制到这个区域里 */
	radius = border.top_left_radius;
	rect = Rect( 0, 0, radius, radius );
	Graph_Quote( &des_area, des, rect );
	Graph_Draw_RoundBorder_LeftTop( 
		&des_area		, Pos( radius, radius ), 
		radius			, border.left_width, 
		border.left_color	, TRUE
	);
	Graph_Draw_RoundBorder_TopLeft( 
		&des_area		, Pos( radius, radius ), 
		radius			, border.top_width, 
		border.top_color	, TRUE 
	);
	
	/* 右上角 */
	radius = border.top_right_radius;
	rect = Rect( des->w-radius-1, 0, radius, radius );
	Graph_Quote( &des_area, des, rect );
	Graph_Draw_RoundBorder_RightTop( 
		&des_area		, Pos( 0, radius ), 
		radius			, border.right_width, 
		border.right_color	, TRUE 
	);
	Graph_Draw_RoundBorder_TopRight( 
		&des_area		, Pos( 0, radius ), 
		radius			, border.top_width, 
		border.top_color	, TRUE 
	);
	
	/* 左下角 */
	radius = border.bottom_left_radius;
	rect = Rect( 0, des->h-radius-1, radius, radius );
	Graph_Quote( &des_area, des, rect );
	Graph_Draw_RoundBorder_LeftBottom( 
		&des_area		, Pos( radius, 0 ), 
		radius			, border.left_width, 
		border.left_color	, TRUE 
	);
	Graph_Draw_RoundBorder_BottomLeft( 
		&des_area		, Pos( radius, 0 ), 
		radius			, border.bottom_width, 
		border.bottom_color	, TRUE 
	);
	
	/* 右下角 */
	radius = border.bottom_left_radius;
	rect = Rect(	des->w-radius-1, 
			des->h-radius-1, radius, radius );
	Graph_Quote( &des_area, des, rect );
	Graph_Draw_RoundBorder_RightBottom( 
		&des_area		, Pos( 0, 0 ), 
		radius			, border.right_width, 
		border.right_color	, TRUE 
	);
	Graph_Draw_RoundBorder_BottomRight( 
		&des_area		, Pos( 0, 0 ), 
		radius			, border.bottom_width, 
		border.bottom_color	, TRUE 
	);
	
	start.x = border.top_left_radius;
	start.y = 0;
	end.x = des->w - border.top_right_radius;
	/* 绘制上边框 */
	Graph_DrawHorizLine( des, border.top_color, border.top_width, start, end.x );
	/* 绘制下边的线 */
	start.y = des->h - border.bottom_width;
	end.x = des->w - border.bottom_right_radius;
	Graph_DrawHorizLine( des, border.top_color, border.bottom_width, start, end.x );
	/* 绘制左边的线 */
	start.x = start.y = 0;
	end.y = des->h - border.bottom_left_radius;
	Graph_DrawVertiLine( des, border.left_color, border.left_width, start, end.y );
	/* 绘制右边的线 */
	start.x = des->w - border.right_width;
	start.y = border.top_right_radius;
	end.y = des->h - border.bottom_right_radius;
	Graph_DrawVertiLine( des, border.right_color, border.right_width, start, end.y );
	/* 边框线绘制完成 */
	return 0;
}
