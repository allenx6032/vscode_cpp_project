#pragma once

#include "pype_common.h"
#include "pype_Vector2.h"
#include "pype_Texture2D.h"

inline static px_void _PX_TextureRenderEx(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float Angle, bool flip){
	px_int newWidth,newHeight;
	
	px_int i,j,resHeight,resWidth;

	px_float invCosAgl,invSinAgl;

	px_double ref_x,ref_y;
	px_int mapX,mapY;

	px_color clr;
	px_int bA,bR,bG,bB;

	//////////////////////////////////////////////////////////////////////////
	px_int left,right,top,bottom;

	
	//////////////////////////////////////////////////////////////////////////

	resHeight=resTexture->height;
	resWidth=resTexture->width;

	newHeight=PX_TRUNC(resHeight*scale);
	newWidth=PX_TRUNC(resWidth*scale);

	//////////////////////////////////////////////////////////////////////////
	switch (refPoint)
	{
	case PX_ALIGN_LEFTTOP:
		break;
	case PX_ALIGN_MIDTOP:
		x-=newWidth/2;
		break;
	case PX_ALIGN_RIGHTTOP:
		x-=newWidth;
		break;
	case PX_ALIGN_LEFTMID:
		y-=newHeight/2;
		break;
	case PX_ALIGN_CENTER:
		y-=newHeight/2;
		x-=newWidth/2;
		break;
	case PX_ALIGN_RIGHTMID:
		y-=newHeight/2;
		x-=newWidth;
		break;
	case PX_ALIGN_LEFTBOTTOM:
		y-=newHeight;
		break;
	case PX_ALIGN_MIDBOTTOM:
		y-=newHeight;
		x-=newWidth/2;
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		y-=newHeight;
		x-=newWidth;
		break;
	}


	if (x<-newWidth)
	{
		return;
	}
	if (x>psurface->width-1)
	{
		return;
	}
	if (y<-newHeight)
	{
		return;
	}
	if (y>psurface->height-1)
	{
		return;
	}

	if (x<0)
	{
		left=-x;
	}
	else
	{
		left=0;
	}

	if (x+newWidth>psurface->width)
	{
		right=psurface->width-x-1;
	}
	else
	{
		right=newWidth-1;
	}

	if (y<0)
	{
		top=-y;
	}
	else
	{
		top=0;
	}

	if (y+newHeight>psurface->height)
	{
		bottom=psurface->height-y-1;
	}
	else
	{
		bottom=newHeight-1;
	}
	//////////////////////////////////////////////////////////////////////////


	invCosAgl=PX_cos_angle(-Angle);
	invSinAgl=PX_sin_angle(-Angle);

    px_int Ab=(px_int)(blend->alpha*1000);
    px_int Rb=(px_int)(blend->hdr_R*1000);
    px_int Gb=(px_int)(blend->hdr_G*1000);
    px_int Bb=(px_int)(blend->hdr_B*1000);

    for (j=top;j<=bottom;j++)
    {
        for (i=left;i<=right;i++)
        {
            ref_x = (i - newWidth / 2.0);
            ref_y = (j - newHeight / 2.0);

            mapX = (px_int)((ref_x * invCosAgl - ref_y * invSinAgl) / scale + resWidth / 2);
            mapY = (px_int)((ref_x * invSinAgl + ref_y * invCosAgl) / scale + resHeight / 2);

            if (mapX<0||mapX>=resWidth)
            {
                continue;
            }

            if (mapY<0||mapY>=resHeight)
            {
                continue;
            }

            if(flip){
                mapX = resWidth - mapX - 1;
            }

            clr=PX_SURFACECOLOR(resTexture,mapX,mapY);
            bA=(px_int)(clr._argb.a*Ab/1000);
            bR=(px_int)(clr._argb.r*Rb/1000);
            bG=(px_int)(clr._argb.g*Gb/1000);
            bB=(px_int)(clr._argb.b*Bb/1000);

            clr._argb.a=bA>255?255:(px_uchar)bA;
            clr._argb.r=bR>255?255:(px_uchar)bR;
            clr._argb.g=bG>255?255:(px_uchar)bG;
            clr._argb.b=bB>255?255:(px_uchar)bB;
            PX_SurfaceDrawPixel(psurface,x+i,y+j,clr);
        }

    }
}

using namespace pkpy;

struct GL {
    PY_CLASS(GL, pype, gl)

    static void _register(VM* vm, PyObject* mod, PyObject* type){
        vm->bind_func<6>(type, "draw_texture", [](VM* vm, ArgsView args){
            px_surface* psurface = &App.runtime.RenderSurface;
            Texture2D& tex = CAST(Texture2D&, args[0]);
            Vector2& pos = CAST(Vector2&, args[1]);
            float angle = vm->num_to_float(args[2]);
            float scale = vm->num_to_float(args[3]);
            Color& color = CAST(Color&, args[4]);
            bool flip = CAST(bool, args[5]);
            _PX_TextureRenderEx(psurface, tex.ptr, pos.x, pos.y, PX_ALIGN_CENTER, color.blend(), scale, angle, flip);
            return vm->None;
        });

        vm->bind_func<6>(type, "draw_line", [](VM* vm, ArgsView args){
            px_surface* psurface = &App.runtime.RenderSurface;
            float x0 = vm->num_to_float(args[0]);
			float y0 = vm->num_to_float(args[1]);
			float x1 = vm->num_to_float(args[2]);
			float y1 = vm->num_to_float(args[3]);
            float line_width = vm->num_to_float(args[4]);
            Color& color = CAST(Color&, args[5]);
            PX_GeoDrawLine(psurface, x0, y0, x1, y1, line_width, color);
            return vm->None;
        });

        vm->bind_func<4>(type, "draw_rect", [](VM* vm, ArgsView args){
            px_surface* psurface = &App.runtime.RenderSurface;
            Rect& rect = CAST(Rect&, args[0]);
            float line_width = vm->num_to_float(args[1]);
            Color& color = CAST(Color&, args[2]);
            float border_radius = vm->num_to_float(args[3]);
            PX_GeoDrawRoundRect(psurface, rect.left(), rect.top(), rect.right(), rect.bottom(), border_radius, line_width, color);
            return vm->None;
        });

        vm->bind_func<3>(type, "draw_solid_rect", [](VM* vm, ArgsView args){
            px_surface* psurface = &App.runtime.RenderSurface;
            Rect& rect = CAST(Rect&, args[0]);
            Color& color = CAST(Color&, args[1]);
            float border_radius = vm->num_to_float(args[2]);
            PX_GeoDrawSolidRoundRect(psurface, rect.left(), rect.top(), rect.right(), rect.bottom(), border_radius, color);
            return vm->None;
        });

        vm->bind_func<5>(type, "draw_circle", [](VM* vm, ArgsView args){
            px_surface* psurface = &App.runtime.RenderSurface;
            float x = vm->num_to_float(args[0]);
			float y = vm->num_to_float(args[1]);
            float radius = vm->num_to_float(args[2]);
            float line_width = vm->num_to_float(args[3]);
            Color& color = CAST(Color&, args[4]);
            PX_GeoDrawCircle(psurface, x, y, radius, line_width, color);
            return vm->None;
        });

        vm->bind_func<4>(type, "draw_solid_circle", [](VM* vm, ArgsView args){
            px_surface* psurface = &App.runtime.RenderSurface;
            float x = vm->num_to_float(args[0]);
			float y = vm->num_to_float(args[1]);
            float radius = vm->num_to_float(args[2]);
            Color& color = CAST(Color&, args[3]);
            PX_GeoDrawSolidCircle(psurface, x, y, radius, color);
            return vm->None;
        });
    }
};