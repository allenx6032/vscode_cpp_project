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

#ifndef LOVE_GRAPHICS_OPENGL_IMAGE_H
#define LOVE_GRAPHICS_OPENGL_IMAGE_H

// LOVE
#include "love_config.h"
#include "love_Matrix.h"
#include "love_Vector.h"
#include "love_StringMap.h"
#include "love_math.h"
#include "love_ImageData.h"
#include "love_CompressedData.h"
#include "love_opengl_Texture.h"

// OpenGL
#include "love_opengl_OpenGL.h"

namespace love
{
namespace graphics
{
namespace opengl
{

/**
 * A drawable image based on OpenGL-textures. This class takes ImageData
 * objects and create textures on the GPU for fast drawing.
 *
 * @author Anders Ruud
 **/
class Image : public Texture
{
public:

	enum Format
	{
		FORMAT_NORMAL,
		FORMAT_SRGB,
		FORMAT_MAX_ENUM
	};

	/**
	 * Creates a new Image. Not that anything is ready to use
	 * before load is called.
	 *
	 * @param data The data from which to load the image.
	 **/
	Image(love::image::ImageData *data, Format format = FORMAT_NORMAL);

	/**
	 * Creates a new Image with compressed image data.
	 *
	 * @param cdata The compressed data from which to load the image.
	 **/
	Image(love::image::CompressedData *cdata, Format format = FORMAT_NORMAL);

	/**
	 * Destructor. Deletes the hardware texture and other resources.
	 **/
	virtual ~Image();

	love::image::ImageData *getImageData() const;
	love::image::CompressedData *getCompressedData() const;

	/**
	 * @copydoc Drawable::draw()
	 **/
	void draw(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky);

	/**
	 * @copydoc Texture::drawq()
	 **/
	void drawq(Quad *quad, float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky);

	/**
	 * Call before using this Image's texture to draw. Binds the texture,
	 * globally scales texture coordinates if the Image has NPOT dimensions and
	 * NPOT isn't supported, etc.
	 **/
	virtual void predraw();
	virtual void postdraw();

	virtual GLuint getGLTexture() const;

	virtual void setFilter(const Texture::Filter &f);
	virtual void setWrap(const Texture::Wrap &w);

	void setMipmapSharpness(float sharpness);
	float getMipmapSharpness() const;

	/**
	 * Whether this Image is using a compressed texture (via CompressedData).
	 **/
	bool isCompressed() const;

	void bind() const;

	bool load();
	void unload();

	// Implements Volatile.
	bool loadVolatile();
	void unloadVolatile();

	/**
	 * Re-uploads the ImageData or CompressedData associated with this Image to
	 * the GPU, allowing situations where lovers modify an ImageData after image
	 * creation from the ImageData, and apply the changes with Image:refresh().
	 **/
	bool refresh();

	Format getFormat() const;

	static void setDefaultMipmapSharpness(float sharpness);
	static float getDefaultMipmapSharpness();
	static void setDefaultMipmapFilter(FilterMode f);
	static FilterMode getDefaultMipmapFilter();

	static bool hasNpot();
	static bool hasAnisotropicFilteringSupport();
	static bool hasMipmapSupport();
	static bool hasMipmapSharpnessSupport();

	static bool hasCompressedTextureSupport();
	static bool hasCompressedTextureSupport(image::CompressedData::Format format);

	static bool hasSRGBSupport();

	static bool getConstant(const char *in, Format &out);
	static bool getConstant(Format in, const char *&out);

	static int imageCount;

private:

	void uploadDefaultTexture();

	void drawv(const Matrix &t, const Vertex *v);

	// The ImageData from which the texture is created. May be null if
	// Compressed image data was used to create the texture.
	StrongRef<love::image::ImageData> data;

	// Or the Compressed Image Data from which the texture is created. May be
	// null if raw ImageData was used to create the texture.
	StrongRef<love::image::CompressedData> cdata;

	// Real dimensions of the texture, if it was auto-padded to POT size.
	int paddedWidth, paddedHeight;

	// OpenGL texture identifier.
	GLuint texture;

	// Mipmap texture LOD bias (sharpness) value.
	float mipmapSharpness;

	// True if mipmaps have been created for this Image.
	bool mipmapsCreated;

	// Whether this Image is using a compressed texture.
	bool compressed;

	// The format to interpret the image's data as.
	Format format;

	// True if the image wasn't able to be properly created and it had to fall
	// back to a default texture.
	bool usingDefaultTexture;

	size_t textureMemorySize;

	void preload();

	void uploadTexturePadded();
	void uploadTexture();

	void uploadCompressedMipmaps();
	void createMipmaps();
	void checkMipmapsCreated();

	static float maxMipmapSharpness;

	static FilterMode defaultMipmapFilter;
	static float defaultMipmapSharpness;

	GLenum getCompressedFormat(image::CompressedData::Format cformat) const;

	static StringMap<Format, FORMAT_MAX_ENUM>::Entry formatEntries[];
	static StringMap<Format, FORMAT_MAX_ENUM> formats;

}; // Image

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_OPENGL_IMAGE_H
