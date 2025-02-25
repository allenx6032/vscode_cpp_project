/***************************************************************************
 *   Copyright (C) 2017 by SWE team <sdl.window.engine@gmail.com>          *
 *                                                                         *
 *   Part of the SWE: SDL Window Engine:                                   *
 *   https://github.com/AndreyBarmaley/sdl-window-engine                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <sstream>

#include "swe_systems.h"
#include "swe_display.h"
#include "swe_cstring.h"
#include "swe_surface.h"

#ifdef SWE_SDL12
 #include "SDL_rotozoom.h"
 #ifndef SWE_DISABLE_IMAGE
 #include "IMG_savepng.h"
 #endif
#else
 #ifdef SWE_BUILTIN_ROOTOZOOM2
  #include "SDL2_rotozoom.h"
 #else
  #include "SDL2_rotozoom.h"
 #endif
#endif

#ifdef SWE_WITH_JSON
#include "swe_json_ext.h"
#endif
namespace SWE
{
    void skipDeleter(SDL_Surface* sf)
    {
    }

    Surface::Surface(SDL_Surface* sf) : ptr{nullptr, SDL_FreeSurface}
    {
        if(sf)
        {
#ifdef SWE_SDL12
            if(sf == SDL_GetVideoSurface())
                ptr.reset(sf, skipDeleter);
            else
                ptr.reset(sf, SDL_FreeSurface);
#else
            ptr.reset(sf, SDL_FreeSurface);
#endif
        }
        else
            ERROR(SDL_GetError());
    }

    Surface::Surface(const Size & sz, bool alpha)
    {
#ifdef SWE_SDL12
        int flag = SDL_SWSURFACE;
#else
        int flag = 0;
#endif
        auto sf = SDL_CreateRGBSurface(flag, sz.w, sz.h, 32, defRMask(), defGMask(), defBMask(), (alpha ? defAMask() : 0));

        if(sf)
        {
            ptr.reset(sf, SDL_FreeSurface);

            if(amask())
            {
                fill(rect(), Color::transparent());
            }
            else
            {
                Color colorKey = Color::colorKey();
                fill(rect(), colorKey);
                setColorKey(colorKey);
            }
        }
        else
            ERROR(SDL_GetError());
    }

    Surface::Surface(const std::string & file) : ptr{nullptr, SDL_FreeSurface}
    {
#ifdef SWE_DISABLE_IMAGE
        auto sf = SDL_LoadBMP(file.c_str());
#else
        auto sf = IMG_Load(file.c_str());
#endif
        if(sf)
            ptr.reset(sf, SDL_FreeSurface);
        else
            ERROR(SDL_GetError());
    }

    Surface::Surface(const BinaryBuf & raw) : ptr{nullptr, SDL_FreeSurface}
    {
        SDL_RWops* rw = SDL_RWFromConstMem(raw.data(), raw.size());

        if(! rw)
            ERROR(SDL_GetError());

#ifdef SWE_DISABLE_IMAGE
        auto sf = SDL_LoadBMP_RW(rw, 1);
#else
        auto sf = IMG_Load_RW(rw, 1);
#endif

        if(sf)
            ptr.reset(sf, SDL_FreeSurface);
        else
            ERROR(SDL_GetError());
    }

    void Surface::setSurface(const Surface & other)
    {
        ptr = other.ptr;
    }

    Surface Surface::copy(const Surface & sf, const Rect & rt)
    {
        SDL_Surface* sf2 = nullptr;

        if(sf.rect() == rt)
        {
            sf2 = SDL_ConvertSurface(sf.ptr.get(), sf.ptr->format, 0);
        }
        else
        {
            auto sf1 = sf.toSDLSurface();
            sf2 = SDL_CreateRGBSurface(sf.flags(), rt.w, rt.h, sf.depth(),
                        sf.rmask(), sf.gmask(), sf.bmask(), sf.amask());
            if(sf2)
            {
                SDL_Rect srt = rt.toSDLRect();
            
                if(0 > SDL_BlitSurface(sf1, & srt, sf2, nullptr))
                    ERROR(SDL_GetError());
            }
        }

        return Surface(sf2);
    }

    Surface Surface::scale(const Surface & sf1, const Size & sz, bool smooth)
    {
        auto ratioWidth = sz.w / static_cast<double>(sf1.width());
        auto ratioHeight = sz.h / static_cast<double>(sf1.height());
        auto sf = zoomSurface(sf1.ptr.get(), ratioWidth, ratioHeight, smooth);
	return Surface(sf);
    }

    Surface Surface::copy(const Surface & sf1, int flip)
    {
        Surface sf2(SDL_ConvertSurface(sf1.ptr.get(), sf1.ptr->format, 0));

    	if((flip & FlipVertical) && (flip & FlipHorizontal))
	{
    	    for(int py = 0; py < sf1.height(); ++py)
            	for(int px = 0; px < sf1.width(); ++px)
            	    sf2.drawPixel(Point(sf1.width() - px - 1, sf1.height() - py - 1), sf1.pixel(Point(px, py)));
        }
	else
    	if(flip & FlipVertical)
	{
    	    for(int py = 0; py < sf1.height(); ++py)
		sf1.blit(Rect(0, py, sf1.width(), 1), Rect(0, sf1.height() - py - 1, sf1.width(), 1), sf2);
        }
	else
	if(flip & FlipHorizontal)
	{
            for(int px = 0; px < sf1.width(); ++px)
		sf1.blit(Rect(px, 0, 1, sf1.height()), Rect(sf1.width() - px - 1, 0, 1, sf1.height()), sf2);
        }

	if(flip & Rotate90Degrees)
	{
	    Surface tmp(rotateSurface90Degrees(sf2.ptr.get(), 1));
	    sf2.swap(tmp);
	}
	else
	if(flip & Rotate180Degrees)
	{
	    Surface tmp(rotateSurface90Degrees(sf2.ptr.get(), 2));
	    sf2.swap(tmp);
	}
	else
	if(flip & Rotate270Degrees)
	{
	    Surface tmp(rotateSurface90Degrees(sf2.ptr.get(), 3));
	    sf2.swap(tmp);
	}

        return sf2;
    }

    Surface Surface::renderGrayScale(const Surface & sf)
    {
	Surface res(SDL_ConvertSurface(sf.ptr.get(), sf.ptr->format, sf.ptr->flags));
	u32 colkey = sf.amask() ? 0 : sf.mapRGB(sf.colorKey());
	u32 pixel = 0;

	for(int py = 0; py < sf.height(); ++py)
    	    for(int px = 0; px < sf.width(); ++px)
	{
    	    pixel = sf.pixel(Point(px, py));
    	    if(0 == colkey || pixel != colkey)
    	    {
        	ARGB col = sf.RGBmap(pixel);
        	int z = col.r() * 0.299f + col.g() * 0.587f + col.b() * 0.114f;
        	res.drawPixel(Point(px, py), res.mapRGB(Color(z, z, z, col.a())));
    	    }
	}

	return res;
    }

    Surface Surface::renderSepia(const Surface & sf)
    {
	Surface res(SDL_ConvertSurface(sf.ptr.get(), sf.ptr->format, sf.ptr->flags));
	u32 colkey = sf.amask() ? 0 : sf.mapRGB(sf.colorKey());
	u32 pixel = 0;

	for(int py = 0; py < sf.height(); ++py)
    	    for(int px = 0; px < sf.width(); ++px)
	{
    	    pixel = sf.pixel(Point(px, py));
    	    if(colkey == 0 || pixel != colkey)
    	    {
        	ARGB col = sf.RGBmap(pixel);
        	//Numbers derived from http://blogs.techrepublic.com.com/howdoi/?p=120
        	#define CLAMP255(val) std::min<u16>((val), 255)
        	int outR = CLAMP255(static_cast<u16>(col.r() * 0.693f + col.g() * 0.769f + col.b() * 0.189f));
        	int outG = CLAMP255(static_cast<u16>(col.r() * 0.449f + col.g() * 0.686f + col.b() * 0.168f));
        	int outB = CLAMP255(static_cast<u16>(col.r() * 0.272f + col.g() * 0.534f + col.b() * 0.131f));
        	pixel = res.mapRGB(Color(outR, outG, outB, col.a()));
        	res.drawPixel(Point(px, py), pixel);
        	#undef CLAMP255
    	    }
	}

	return res;
    }

    void Surface::swap(Surface & sf)
    {
        std::swap(ptr, sf.ptr);
    }

    void Surface::reset(void)
    {
        ptr.reset();
    }

#ifdef SWE_SDL12
    void Surface::convertToDisplayFormat(void)
    {
	if(ptr)
	{
    	    SDL_Surface* sf = amask() ? SDL_DisplayFormatAlpha(ptr.get()) : SDL_DisplayFormat(ptr.get());
	    ptr.reset(sf, SDL_FreeSurface);
	}
    }
#endif

    bool Surface::isValid(void) const
    {
        return ptr.get();
    }

    SDL_Surface* Surface::toSDLSurface(void) const
    {
        return ptr.get();
    }

    bool Surface::operator== (const Surface & sf) const
    {
        return ptr == sf.ptr;
    }

    bool Surface::operator!= (const Surface & sf) const
    {
        return ptr != sf.ptr;
    }

    int Surface::width(void) const
    {
        return ptr ? ptr->w : 0;
    }

    int Surface::height(void) const
    {
        return ptr ? ptr->h : 0;
    }

    Size Surface::size(void) const
    {
        return Size(width(), height());
    }

    Rect Surface::rect(void) const
    {
        return Rect(Point(0, 0), size());
    }

    u32 Surface::mapRGB(const Color & col) const
    {
        if(ptr)
        {
            return amask() ?
                   SDL_MapRGBA(ptr->format, col.r(), col.g(), col.b(), col.a()) :
                   SDL_MapRGB(ptr->format, col.r(), col.g(), col.b());
        }

        return 0;
    }

    Color Surface::RGBmap(u32 val) const
    {
        Uint8 r = 0;
        Uint8 g = 0;
        Uint8 b = 0;
        Uint8 a = 0;

        if(ptr)
        {
            if(amask())
                SDL_GetRGBA(val, ptr->format, &r, &g, &b, &a);
            else
                SDL_GetRGB(val, ptr->format, &r, &g, &b);
        }

        return Color(r, g, b, a);
    }

    int Surface::flags(void) const
    {
        return ptr ? ptr->flags : 0;
    }

    int Surface::depth(void) const
    {
        return ptr ? ptr->format->BitsPerPixel : 0;
    }

    u32 Surface::rmask(void) const
    {
        return ptr ? ptr->format->Rmask : 0;
    }

    u32 Surface::gmask(void) const
    {
        return ptr ? ptr->format->Gmask : 0;
    }

    u32 Surface::bmask(void) const
    {
        return ptr ? ptr->format->Bmask : 0;
    }

    u32 Surface::amask(void) const
    {
        return ptr ? ptr->format->Amask : 0;
    }


    void Surface::clear(const Color & col)
    {
        fill(rect(), col);
    }

    int Surface::alphaMod(void) const
    {
        Uint8 res = 0;

        if(ptr)
        {
#ifdef SWE_SDL12
            res = ptr->format->alpha;
#else
            if(0 != SDL_GetSurfaceAlphaMod(ptr.get(), & res))
                ERROR(SDL_GetError());
#endif
        }

        return res;
    }

    Color Surface::colorKey(void) const
    {
        Uint32 key = 0;

        if(ptr)
        {
#ifdef SWE_SDL12
            if(ptr->flags & SDL_SRCCOLORKEY)
                key = ptr->format->colorkey;
#else
            if(0 != SDL_GetColorKey(ptr.get(), &key))
                ERROR(SDL_GetError());
#endif
        }

        return RGBmap(key);
    }

    void Surface::setColorKey(const Color & col)
    {
        if(ptr)
        {
#ifdef SWE_SDL12
            SDL_SetColorKey(ptr.get(), SDL_SRCCOLORKEY, mapRGB(col));
#else
            SDL_SetColorKey(ptr.get(), SDL_TRUE, mapRGB(col));
#endif
        }
    }

    void Surface::setAlphaMod(int val)
    {
        if(ptr)
        {
#ifdef SWE_SDL12
            if(0 != SDL_SetAlpha(ptr.get(), (0 < val ? SDL_SRCALPHA : 0), val))
                ERROR(SDL_GetError());

#else
            if(0 != SDL_SetSurfaceAlphaMod(ptr.get(), val))
                ERROR(SDL_GetError());
#endif
        }
    }

    void Surface::setFlags(int f)
    {
        if(ptr) ptr->flags |= f;
    }

    void Surface::resetFlags(int f)
    {
        if(ptr) ptr->flags &= ~f;
    }

    void Surface::draw4(const Point & dpt, u32 pixel)
    {
	u32* bufp = static_cast<u32 *>(ptr->pixels) + dpt.y * (ptr->pitch >> 2) + dpt.x;
	*bufp = pixel;
    }

    void Surface::draw3(const Point & dpt, u32 pixel)
    {
	u8* bufp = static_cast<u8 *>(ptr->pixels) + dpt.y * ptr->pitch + dpt.x * 3;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	bufp[2] = pixel; bufp[1] = pixel >> 8; bufp[0] = pixel >> 16;
#else
	bufp[0] = pixel; bufp[1] = pixel >> 8; bufp[2] = pixel >> 16;
#endif
    }

    void Surface::draw2(const Point & dpt, u32 pixel)
    {
	u16* bufp = static_cast<u16 *>(ptr->pixels) + dpt.y * (ptr->pitch >> 1) + dpt.x;
	*bufp = static_cast<u16>(pixel);
    }

    void Surface::draw1(const Point & dpt, u32 pixel)
    {
	u8* bufp = static_cast<u8 *>(ptr->pixels) + dpt.y * ptr->pitch + dpt.x;
	*bufp = static_cast<u8>(pixel);
    }

    void Surface::drawPoint(const Point & dpt, const Color & col)
    {
        drawPixel(dpt, mapRGB(col));
    }

    void Surface::drawPixel(const Point & dpt, u32 pixel)
    {
        if(ptr)
        {
            if(0 <= dpt.x && dpt.x < ptr->w && 0 <= dpt.y && dpt.y < ptr->h)
            {
                SDL_LockSurface(ptr.get());
		switch(ptr->format->BytesPerPixel)
		{
		    case 1: draw1(dpt, pixel); break;
		    case 2: draw2(dpt, pixel); break;
		    case 3: draw3(dpt, pixel); break;
		    case 4: draw4(dpt, pixel); break;
		    default: break;
		}
                SDL_UnlockSurface(ptr.get());
            }
            else
                ERROR("out of range: " << "pos: " << dpt.toString() << ", " << "size: " << size().toString());
        }
    }

    u32 Surface::pixel4(const Point & dpt) const
    {
	u32* bufp = static_cast<u32 *>(ptr->pixels) + dpt.y * (ptr->pitch >> 2) + dpt.x;
	return *bufp;
    }

    u32 Surface::pixel3(const Point & dpt) const
    {
	u8* bufp = static_cast<u8 *>(ptr->pixels) + dpt.y * ptr->pitch + dpt.x * 3;
	u32 res = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	res |= bufp[0]; res <<= 8; res |= bufp[1]; res <<= 8; res |= bufp[2];
#else
        res |= bufp[2]; res <<= 8; res |= bufp[1]; res <<= 8; res |= bufp[0];
#endif
	return res;
    }

    u32 Surface::pixel2(const Point & dpt) const
    {
	u16* bufp = static_cast<u16 *>(ptr->pixels) + dpt.y * (ptr->pitch >> 1) + dpt.x;
	return static_cast<u32>(*bufp);
    }

    u32 Surface::pixel1(const Point & dpt) const
    {
	u8* bufp = static_cast<u8 *>(ptr->pixels) + dpt.y * ptr->pitch + dpt.x;
	return static_cast<u32>(*bufp);
    }

    u32 Surface::pixel(const Point & dpt) const
    {
        u32 res = 0;

        if(ptr)
        {
            if(0 <= dpt.x && dpt.x < ptr->w && 0 <= dpt.y && dpt.y < ptr->h)
            {
                SDL_LockSurface(ptr.get());
		switch(ptr->format->BytesPerPixel)
		{
		    case 1: res = pixel1(dpt); break;
		    case 2: res = pixel2(dpt); break;
		    case 3: res = pixel3(dpt); break;
		    case 4: res = pixel4(dpt); break;
		    default: break;
		}
                SDL_UnlockSurface(ptr.get());
            }
            else
                ERROR("out of range: " << "pos: " << dpt.toString() << ", " << "size: " << size().toString());
        }

        return res;
    }

    void Surface::fill(const Rect & drt, const Color & col)
    {
        if(ptr)
        {
            SDL_Rect dstRect = drt.toSDLRect();

            if(0 > SDL_FillRect(ptr.get(), & dstRect, mapRGB(col)))
                ERROR(SDL_GetError());
        }
    }

    void Surface::blit(const Rect & srt, const Rect & drt, Surface & dsf) const
    {
        if(ptr && dsf.ptr)
        {
            SDL_Rect srcRect = srt.toSDLRect();
            SDL_Rect dstRect = drt.toSDLRect();

            if(0 > SDL_BlitSurface(ptr.get(), & srcRect, dsf.ptr.get(), & dstRect))
                ERROR(SDL_GetError());
        }
    }

    bool Surface::save(const std::string & file) const
    {
        if(ptr)
        {
#ifdef SWE_DISABLE_IMAGE
            return 0 == SDL_SaveBMP(ptr.get(), file.c_str());
#else
#ifdef SWE_SDL12
            return 0 == IMG_SavePNG(file.c_str(), ptr.get(), 7);
#else
	    auto type = String::toLower(file.substr(file.size() - 3));
	    if(type == "png")
        	return 0 == IMG_SavePNG(ptr.get(), file.c_str());
	    else
	    // if(type == "jpg")
        // 	return 0 == IMG_SaveJPG(ptr.get(), file.c_str(), 90);
	    // else
	    {
		ERROR("unknown image format: " << type);
	    }
#endif
#endif
        }

        return false;
    }

    std::string Surface::toString(void) const
    {
    	std::ostringstream os;
        os << "self" << "(" << String::pointer(this) << "), ";

        if(isValid())
        {
	    os << "size" << "(" << size().toString() << "), " <<
               "bpp" << "(" << 32 << "), " <<
               "alphaMod" << "(" << alphaMod() << "), " <<
               "amask" << "(" << String::hex(amask()) << "), " <<
               "flags" << "(" << String::hex(flags()) << "), " <<
               "colorKey" << "(" << colorKey().toString() << "), " <<
    	       "refCount" << "(" << ptr.use_count() << ")";
        }
	else
	    os << "invalid";

    	return os.str();
    }

#ifdef SWE_WITH_JSON
    JsonObject Surface::toJson(void) const
    {
        JsonObject res = ObjectClass::toJson();

        if(! isValid())
        {
            res.addBoolean("valid", false);
            return res;
        }

        res.addString("self", String::pointer(this));
        res.addBoolean("valid", true);
        res.addArray("size", JsonPack::size(size()));
        res.addInteger("alphaMod", alphaMod());
        res.addString("amask", String::hex(amask()));
        res.addInteger("flags", flags());
        res.addString("colorKey", colorKey().toString());
        res.addInteger("refCount", ptr.use_count());
        return res;
    }
#endif
}
