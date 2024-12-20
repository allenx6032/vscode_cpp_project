#ifndef HIDE_ON_BRUSH_H
#define HIDE_ON_BRUSH_H

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>

typedef int (*HB_PixelFunction)(void* userdata, int x, int y);
typedef int (*HB_ColorFunction)(void* userdata, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

typedef struct HB_State
{
	HB_PixelFunction putPixel;
	HB_ColorFunction setColor;
	int8_t red; 
	int8_t green; 
	int8_t blue; 
	int8_t alpha;
}HB_State;

extern HB_State HB_GlobalState;

int HB_SetPixelFunction(HB_PixelFunction pixel);
int HB_SetColorFunction(HB_ColorFunction color);
int HB_SetColor(void* userdata, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
int HB_PutPixel(void* userdata, int x, int y);

int HB_Line(void* userdata, int x1, int y1, int x2, int y2);
int HB_Circle(void* userdata, int x, int y, int r);
int HB_FillCircle(void* userdata, int x, int y, int r);
int HB_Triangle(void* userdata, int x1, int y1, int x2, int y2, int x3, int y3);
int HB_FillTriangle(void* userdata, int x1, int y1, int x2, int y2, int x3, int y3);
int HB_Ellipse(void* userdata, int x , int y , int rx, int ry);
int HB_FillEllipse(void* userdata, int x , int y , int rx, int ry);

#ifdef __cplusplus
	}
#endif

#endif