#pragma once

#include "pype_common.h"
#include "pype_Color.h"
#include "pype_platform.h"
#include <sstream>

using namespace pkpy;

struct Texture2D {
    PY_CLASS(Texture2D, pype, Texture2D)

    px_texture* ptr;
    Texture2D(px_texture* ptr) : ptr(ptr) {}

    static void _register(VM* vm, PyObject* mod, PyObject* type){
        vm->bind_static_method<1>(type, "__new__", [](VM* vm, ArgsView args){
            px_texture* ptr = CAST(px_texture*, args[0]);
            return VAR_T(Texture2D, ptr);
        });

        vm->bind_method<0>(type, "__repr__", [](VM* vm, ArgsView args){
            Texture2D& self = CAST(Texture2D&, args[0]);
            std::stringstream ss;
            ss << "Texture2D(" << self.ptr->width << ", " << self.ptr->height << ")";
            return VAR(ss.str());
        });

#define PROPERTY(name)                                              \
        type->attr().set(#name, vm->property(                       \
            [](VM* vm, ArgsView args){                              \
                Texture2D& self = CAST(Texture2D&, args[0]);        \
                return VAR(self.ptr->name);                         \
            }));

        PROPERTY(width);
        PROPERTY(height);
        PROPERTY(limit_left);
        PROPERTY(limit_top);
        PROPERTY(limit_right);
        PROPERTY(limit_bottom);

#undef PROPERTY
        
    }
};

inline px_bool _PX_TextureCreateFromMemory(px_memorypool *mp,px_void *data,px_int size,px_texture *tex)
{
	px_int width;
	px_int height;
	if (PX_BitmapVerify(data,size))
	{
		width=PX_BitmapGetWidth(data);
		height=PX_BitmapGetHeight(data);
		if(PX_TextureCreate(mp,tex,width,height))
		{
			PX_BitmapRender(tex,data,size,0,0);
			return PX_TRUE;
		}
		else
		{
			PXError("PX_TextureCreate() 失败");
			return PX_FALSE;
		}

	}

	if (PX_TRawVerify(data,size))
	{
		width=PX_TRawGetWidth(data);
		height=PX_TRawGetHeight(data);
		if(PX_TextureCreate(mp,tex,width,height))
		{
			PX_TRawRender(tex,data,0,0);
			return PX_TRUE;
		}
		else
		{
			PXError("PX_TextureCreate() 失败");
			return PX_FALSE;
		}
	}

	if (PX_PngVerify((px_byte *)data,size,&width,&height,0))
	{
		if (PX_TextureCreate(mp, tex, width, height))
		{
			if (!PX_PngToRenderBuffer(mp, (px_byte*)data, size, tex)){
				PXError("PX_PngToRenderBuffer() 失败");
				return PX_FALSE;
			}
			return PX_TRUE;
		}
		else
		{
			PXError("PX_TextureCreate() 失败");
			return PX_FALSE;
		}
	}

	if (PX_JpgVerify((px_byte*)data, size))
	{
		PX_JpgDecoder decoder;
		if (PX_JpgDecoderInitialize(mp, &decoder, (px_byte*)data, size))
		{
			if (PX_TextureCreate(mp, tex, decoder.width, decoder.height))
			{
				PX_JpgDecoderRenderToSurface(&decoder, tex);
				PX_JpgDecoderFree(&decoder);
				return PX_TRUE;
			}
			PX_JpgDecoderFree(&decoder);
			PXError("PX_TextureCreate() 失败");
		}else{
			PXError("PX_JpgDecoderInitialize() 失败");
		}
	}
	PXError("不支持的图片格式");
	return PX_FALSE;
}