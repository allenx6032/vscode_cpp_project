#include "brush.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Private */
static void swap(int* n, int* m);
static void ellipse_put4pixel(void* userdata, int x, int y, int vx, int vy);


/*Basic Config*****************************************************************/
HB_State HB_GlobalState = {NULL, NULL, 0, 0, 0};

int HB_SetColor(void* userdata, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	HB_GlobalState.red = red;
	HB_GlobalState.green = green;
	HB_GlobalState.alpha = alpha;
	HB_GlobalState.setColor(userdata, red, green, blue, alpha);
	
	return 0;
}
int HB_PutPixel(void* userdata, int x, int y)
{
	HB_GlobalState.putPixel(userdata, x, y);
	
	return 0;
}


int HB_SetPixelFunction(HB_PixelFunction pixel)
{
	HB_GlobalState.putPixel = pixel;
}


int HB_SetColorFunction(HB_ColorFunction color)
{
	HB_GlobalState.setColor = color;
}
/******************************************************************************/

/* Bresenham's line algorithm 
 * Refer to https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
int HB_Line(void* userdata, int x1, int y1, int x2, int y2)
{
	int steep = abs(y2 - y1) > abs(x2 - x1);
	if(steep)
	{
		swap(&x1, &y1);
		swap(&x2, &y2);
	}
	if(x1 > x2)
	{
		swap(&x1, &x2);
		swap(&y1, &y2);
	}
	
	int delta_x = x2 - x1;
	int delta_y = abs(y2 - y1);
	int error = delta_x >> 1;
	int y_step = y1 < y2 ? 1 : -1;
	int y = y1;
	for(int x = x1; x <= x2; x++)
	{
		if(steep)
		{
			HB_PutPixel(userdata, y, x);
		}
		else
		{
			HB_PutPixel(userdata, x, y);
		}
		error -= delta_y;
		if(error < 0)
		{
			y += y_step;
			error += delta_x;
		}
	}
	
	return 0;
}


/* Midpoint circle algorithm 
 * Refer to https://en.wikipedia.org/wiki/Midpoint_circle_algorithm 
 */
int HB_Circle(void* userdata, int x, int y, int r)
{
	int vx = r - 1;
	int vy = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (r << 1);
	
	while(vx >= vy)
	{
		HB_PutPixel(userdata, x + vx, y + vy);
		HB_PutPixel(userdata, x + vy, y + vx);
		HB_PutPixel(userdata, x - vy, y + vx);
		HB_PutPixel(userdata, x - vx, y + vy);
		HB_PutPixel(userdata, x - vx, y - vy);
		HB_PutPixel(userdata, x - vy, y - vx);
		HB_PutPixel(userdata, x + vy, y - vx);
		HB_PutPixel(userdata, x + vx, y - vy);
		if(err <= 0)
		{
			vy++;
			err += dy;
			dy += 2;
		}
		else
		{
			vx--;
			dx += 2;
			err += dx - (r << 1);
		}
	}
	
	return 0;
}

/* Midpoint circle algorithm 
 * Refer to https://en.wikipedia.org/wiki/Midpoint_circle_algorithm 
 */
int HB_FillCircle(void* userdata, int x, int y, int r)
{
	int vx = r - 1;
	int vy = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (r << 1);
	
	while(vx >= vy)
	{

		HB_Line(userdata, x, y, x + vx, y + vy);
		HB_Line(userdata, x, y, x + vy, y + vx);
		HB_Line(userdata, x, y, x - vy, y + vx);
		HB_Line(userdata, x, y, x - vx, y + vy);
		HB_Line(userdata, x, y, x - vx, y - vy);
		HB_Line(userdata, x, y, x - vy, y - vx);
		HB_Line(userdata, x, y, x + vy, y - vx);
		HB_Line(userdata, x, y, x + vx, y - vy);
		
		if(err <= 0)
		{
			vy++;
			err += dy;
			dy += 2;
		}
		else
		{
			vx--;
			dx += 2;
			err += dx - (r << 1);
		}
	}
	
	return 0;
}

/* Draw Triangle
 * Refer to https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
int HB_Triangle(void* userdata, int x1, int y1, int x2, int y2, int x3, int y3)
{
	HB_Line(userdata, x1, y1, x2, y2);
	HB_Line(userdata, x1, y1, x3, y3);
	HB_Line(userdata, x2, y2, x3, y3);
	
	return 0;
}


/* Fill Triangle
 * Refer to https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
int HB_FillTriangle(void* userdata, int x1, int y1, int x2, int y2, int x3, int y3)
{
	/* P1(x1,y1) P2(x2,y2) P3(x3,y3) are from left to right */
	if(x1 > x2)
	{
		swap(&x1, &x2);
		swap(&y1, &y2);
	}
	
	if(x2 > x3)
	{
		swap(&x2, &x3);
		swap(&y2, &y3);
	}
	
	if(x1 > x3)
	{
		swap(&x1, &x3);
		swap(&y1, &y3);
	}
	
	/* P2 - M(xm,ym) perpendicular to x raw and cross P1-P3 at M */
	int xm = x2;
	int ym = y1 + (double)(y3-y1)/(x3-x1)*(xm-x1) + 0.5;
	
	double d12 = (double)(y2-y1)/(x2-x1);
	double d23 = (double)(y3-y2)/(x3-x2);
	double d1m = (double)(y3-y1)/(x3-x1);
	
	for(int x = 0; x <= x2-x1; x++)
	{
		HB_Line(userdata, x1+x, y1+d12*x+0.5, x1+x, y1+d1m*x+0.5);
	}
	
	for(int x = 0; x <= x3-x2; x++)
	{
		HB_Line(userdata, x2+x, y2+d23*x+0.5, x2+x, ym+d1m*x+0.5);
	}
	
	return 0;
}

/* Draw Ellipse
 * Refer to https://blog.csdn.net/orbit/article/details/7496008
 */
int HB_Ellipse(void* userdata, int x , int y , int rx, int ry)
{
	int sq_rx = rx * rx;
	int sq_ry = ry * ry;
	
	int vx = 0;
	int vy = ry;
	int d  = 2 * sq_ry - 2 * ry * sq_rx + sq_rx;
	int p  = (double)sq_rx/sqrt((double)(sq_rx+sq_ry)) + 0.5;
	ellipse_put4pixel(userdata, x, y, vx, vy);
	while(vx <= p)
	{
		if(d < 0)
		{
			d += 2 * sq_ry * (2 * vx + 3);
		}
		else
		{
			d += 2 * sq_ry * (2 * vx + 3) - 4 * sq_rx * (vy - 1);
			vy--;
		}
		vx++;
		ellipse_put4pixel(userdata, x, y, vx, vy);
	}
	
	d = sq_ry * (vx * vx + vx) + sq_rx * (vy * vy - vy) - sq_rx * sq_ry;
	while(vy >= 0)
	{
		ellipse_put4pixel(userdata, x, y, vx, vy);
		vy--;
		if(d < 0)
		{
			vx++;
			d -= 2 * sq_rx * vy + sq_rx - 2 * sq_ry * vx - 2 * sq_ry;
		}
		else
		{
			d -= 2 * sq_rx * vy + sq_rx;
		}
	}
	
	return 0;
}


/* Fill Ellipse
 * Refer to https://blog.csdn.net/orbit/article/details/7496008
 */
int HB_FillEllipse(void* userdata, int x , int y , int rx, int ry)
{
	int sq_rx = rx * rx;
	int sq_ry = ry * ry;
	
	int vx = 0;
	int vy = ry;
	int d  = 2 * sq_ry - 2 * ry * sq_rx + sq_rx;
	int p  = (double)sq_rx/sqrt((double)(sq_rx+sq_ry)) + 0.5;
	while(vx <= p)
	{
		if(d < 0)
		{
			d += 2 * sq_ry * (2 * vx + 3);
		}
		else
		{
			d += 2 * sq_ry * (2 * vx + 3) - 4 * sq_rx * (vy - 1);
			vy--;
		}
		vx++;
		HB_Line(userdata, x + vx, y + vy, x - vx, y + vy);
		HB_Line(userdata, x + vx, y - vy, x - vx, y - vy);
	}
	
	d = sq_ry * (vx * vx + vx) + sq_rx * (vy * vy - vy) - sq_rx * sq_ry;
	while(vy >= 0)
	{
		HB_Line(userdata, x + vx, y + vy, x - vx, y + vy);
		HB_Line(userdata, x + vx, y - vy, x - vx, y - vy);
		vy--;
		if(d < 0)
		{
			vx++;
			d -= 2 * sq_rx * vy + sq_rx - 2 * sq_ry * vx - 2 * sq_ry;
		}
		else
		{
			d -= 2 * sq_rx * vy + sq_rx;
		}
	}
	
	return 0;
}


 
/* Private */
static void swap(int* n, int* m)
{
	*n ^= *m;
	*m ^= *n;
	*n ^= *m;
}

static void ellipse_put4pixel(void* userdata, int x, int y, int vx, int vy)
{
	HB_PutPixel(userdata, x + vx, y + vy);
	HB_PutPixel(userdata, x + vx, y - vy);
	HB_PutPixel(userdata, x - vx, y + vy);
	HB_PutPixel(userdata, x - vx, y - vy);
}
