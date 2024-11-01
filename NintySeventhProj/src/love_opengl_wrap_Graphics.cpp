/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "love_opengl_wrap_Graphics.h"
#include "love_opengl_OpenGL.h"
#include "love_Texture.h"
#include "love_ImageData.h"
#include "love_Image.h"
#include "love_Rasterizer.h"
#include "love_wrap_Filesystem.h"

#include "love_graphics.lua.h"
#include <cassert>
#include <cstring>

namespace love
{
namespace graphics
{
namespace opengl
{

#define instance() (Module::getInstance<Graphics>(Module::M_GRAPHICS))

int w_reset(lua_State *)
{
	instance()->reset();
	return 0;
}

int w_clear(lua_State *)
{
	instance()->clear();
	return 0;
}

int w_present(lua_State *)
{
	instance()->present();
	return 0;
}

int w_isCreated(lua_State *L)
{
	luax_pushboolean(L, instance()->isCreated());
	return 1;
}

int w_getWidth(lua_State *L)
{
	lua_pushinteger(L, instance()->getWidth());
	return 1;
}

int w_getHeight(lua_State *L)
{
	lua_pushinteger(L, instance()->getHeight());
	return 1;
}

int w_getDimensions(lua_State *L)
{
	lua_pushinteger(L, instance()->getWidth());
	lua_pushinteger(L, instance()->getHeight());
	return 2;
}

int w_setScissor(lua_State *L)
{
	int nargs = lua_gettop(L);

	if (nargs == 0 || (nargs == 4 && lua_isnil(L, 1) && lua_isnil(L, 2)
		&& lua_isnil(L, 3) && lua_isnil(L, 4)))
	{
		instance()->setScissor();
		return 0;
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	int w = luaL_checkint(L, 3);
	int h = luaL_checkint(L, 4);

	if (w < 0 || h < 0)
		return luaL_error(L, "Can't set scissor with negative width and/or height.");

	instance()->setScissor(x, y, w, h);
	return 0;
}

int w_getScissor(lua_State *L)
{
	int x, y, w, h;
	if (!instance()->getScissor(x, y, w, h))
		return 0;

	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);

	return 4;
}

static int setStencil(lua_State *L, bool invert)
{
	// no argument -> clear stencil
	if (lua_isnoneornil(L, 1))
	{
		instance()->discardStencil();
		return 0;
	}

	luaL_checktype(L, 1, LUA_TFUNCTION);

	instance()->defineStencil();
	lua_call(L, lua_gettop(L) - 1, 0); // call stencil(...)
	instance()->useStencil(invert);

	return 0;
}

int w_setStencil(lua_State *L)
{
	return setStencil(L, false);
}

int w_setInvertedStencil(lua_State *L)
{
	return setStencil(L, true);
}

int w_getMaxTextureSize(lua_State *L)
{
	lua_pushinteger(L, instance()->getSystemLimit(Graphics::LIMIT_TEXTURE_SIZE));
	return 1;
}

int w_newImage(lua_State *L)
{
	love::image::ImageData *data = nullptr;
	love::image::CompressedData *cdata = nullptr;

	Image::Format format = Image::FORMAT_NORMAL;
	const char *fstr = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);

	if (fstr != nullptr && !Image::getConstant(fstr, format))
		return luaL_error(L, "Invalid Image format: %s", fstr);

	bool releasedata = false;

	// Convert to ImageData / CompressedData, if necessary.
	if (lua_isstring(L, 1) || luax_istype(L, 1, FILESYSTEM_FILE_T) || luax_istype(L, 1, FILESYSTEM_FILE_DATA_T))
	{
		love::image::Image *image = Module::getInstance<love::image::Image>(Module::M_IMAGE);
		if (image == nullptr)
			return luaL_error(L, "Cannot load images without the love.image module.");

		love::filesystem::FileData *fdata = love::filesystem::luax_getfiledata(L, 1);

		if (image->isCompressed(fdata))
		{
			luax_catchexcept(L,
				[&]() { cdata = image->newCompressedData(fdata); },
				[&]() { fdata->release(); }
			);
		}
		else
		{
			luax_catchexcept(L,
				[&]() { data = image->newImageData(fdata); },
				[&]() { fdata->release(); }
			);
		}

		// Lua's GC won't release the image data, so we should do it ourselves.
		releasedata = true;
	}
	else if (luax_istype(L, 1, IMAGE_COMPRESSED_DATA_T))
		cdata = luax_checktype<love::image::CompressedData>(L, 1, "CompressedData", IMAGE_COMPRESSED_DATA_T);
	else
		data = luax_checktype<love::image::ImageData>(L, 1, "ImageData", IMAGE_IMAGE_DATA_T);

	if (!data && !cdata)
		return luaL_error(L, "Error creating image (could not load data.)");

	// Create the image.
	Image *image = nullptr;
	luax_catchexcept(L,
		[&]() {
			if (cdata)
				image = instance()->newImage(cdata, format);
			else if (data)
				image = instance()->newImage(data, format);
		},
		[&]() {
			if (releasedata && data)
				data->release();
			else if (releasedata && cdata)
				cdata->release();
		}
	);

	if (image == nullptr)
		return luaL_error(L, "Could not load image.");

	// Push the type.
	luax_pushtype(L, "Image", GRAPHICS_IMAGE_T, image);
	image->release();
	return 1;
}

int w_newQuad(lua_State *L)
{
	Quad::Viewport v;
	v.x = (float) luaL_checknumber(L, 1);
	v.y = (float) luaL_checknumber(L, 2);
	v.w = (float) luaL_checknumber(L, 3);
	v.h = (float) luaL_checknumber(L, 4);

	float sw = (float) luaL_checknumber(L, 5);
	float sh = (float) luaL_checknumber(L, 6);

	Quad *quad = instance()->newQuad(v, sw, sh);
	luax_pushtype(L, "Quad", GRAPHICS_QUAD_T, quad);
	quad->release();
	return 1;
}

int w_newFont(lua_State *L)
{
	// Convert to Rasterizer, if necessary. Note that lua_isstring returns true
	// if the value is a number, which we rely on for the variant that uses the
	// default Font rather than a font file.
	if (lua_isnoneornil(L, 1) || lua_isstring(L, 1) || luax_istype(L, 1, FILESYSTEM_FILE_T) || luax_istype(L, 1, FILESYSTEM_FILE_DATA_T))
	{
		if (lua_isnone(L, 1))
			lua_pushnil(L);

		std::vector<int> idxs;
		for (int i = 0; i < lua_gettop(L); i++)
			idxs.push_back(i + 1);

		luax_convobj(L, &idxs[0], (int) idxs.size(), "font", "newRasterizer");
	}

	love::font::Rasterizer *rasterizer = luax_checktype<love::font::Rasterizer>(L, 1, "Rasterizer", FONT_RASTERIZER_T);

	Font *font = 0;
	luax_catchexcept(L, [&]() {
		font = instance()->newFont(rasterizer, instance()->getDefaultFilter()); }
	);

	if (font == 0)
		return luaL_error(L, "Could not load font.");

	// Push the type.
	luax_pushtype(L, "Font", GRAPHICS_FONT_T, font);
	font->release();
	return 1;
}

int w_newImageFont(lua_State *L)
{
	// filter for glyphs
	Texture::Filter filter = instance()->getDefaultFilter();

	// Convert to ImageData if necessary.
	if (lua_isstring(L, 1) || luax_istype(L, 1, FILESYSTEM_FILE_T) || luax_istype(L, 1, FILESYSTEM_FILE_DATA_T))
		luax_convobj(L, 1, "image", "newImageData");
	else if (luax_istype(L, 1, GRAPHICS_IMAGE_T))
	{
		Image *i = luax_checktype<Image>(L, 1, "Image", GRAPHICS_IMAGE_T);
		filter = i->getFilter();
		love::image::ImageData *id = i->getImageData();
		if (!id)
			return luaL_argerror(L, 1, "Image must not be compressed.");
		luax_pushtype(L, "ImageData", IMAGE_IMAGE_DATA_T, id);
		lua_replace(L, 1);
	}

	// Convert to Rasterizer if necessary.
	if (luax_istype(L, 1, IMAGE_IMAGE_DATA_T))
	{
		luaL_checkstring(L, 2);
		int idxs[] = {1, 2};
		luax_convobj(L, idxs, 2, "font", "newRasterizer");
	}

	love::font::Rasterizer *rasterizer = luax_checktype<love::font::Rasterizer>(L, 1, "Rasterizer", FONT_RASTERIZER_T);

	// Create the font.
	Font *font = instance()->newFont(rasterizer, filter);

	if (font == 0)
		return luaL_error(L, "Could not load font.");

	// Push the type.
	luax_pushtype(L, "Font", GRAPHICS_FONT_T, font);
	font->release();

	return 1;
}

int w_newSpriteBatch(lua_State *L)
{
	Texture *texture = luax_checktexture(L, 1);
	int size = luaL_optint(L, 2, 1000);
	SpriteBatch::UsageHint usage = SpriteBatch::USAGE_DYNAMIC;
	if (lua_gettop(L) > 2)
	{
		const char *usagestr = luaL_checkstring(L, 3);
		if (!SpriteBatch::getConstant(usagestr, usage))
			return luaL_error(L, "Invalid SpriteBatch usage hint: %s", usagestr);
	}

	SpriteBatch *t = nullptr;
	luax_catchexcept(L,
		[&](){ t = instance()->newSpriteBatch(texture, size, usage); }
	);

	luax_pushtype(L, "SpriteBatch", GRAPHICS_SPRITE_BATCH_T, t);
	t->release();
	return 1;
}

int w_newParticleSystem(lua_State *L)
{
	Texture *texture = luax_checktexture(L, 1);
	lua_Number size = luaL_optnumber(L, 2, 1000);
	ParticleSystem *t = 0;
	if (size < 1.0 || size > ParticleSystem::MAX_PARTICLES)
		return luaL_error(L, "Invalid ParticleSystem size");	

	luax_catchexcept(L,
		[&](){ t = instance()->newParticleSystem(texture, int(size)); }
	);

	luax_pushtype(L, "ParticleSystem", GRAPHICS_PARTICLE_SYSTEM_T, t);
	t->release();
	return 1;
}

int w_newCanvas(lua_State *L)
{
	// check if width and height are given. else default to screen dimensions.
	int width       = luaL_optint(L, 1, instance()->getWidth());
	int height      = luaL_optint(L, 2, instance()->getHeight());
	const char *str = luaL_optstring(L, 3, "normal");
	int msaa        = luaL_optint(L, 4, 0);

	Canvas::Format format;
	if (!Canvas::getConstant(str, format))
		return luaL_error(L, "Invalid Canvas format: %s", str);

	Canvas *canvas = nullptr;
	luax_catchexcept(L,
		[&](){ canvas = instance()->newCanvas(width, height, format, msaa); }
	);

	if (canvas == nullptr)
		return luaL_error(L, "Canvas not created, but no error thrown. I don't even...");

	luax_pushtype(L, "Canvas", GRAPHICS_CANVAS_T, canvas);
	canvas->release();
	return 1;
}

int w_newShader(lua_State *L)
{
	if (!Shader::isSupported())
		return luaL_error(L, "Sorry, your graphics card does not support shaders.");

	// clamp stack to 2 elements
	lua_settop(L, 2);

	// read any filepath arguments
	for (int i = 1; i <= 2; i++)
	{
		if (!lua_isstring(L, i))
			continue;

		// call love.filesystem.isFile(arg_i)
		luax_getfunction(L, "filesystem", "isFile");
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);

		bool isFile = luax_toboolean(L, -1);
		lua_pop(L, 1);

		if (isFile)
		{
			luax_getfunction(L, "filesystem", "read");
			lua_pushvalue(L, i);
			lua_call(L, 1, 1);
			lua_replace(L, i);
		}
		else
		{
			// Check if the argument looks like a filepath - we want a nicer
			// error for misspelled filepath arguments.
			size_t slen = 0;
			const char *str = lua_tolstring(L, i, &slen);
			if (slen > 0 && slen < 256 && !strchr(str, '\n'))
			{
				const char *ext = strchr(str, '.');
				if (ext != nullptr && !strchr(ext, ';') && !strchr(ext, ' '))
					return luaL_error(L, "Could not open file %s. Does not exist.", str);
			}
		}
	}

	bool has_arg1 = lua_isstring(L, 1);
	bool has_arg2 = lua_isstring(L, 2);

	// require at least one string argument
	if (!(has_arg1 || has_arg2))
		luaL_checkstring(L, 1);

	luax_getfunction(L, "graphics", "_shaderCodeToGLSL");

	// push vertexcode and pixelcode strings to the top of the stack
	lua_pushvalue(L, 1);
	lua_pushvalue(L, 2);

	// call effectCodeToGLSL, returned values will be at the top of the stack
	if (lua_pcall(L, 2, 2, 0) != 0)
		return luaL_error(L, "%s", lua_tostring(L, -1));

	Shader::ShaderSource source;

	// vertex shader code
	if (lua_isstring(L, -2))
		source.vertex = luax_checkstring(L, -2);
	else if (has_arg1 && has_arg2)
		return luaL_error(L, "Could not parse vertex shader code (missing 'position' function?)");

	// pixel shader code
	if (lua_isstring(L, -1))
		source.pixel = luax_checkstring(L, -1);
	else if (has_arg1 && has_arg2)
		return luaL_error(L, "Could not parse pixel shader code (missing 'effect' function?)");

	if (source.vertex.empty() && source.pixel.empty())
	{
		// Original args had source code, but effectCodeToGLSL couldn't translate it
		for (int i = 1; i <= 2; i++)
		{
			if (lua_isstring(L, i))
				return luaL_argerror(L, i, "missing 'position' or 'effect' function?");
		}
	}

	bool should_error = false;
	try
	{
		Shader *shader = instance()->newShader(source);
		luax_pushtype(L, "Shader", GRAPHICS_SHADER_T, shader);
		shader->release();
	}
	catch (love::Exception &e)
	{
		luax_getfunction(L, "graphics", "_transformGLSLErrorMessages");
		lua_pushstring(L, e.what());

		// Function pushes the new error string onto the stack.
		lua_pcall(L, 1, 1, 0);
		should_error = true;
	}

	if (should_error)
		return lua_error(L);

	return 1;
}

int w_newMesh(lua_State *L)
{
	// Check first argument: table of vertices or number of vertices.
	int ttype = lua_type(L, 1);
	if (ttype != LUA_TTABLE && ttype != LUA_TNUMBER)
		luaL_argerror(L, 1, "table or number expected");

	// Second argument: optional texture.
	Texture *tex = nullptr;
	if (!lua_isnoneornil(L, 2))
		tex = luax_checktexture(L, 2);

	// Third argument: optional draw mode.
	const char *str = 0;
	Mesh::DrawMode mode = Mesh::DRAW_MODE_FAN;
	str = lua_isnoneornil(L, 3) ? 0 : luaL_checkstring(L, 3);

	if (str && !Mesh::getConstant(str, mode))
		return luaL_error(L, "Invalid mesh draw mode: %s", str);

	Mesh *t = nullptr;

	if (ttype == LUA_TTABLE)
	{
		size_t vertex_count = lua_objlen(L, 1);
		std::vector<Vertex> vertices;
		vertices.reserve(vertex_count);

		bool use_colors = false;

		// Get the vertices from the table.
		for (size_t i = 1; i <= vertex_count; i++)
		{
			lua_rawgeti(L, 1, i);

			if (lua_type(L, -1) != LUA_TTABLE)
				return luax_typerror(L, 1, "table of tables");

			for (int j = 1; j <= 8; j++)
				lua_rawgeti(L, -j, j);

			Vertex v;

			v.x = (float) luaL_checknumber(L, -8);
			v.y = (float) luaL_checknumber(L, -7);

			v.s = (float) luaL_optnumber(L, -6, 0.0);
			v.t = (float) luaL_optnumber(L, -5, 0.0);

			v.r = (unsigned char) luaL_optinteger(L, -4, 255);
			v.g = (unsigned char) luaL_optinteger(L, -3, 255);
			v.b = (unsigned char) luaL_optinteger(L, -2, 255);
			v.a = (unsigned char) luaL_optinteger(L, -1, 255);

			// Enable per-vertex coloring if any color is not the default.
			if (!use_colors && (v.r != 255 || v.g != 255 || v.b != 255 || v.a != 255))
				use_colors = true;

			lua_pop(L, 9);
			vertices.push_back(v);
		}

		luax_catchexcept(L, [&](){ t = instance()->newMesh(vertices, mode); });
		t->setVertexColors(use_colors);
	}
	else
	{
		int count = luaL_checkint(L, 1);
		luax_catchexcept(L, [&](){ t = instance()->newMesh(count, mode); });
	}

	if (tex)
		t->setTexture(tex);

	luax_pushtype(L, "Mesh", GRAPHICS_MESH_T, t);
	t->release();
	return 1;
}

int w_setColor(lua_State *L)
{
	Color c;
	if (lua_istable(L, 1))
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);

		c.r = (unsigned char)luaL_checkint(L, -4);
		c.g = (unsigned char)luaL_checkint(L, -3);
		c.b = (unsigned char)luaL_checkint(L, -2);
		c.a = (unsigned char)luaL_optint(L, -1, 255);

		lua_pop(L, 4);
	}
	else
	{
		c.r = (unsigned char)luaL_checkint(L, 1);
		c.g = (unsigned char)luaL_checkint(L, 2);
		c.b = (unsigned char)luaL_checkint(L, 3);
		c.a = (unsigned char)luaL_optint(L, 4, 255);
	}
	instance()->setColor(c);
	return 0;
}

int w_getColor(lua_State *L)
{
	Color c = instance()->getColor();
	lua_pushinteger(L, c.r);
	lua_pushinteger(L, c.g);
	lua_pushinteger(L, c.b);
	lua_pushinteger(L, c.a);
	return 4;
}

int w_setBackgroundColor(lua_State *L)
{
	Color c;
	if (lua_istable(L, 1))
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);

		c.r = (unsigned char)luaL_checkint(L, -4);
		c.g = (unsigned char)luaL_checkint(L, -3);
		c.b = (unsigned char)luaL_checkint(L, -2);
		c.a = (unsigned char)luaL_optint(L, -1, 255);

		lua_pop(L, 4);
	}
	else
	{
		c.r = (unsigned char)luaL_checkint(L, 1);
		c.g = (unsigned char)luaL_checkint(L, 2);
		c.b = (unsigned char)luaL_checkint(L, 3);
		c.a = (unsigned char)luaL_optint(L, 4, 255);
	}
	instance()->setBackgroundColor(c);
	return 0;
}

int w_getBackgroundColor(lua_State *L)
{
	Color c = instance()->getBackgroundColor();
	lua_pushinteger(L, c.r);
	lua_pushinteger(L, c.g);
	lua_pushinteger(L, c.b);
	lua_pushinteger(L, c.a);
	return 4;
}

int w_setNewFont(lua_State *L)
{
	int ret = w_newFont(L);
	Font *font = luax_checktype<Font>(L, -1, "Font", GRAPHICS_FONT_T);
	instance()->setFont(font);
	return ret;
}

int w_setFont(lua_State *L)
{
	Font *font = luax_checktype<Font>(L, 1, "Font", GRAPHICS_FONT_T);
	instance()->setFont(font);
	return 0;
}

int w_getFont(lua_State *L)
{
	Font *f = nullptr;
	luax_catchexcept(L, [&](){ f = instance()->getFont(); });

	luax_pushtype(L, "Font", GRAPHICS_FONT_T, f);
	return 1;
}

int w_setColorMask(lua_State *L)
{
	Graphics::ColorMask mask;

	if (lua_gettop(L) <= 1 && lua_isnoneornil(L, 1))
	{
		// Enable all color components if no argument is given.
		mask.r = mask.g = mask.b = mask.a = true;
	}
	else
	{
		mask.r = luax_toboolean(L, 1);
		mask.g = luax_toboolean(L, 2);
		mask.b = luax_toboolean(L, 3);
		mask.a = luax_toboolean(L, 4);
	}

	instance()->setColorMask(mask);

	return 0;
}

int w_getColorMask(lua_State *L)
{
	Graphics::ColorMask mask = instance()->getColorMask();

	luax_pushboolean(L, mask.r);
	luax_pushboolean(L, mask.g);
	luax_pushboolean(L, mask.b);
	luax_pushboolean(L, mask.a);

	return 4;
}

int w_setBlendMode(lua_State *L)
{
	Graphics::BlendMode mode;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, mode))
		return luaL_error(L, "Invalid blend mode: %s", str);

	luax_catchexcept(L, [&](){ instance()->setBlendMode(mode); });
	return 0;
}

int w_getBlendMode(lua_State *L)
{
	const char *str;
	Graphics::BlendMode mode;

	luax_catchexcept(L, [&](){ mode = instance()->getBlendMode(); });

	if (!Graphics::getConstant(mode, str))
		return luaL_error(L, "Unknown blend mode");

	lua_pushstring(L, str);
	return 1;
}

int w_setDefaultFilter(lua_State *L)
{
	Texture::FilterMode min;
	Texture::FilterMode mag;

	const char *minstr = luaL_checkstring(L, 1);
	const char *magstr = luaL_optstring(L, 2, minstr);

	if (!Texture::getConstant(minstr, min))
		return luaL_error(L, "Invalid filter mode: %s", minstr);
	if (!Texture::getConstant(magstr, mag))
		return luaL_error(L, "Invalid filter mode: %s", magstr);

	float anisotropy = (float) luaL_optnumber(L, 3, 1.0);

	Texture::Filter f;
	f.min = min;
	f.mag = mag;
	f.anisotropy = anisotropy;

	instance()->setDefaultFilter(f);
	
	return 0;
}

int w_getDefaultFilter(lua_State *L)
{
	const Texture::Filter &f = instance()->getDefaultFilter();
	const char *minstr;
	const char *magstr;
	if (!Texture::getConstant(f.min, minstr))
		return luaL_error(L, "Unknown minification filter mode");
	if (!Texture::getConstant(f.mag, magstr))
		return luaL_error(L, "Unknown magnification filter mode");
	lua_pushstring(L, minstr);
	lua_pushstring(L, magstr);
	lua_pushnumber(L, f.anisotropy);
	return 3;
}

int w_setDefaultMipmapFilter(lua_State *L)
{
	Texture::FilterMode filter = Texture::FILTER_NONE;
	if (!lua_isnoneornil(L, 1))
	{
		const char *str = luaL_checkstring(L, 1);
		if (!Texture::getConstant(str, filter))
			return luaL_error(L, "Invalid filter mode: %s", str);
	}

	float sharpness = (float) luaL_optnumber(L, 2, 0);

	instance()->setDefaultMipmapFilter(filter, sharpness);

	return 0;
}

int w_getDefaultMipmapFilter(lua_State *L)
{
	Texture::FilterMode filter;
	float sharpness;

	instance()->getDefaultMipmapFilter(&filter, &sharpness);

	const char *str;
	if (Texture::getConstant(filter, str))
		lua_pushstring(L, str);
	else
		lua_pushnil(L);
	
	lua_pushnumber(L, sharpness);

	return 2;
}

int w_setLineWidth(lua_State *L)
{
	float width = (float)luaL_checknumber(L, 1);
	instance()->setLineWidth(width);
	return 0;
}

int w_setLineStyle(lua_State *L)
{
	Graphics::LineStyle style;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, style))
		return luaL_error(L, "Invalid line style: %s", str);

	instance()->setLineStyle(style);
	return 0;
}

int w_setLineJoin(lua_State *L)
{
	Graphics::LineJoin join;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, join))
		return luaL_error(L, "Invalid line join mode: %s", str);

	instance()->setLineJoin(join);
	return 0;
}

int w_getLineWidth(lua_State *L)
{
	lua_pushnumber(L, instance()->getLineWidth());
	return 1;
}

int w_getLineStyle(lua_State *L)
{
	Graphics::LineStyle style = instance()->getLineStyle();
	const char *str;
	if (!Graphics::getConstant(style, str))
		return luaL_error(L, "Unknown line style");
	lua_pushstring(L, str);
	return 1;
}

int w_getLineJoin(lua_State *L)
{
	Graphics::LineJoin join = instance()->getLineJoin();
	const char *str;
	if (!Graphics::getConstant(join, str))
		return luaL_error(L, "Unknown line join");
	lua_pushstring(L, str);
	return 1;
}

int w_setPointSize(lua_State *L)
{
	float size = (float)luaL_checknumber(L, 1);
	instance()->setPointSize(size);
	return 0;
}

int w_setPointStyle(lua_State *L)
{
	Graphics::PointStyle style;

	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, style))
		return luaL_error(L, "Invalid point style: %s", str);

	instance()->setPointStyle(style);
	return 0;
}

int w_getPointSize(lua_State *L)
{
	lua_pushnumber(L, instance()->getPointSize());
	return 1;
}

int w_getPointStyle(lua_State *L)
{
	Graphics::PointStyle style = instance()->getPointStyle();
	const char *str;
	if (!Graphics::getConstant(style, str))
		return luaL_error(L, "Unknown point style");
	lua_pushstring(L, str);
	return 1;
}

int w_getMaxPointSize(lua_State *L)
{
	lua_pushnumber(L, instance()->getSystemLimit(Graphics::LIMIT_POINT_SIZE));
	return 1;
}

int w_setWireframe(lua_State *L)
{
	instance()->setWireframe(luax_toboolean(L, 1));
	return 0;
}

int w_isWireframe(lua_State *L)
{
	luax_pushboolean(L, instance()->isWireframe());
	return 1;
}

int w_newScreenshot(lua_State *L)
{
	love::image::Image *image = luax_getmodule<love::image::Image>(L, "image", MODULE_IMAGE_T);
	bool copyAlpha = luax_optboolean(L, 1, false);
	love::image::ImageData *i = 0;

	luax_catchexcept(L, [&](){ i = instance()->newScreenshot(image, copyAlpha); });

	luax_pushtype(L, "ImageData", IMAGE_IMAGE_DATA_T, i);
	i->release();
	return 1;
}

int w_setCanvas(lua_State *L)
{
	// discard stencil testing
	instance()->discardStencil();

	// called with none -> reset to default buffer
	if (lua_isnoneornil(L, 1))
	{
		instance()->setCanvas();
		return 0;
	}

	bool is_table = lua_istable(L, 1);
	std::vector<Canvas *> canvases;

	if (is_table)
	{
		for (size_t i = 1; i <= lua_objlen(L, 1); i++)
		{
			lua_rawgeti(L, 1, i);
			canvases.push_back(luax_checkcanvas(L, -1));
			lua_pop(L, 1);
		}
	}
	else
	{
		for (int i = 1; i <= lua_gettop(L); i++)
			canvases.push_back(luax_checkcanvas(L, i));
	}

	luax_catchexcept(L, [&]() {
		if (canvases.size() > 0)
			instance()->setCanvas(canvases);
		else
			instance()->setCanvas();
	});

	return 0;
}

int w_getCanvas(lua_State *L)
{
	const std::vector<Canvas *> canvases = instance()->getCanvas();
	int n = 0;

	for (Canvas *c : canvases)
	{
		luax_pushtype(L, "Canvas", GRAPHICS_CANVAS_T, c);
		n++;
	}

	if (n == 0)
	{
		lua_pushnil(L);
		n = 1;
	}

	return n;
}

int w_setShader(lua_State *L)
{
	if (lua_isnoneornil(L,1))
	{
		instance()->setShader();
		return 0;
	}

	Shader *shader = luax_checkshader(L, 1);
	instance()->setShader(shader);
	return 0;
}

int w_getShader(lua_State *L)
{
	Shader *shader = instance()->getShader();
	if (shader)
		luax_pushtype(L, "Shader", GRAPHICS_SHADER_T, shader);
	else
		lua_pushnil(L);

	return 1;
}

int w_isSupported(lua_State *L)
{
	bool supported = true;

	for (int i = 1; i <= lua_gettop(L); i++)
	{
		const char *str = luaL_checkstring(L, i);
		Graphics::Support feature;
		if (!Graphics::getConstant(str, feature))
			return luaL_error(L, "Invalid graphics feature: %s", str);

		if (!instance()->isSupported(feature))
		{
			supported = false;
			break;
		}
	}

	luax_pushboolean(L, supported);
	return 1;
}

int w_getCanvasFormats(lua_State *L)
{
	lua_createtable(L, 0, (int) Canvas::FORMAT_MAX_ENUM);

	for (int i = 0; i < (int) Canvas::FORMAT_MAX_ENUM; i++)
	{
		Canvas::Format format = (Canvas::Format) i;
		const char *name = nullptr;

		if (!Canvas::getConstant(format, name))
			continue;

		luax_pushboolean(L, Canvas::isFormatSupported(format));
		lua_setfield(L, -2, name);
	}

	return 1;
}

int w_getCompressedImageFormats(lua_State *L)
{
	lua_createtable(L, 0, (int) image::CompressedData::FORMAT_MAX_ENUM);

	for (int i = 0; i < (int) image::CompressedData::FORMAT_MAX_ENUM; i++)
	{
		image::CompressedData::Format format = (image::CompressedData::Format) i;
		const char *name = nullptr;

		if (format == image::CompressedData::FORMAT_UNKNOWN)
			continue;

		if (!image::CompressedData::getConstant(format, name))
			continue;

		luax_pushboolean(L, Image::hasCompressedTextureSupport(format));
		lua_setfield(L, -2, name);
	}

	return 1;
}

int w_getRendererInfo(lua_State *L)
{
	Graphics::RendererInfo info;
	luax_catchexcept(L, [&](){ info = instance()->getRendererInfo(); });

	luax_pushstring(L, info.name);
	luax_pushstring(L, info.version);
	luax_pushstring(L, info.vendor);
	luax_pushstring(L, info.device);
	return 4;
}

int w_getStats(lua_State *L)
{
	Graphics::Stats stats = instance()->getStats();

	lua_createtable(L, 0, (int) Graphics::STAT_MAX_ENUM);

	const char *sname = nullptr;

	Graphics::getConstant(Graphics::STAT_DRAW_CALLS, sname);
	lua_pushinteger(L, stats.drawCalls);
	lua_setfield(L, -2, sname);

	Graphics::getConstant(Graphics::STAT_CANVAS_SWITCHES, sname);
	lua_pushinteger(L, stats.canvasSwitches);
	lua_setfield(L, -2, sname);

	Graphics::getConstant(Graphics::STAT_CANVASES, sname);
	lua_pushinteger(L, stats.canvases);
	lua_setfield(L, -2, sname);

	Graphics::getConstant(Graphics::STAT_IMAGES, sname);
	lua_pushinteger(L, stats.images);
	lua_setfield(L, -2, sname);

	Graphics::getConstant(Graphics::STAT_FONTS, sname);
	lua_pushinteger(L, stats.fonts);
	lua_setfield(L, -2, sname);

	Graphics::getConstant(Graphics::STAT_TEXTURE_MEMORY, sname);
	lua_pushnumber(L, (lua_Number) stats.textureMemory);
	lua_setfield(L, -2, sname);

	return 1;
}

int w_getSystemLimit(lua_State *L)
{
	const char *limitstr = luaL_checkstring(L, 1);
	Graphics::SystemLimit limittype;

	if (!Graphics::getConstant(limitstr, limittype))
		return luaL_error(L, "Invalid system limit type: %s", limitstr);

	lua_pushnumber(L, instance()->getSystemLimit(limittype));
	return 1;
}

int w_draw(lua_State *L)
{
	Drawable *drawable = nullptr;
	Texture *texture = nullptr;
	Quad *quad = nullptr;
	int startidx = 2;

	if (luax_istype(L, 2, GRAPHICS_QUAD_T))
	{
		texture = luax_checktexture(L, 1);
		quad = luax_totype<Quad>(L, 2, "Quad", GRAPHICS_QUAD_T);
		startidx = 3;
	}
	else if (lua_isnil(L, 2) && !lua_isnoneornil(L, 3))
	{
		return luax_typerror(L, 2, "Quad");
	}
	else
	{
		drawable = luax_checktype<Drawable>(L, 1, "Drawable", GRAPHICS_DRAWABLE_T);
		startidx = 2;
	}

	float x  = (float) luaL_optnumber(L, startidx + 0, 0.0);
	float y  = (float) luaL_optnumber(L, startidx + 1, 0.0);
	float a  = (float) luaL_optnumber(L, startidx + 2, 0.0);
	float sx = (float) luaL_optnumber(L, startidx + 3, 1.0);
	float sy = (float) luaL_optnumber(L, startidx + 4, sx);
	float ox = (float) luaL_optnumber(L, startidx + 5, 0.0);
	float oy = (float) luaL_optnumber(L, startidx + 6, 0.0);
	float kx = (float) luaL_optnumber(L, startidx + 7, 0.0);
	float ky = (float) luaL_optnumber(L, startidx + 8, 0.0);

	if (texture && quad)
		texture->drawq(quad, x, y, a, sx, sy, ox, oy, kx, ky);
	else if (drawable)
		drawable->draw(x, y, a, sx, sy, ox, oy, kx, ky);

	return 0;
}

int w_print(lua_State *L)
{
	std::string str = luax_checkstring(L, 1);
	float x = (float)luaL_optnumber(L, 2, 0.0);
	float y = (float)luaL_optnumber(L, 3, 0.0);
	float angle = (float)luaL_optnumber(L, 4, 0.0f);
	float sx = (float)luaL_optnumber(L, 5, 1.0f);
	float sy = (float)luaL_optnumber(L, 6, sx);
	float ox = (float)luaL_optnumber(L, 7, 0.0f);
	float oy = (float)luaL_optnumber(L, 8, 0.0f);
	float kx = (float)luaL_optnumber(L, 9, 0.0f);
	float ky = (float)luaL_optnumber(L, 10, 0.0f);

	luax_catchexcept(L,
		[&](){ instance()->print(str, x, y, angle, sx, sy, ox, oy, kx,ky); }
	);
	return 0;
}

int w_printf(lua_State *L)
{
	std::string str = luax_checkstring(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	float wrap = (float)luaL_checknumber(L, 4);

	float angle = 0.0f;
	float sx = 1.0f, sy = 1.0f;
	float ox = 0.0f, oy = 0.0f;
	float kx = 0.0f, ky = 0.0f;

	Graphics::AlignMode align = Graphics::ALIGN_LEFT;

	if (lua_gettop(L) >= 5)
	{
		if (!lua_isnil(L, 5))
		{
			const char *str = luaL_checkstring(L, 5);
			if (!Graphics::getConstant(str, align))
				return luaL_error(L, "Incorrect alignment: %s", str);
		}

		angle = (float) luaL_optnumber(L, 6, 0.0f);
		sx = (float) luaL_optnumber(L, 7, 1.0f);
		sy = (float) luaL_optnumber(L, 8, sx);
		ox = (float) luaL_optnumber(L, 9, 0.0f);
		oy = (float) luaL_optnumber(L, 10, 0.0f);
		kx = (float) luaL_optnumber(L, 11, 0.0f);
		ky = (float) luaL_optnumber(L, 12, 0.0f);
	}

	luax_catchexcept(L,
		[&](){ instance()->printf(str, x, y, wrap, align, angle, sx, sy, ox, oy, kx, ky); }
	);
	return 0;
}

int w_point(lua_State *L)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	instance()->point(x, y);
	return 0;
}

int w_line(lua_State *L)
{
	int args = lua_gettop(L);
	bool is_table = false;
	if (args == 1 && lua_istable(L, 1))
	{
		args = lua_objlen(L, 1);
		is_table = true;
	}

	if (args % 2 != 0)
		return luaL_error(L, "Number of vertex components must be a multiple of two");
	else if (args < 4)
		return luaL_error(L, "Need at least two vertices to draw a line");

	float *coords = new float[args];
	if (is_table)
	{
		for (int i = 0; i < args; ++i)
		{
			lua_rawgeti(L, 1, i + 1);
			coords[i] = luax_tofloat(L, -1);
			lua_pop(L, 1);
		}
	}
	else
	{
		for (int i = 0; i < args; ++i)
			coords[i] = luax_tofloat(L, i + 1);
	}

	instance()->polyline(coords, args);

	delete[] coords;
	return 0;
}

int w_rectangle(lua_State *L)
{
	Graphics::DrawMode mode;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, mode))
		return luaL_error(L, "Incorrect draw mode %s", str);

	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	float w = (float)luaL_checknumber(L, 4);
	float h = (float)luaL_checknumber(L, 5);
	instance()->rectangle(mode, x, y, w, h);
	return 0;
}

int w_circle(lua_State *L)
{
	Graphics::DrawMode mode;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, mode))
		return luaL_error(L, "Incorrect draw mode %s", str);

	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	float radius = (float)luaL_checknumber(L, 4);
	int points;
	if (lua_isnoneornil(L, 5))
		points = radius > 10 ? (int)(radius) : 10;
	else
		points = luaL_checkint(L, 5);

	instance()->circle(mode, x, y, radius, points);
	return 0;
}

int w_arc(lua_State *L)
{
	Graphics::DrawMode mode;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, mode))
		return luaL_error(L, "Incorrect draw mode %s", str);

	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	float radius = (float)luaL_checknumber(L, 4);
	float angle1 = (float)luaL_checknumber(L, 5);
	float angle2 = (float)luaL_checknumber(L, 6);
	int points;
	if (lua_isnoneornil(L, 7))
		points = radius > 10 ? (int)(radius) : 10;
	else
		points = luaL_checkint(L, 7);

	instance()->arc(mode, x, y, radius, angle1, angle2, points);
	return 0;
}

int w_polygon(lua_State *L)
{
	int args = lua_gettop(L) - 1;

	Graphics::DrawMode mode;
	const char *str = luaL_checkstring(L, 1);
	if (!Graphics::getConstant(str, mode))
		return luaL_error(L, "Invalid draw mode: %s", str);

	bool is_table = false;
	float *coords;
	if (args == 1 && lua_istable(L, 2))
	{
		args = lua_objlen(L, 2);
		is_table = true;
	}

	if (args % 2 != 0)
		return luaL_error(L, "Number of vertex components must be a multiple of two");
	else if (args < 6)
		return luaL_error(L, "Need at least three vertices to draw a polygon");

	// fetch coords
	coords = new float[args + 2];
	if (is_table)
	{
		for (int i = 0; i < args; ++i)
		{
			lua_rawgeti(L, 2, i + 1);
			coords[i] = luax_tofloat(L, -1);
			lua_pop(L, 1);
		}
	}
	else
	{
		for (int i = 0; i < args; ++i)
			coords[i] = luax_tofloat(L, i + 2);
	}

	// make a closed loop
	coords[args]   = coords[0];
	coords[args+1] = coords[1];
	instance()->polygon(mode, coords, args+2);
	delete[] coords;

	return 0;
}

int w_push(lua_State *L)
{
	Graphics::StackType stype = Graphics::STACK_TRANSFORM;
	const char *sname = lua_isnoneornil(L, 1) ? nullptr : luaL_checkstring(L, 1);
	if (sname && !Graphics::getConstant(sname, stype))
		return luaL_error(L, "Invalid graphics stack type: %s", sname);

	luax_catchexcept(L, [&](){ instance()->push(stype); });
	return 0;
}

int w_pop(lua_State *L)
{
	luax_catchexcept(L, [&](){ instance()->pop(); });
	return 0;
}

int w_rotate(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	instance()->rotate(angle);
	return 0;
}

int w_scale(lua_State *L)
{
	float sx = (float)luaL_optnumber(L, 1, 1.0f);
	float sy = (float)luaL_optnumber(L, 2, sx);
	instance()->scale(sx, sy);
	return 0;
}

int w_translate(lua_State *L)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	instance()->translate(x, y);
	return 0;
}

int w_shear(lua_State *L)
{
	float kx = (float)luaL_checknumber(L, 1);
	float ky = (float)luaL_checknumber(L, 2);
	instance()->shear(kx, ky);
	return 0;
}

int w_origin(lua_State * /*L*/)
{
	instance()->origin();
	return 0;
}


// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "reset", w_reset },
	{ "clear", w_clear },
	{ "present", w_present },

	{ "newImage", w_newImage },
	{ "newQuad", w_newQuad },
	{ "newFont", w_newFont },
	{ "newImageFont", w_newImageFont },
	{ "newSpriteBatch", w_newSpriteBatch },
	{ "newParticleSystem", w_newParticleSystem },
	{ "newCanvas", w_newCanvas },
	{ "newShader", w_newShader },
	{ "newMesh", w_newMesh },

	{ "setColor", w_setColor },
	{ "getColor", w_getColor },
	{ "setBackgroundColor", w_setBackgroundColor },
	{ "getBackgroundColor", w_getBackgroundColor },

	{ "setNewFont", w_setNewFont },
	{ "setFont", w_setFont },
	{ "getFont", w_getFont },

	{ "setColorMask", w_setColorMask },
	{ "getColorMask", w_getColorMask },
	{ "setBlendMode", w_setBlendMode },
	{ "getBlendMode", w_getBlendMode },
	{ "setDefaultFilter", w_setDefaultFilter },
	{ "getDefaultFilter", w_getDefaultFilter },
	{ "setDefaultMipmapFilter", w_setDefaultMipmapFilter },
	{ "getDefaultMipmapFilter", w_getDefaultMipmapFilter },
	{ "setLineWidth", w_setLineWidth },
	{ "setLineStyle", w_setLineStyle },
	{ "setLineJoin", w_setLineJoin },
	{ "getLineWidth", w_getLineWidth },
	{ "getLineStyle", w_getLineStyle },
	{ "getLineJoin", w_getLineJoin },
	{ "setPointSize", w_setPointSize },
	{ "setPointStyle", w_setPointStyle },
	{ "getPointSize", w_getPointSize },
	{ "getPointStyle", w_getPointStyle },
	{ "setWireframe", w_setWireframe },
	{ "isWireframe", w_isWireframe },
	{ "newScreenshot", w_newScreenshot },
	{ "setCanvas", w_setCanvas },
	{ "getCanvas", w_getCanvas },

	{ "setShader", w_setShader },
	{ "getShader", w_getShader },

	{ "isSupported", w_isSupported },
	{ "getCanvasFormats", w_getCanvasFormats },
	{ "getCompressedImageFormats", w_getCompressedImageFormats },
	{ "getRendererInfo", w_getRendererInfo },
	{ "getStats", w_getStats },
	{ "getSystemLimit", w_getSystemLimit },

	{ "draw", w_draw },

	{ "print", w_print },
	{ "printf", w_printf },

	{ "isCreated", w_isCreated },
	{ "getWidth", w_getWidth },
	{ "getHeight", w_getHeight },
	{ "getDimensions", w_getDimensions },

	{ "setScissor", w_setScissor },
	{ "getScissor", w_getScissor },

	{ "setStencil", w_setStencil },
	{ "setInvertedStencil", w_setInvertedStencil },

	{ "point", w_point },
	{ "line", w_line },
	{ "rectangle", w_rectangle },
	{ "circle", w_circle },
	{ "arc", w_arc },

	{ "polygon", w_polygon },

	{ "push", w_push },
	{ "pop", w_pop },
	{ "rotate", w_rotate },
	{ "scale", w_scale },
	{ "translate", w_translate },
	{ "shear", w_shear },
	{ "origin", w_origin },

	// Deprecated since 0.9.1.
	{ "getMaxImageSize", w_getMaxTextureSize },
	{ "getMaxPointSize", w_getMaxPointSize },

	{ 0, 0 }
};

// Types for this module.
static const lua_CFunction types[] =
{
	luaopen_font,
	luaopen_image,
	luaopen_quad,
	luaopen_spritebatch,
	luaopen_particlesystem,
	luaopen_canvas,
	luaopen_shader,
	luaopen_mesh,
	0
};

extern "C" int luaopen_love_graphics(lua_State *L)
{
	Graphics *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new Graphics(); });
	}
	else
		instance->retain();

	WrappedModule w;
	w.module = instance;
	w.name = "graphics";
	w.flags = MODULE_GRAPHICS_T;
	w.functions = functions;
	w.types = types;

	int n = luax_register_module(L, w);

	if (luaL_loadbuffer(L, (const char *)graphics_lua, sizeof(graphics_lua), "love_graphics.lua") == 0)
		lua_call(L, 0, 0);

	return n;
}

} // opengl
} // graphics
} // love
