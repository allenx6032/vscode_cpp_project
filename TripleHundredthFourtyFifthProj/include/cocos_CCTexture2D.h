/****************************************************************************
Copyright (c) 2008      Apple Inc. All Rights Reserved.
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCTEXTURE2D_H__
#define __CCTEXTURE2D_H__

#include <string>
#include <map>

#include "cocos_CCRef.h"
#include "cocos_CCGeometry.h"
#include "cocos_ccTypes.h"

NS_CC_BEGIN

class Image;
typedef struct _MipmapInfo MipmapInfo;

/**
 * @addtogroup textures
 * @{
 */

//CONSTANTS:

class GLProgram;

//CLASS INTERFACES:

/** @brief Texture2D class.
* This class allows to easily create OpenGL 2D textures from images, text or raw data.
* The created Texture2D object will always have power-of-two dimensions.
* Depending on how you create the Texture2D object, the actual image area of the texture might be smaller than the texture dimensions i.e. "contentSize" != (pixelsWide, pixelsHigh) and (maxS, maxT) != (1.0, 1.0).
* Be aware that the content of the generated textures will be upside-down!
*/
class CC_DLL Texture2D : public Ref
{
public:
    /** @typedef Texture2D::PixelFormat
     Possible texture pixel formats
     */
    enum class PixelFormat
    {
        //! auto detect the type
        AUTO,
        //! 32-bit texture: BGRA8888
        BGRA8888,
        //! 32-bit texture: RGBA8888
        RGBA8888,
        //! 24-bit texture: RGBA888
        RGB888,
        //! 16-bit texture without Alpha channel
        RGB565,
        //! 8-bit textures used as masks
        A8,
        //! 8-bit intensity texture
        I8,
        //! 16-bit textures used as masks
        AI88,
        //! 16-bit textures: RGBA4444
        RGBA4444,
        //! 16-bit textures: RGB5A1
        RGB5A1,
        //! 4-bit PVRTC-compressed texture: PVRTC4
        PVRTC4,
        //! 4-bit PVRTC-compressed texture: PVRTC4 (has alpha channel)
        PVRTC4A,
        //! 2-bit PVRTC-compressed texture: PVRTC2
        PVRTC2,
        //! 2-bit PVRTC-compressed texture: PVRTC2 (has alpha channel)
        PVRTC2A,
        //! ETC-compressed texture: ETC
        ETC,
        //! S3TC-compressed texture: S3TC_Dxt1
        S3TC_DXT1,
        //! S3TC-compressed texture: S3TC_Dxt3
        S3TC_DXT3,
        //! S3TC-compressed texture: S3TC_Dxt5
        S3TC_DXT5,
        //! ATITC-compressed texture: ATC_RGB
        ATC_RGB,
        //! ATITC-compressed texture: ATC_EXPLICIT_ALPHA
        ATC_EXPLICIT_ALPHA,
        //! ATITC-compresed texture: ATC_INTERPOLATED_ALPHA
        ATC_INTERPOLATED_ALPHA,
        //! Default texture format: AUTO
        DEFAULT = AUTO,
        
        NONE = -1
    };
    
    
    struct PixelFormatInfo {

        PixelFormatInfo(GLenum anInternalFormat, GLenum aFormat, GLenum aType, int aBpp, bool aCompressed, bool anAlpha)
            : internalFormat(anInternalFormat)
            , format(aFormat)
            , type(aType)
            , bpp(aBpp)
            , compressed(aCompressed)
            , alpha(anAlpha)
        {}

        GLenum internalFormat;
        GLenum format;
        GLenum type;
        int bpp;
        bool compressed;
        bool alpha;
    };
    
    typedef std::map<Texture2D::PixelFormat, const PixelFormatInfo> PixelFormatInfoMap;
    
    /**
     Extension to set the Min / Mag filter
     */
    typedef struct _TexParams {
        GLuint    minFilter;
        GLuint    magFilter;
        GLuint    wrapS;
        GLuint    wrapT;
    }TexParams;
    
public:
    /** sets the default pixel format for UIImagescontains alpha channel.
     If the UIImage contains alpha channel, then the options are:
     - generate 32-bit textures: Texture2D::PixelFormat::RGBA8888 (default one)
     - generate 24-bit textures: Texture2D::PixelFormat::RGB888
     - generate 16-bit textures: Texture2D::PixelFormat::RGBA4444
     - generate 16-bit textures: Texture2D::PixelFormat::RGB5A1
     - generate 16-bit textures: Texture2D::PixelFormat::RGB565
     - generate 8-bit textures: Texture2D::PixelFormat::A8 (only use it if you use just 1 color)

     How does it work ?
     - If the image is an RGBA (with Alpha) then the default pixel format will be used (it can be a 8-bit, 16-bit or 32-bit texture)
     - If the image is an RGB (without Alpha) then: If the default pixel format is RGBA8888 then a RGBA8888 (32-bit) will be used. Otherwise a RGB565 (16-bit texture) will be used.

     This parameter is not valid for PVR / PVR.CCZ images.

     @since v0.8
     */
    static void setDefaultAlphaPixelFormat(Texture2D::PixelFormat format);

    /** returns the alpha pixel format
     @since v0.8
     */
    static Texture2D::PixelFormat getDefaultAlphaPixelFormat();
    
public:
    /**
     * @js ctor
     */
    Texture2D();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~Texture2D();
    /**
     * @js NA
     * @lua NA
     */
    virtual std::string getDescription() const;

	/** release only the gl texture.
     * @js NA
     * @lua NA
     */
	void releaseGLTexture();

    /** Initializes with a texture2d with data 
     * @js NA
     * @lua NA
     */
    bool initWithData(const void *data, ssize_t dataLen, Texture2D::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh, const Size& contentSize);

    /** Initializes with mipmaps */
    bool initWithMipmaps(MipmapInfo* mipmaps, int mipmapsNum, Texture2D::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh);

    /** Update with texture data*/
    bool updateWithData(const void *data,int offsetX,int offsetY,int width,int height);
    /**
    Drawing extensions to make it easy to draw basic quads using a Texture2D object.
    These functions require GL_TEXTURE_2D and both GL_VERTEX_ARRAY and GL_TEXTURE_COORD_ARRAY client states to be enabled.
    */
    /** draws a texture at a given point */
    void drawAtPoint(const Vec2& point);
    /** draws a texture inside a rect */
    void drawInRect(const Rect& rect);

    /**
    Extensions to make it easy to create a Texture2D object from an image file.
    */
    /** 
	Initializes a texture from a UIImage object.
    We will use the format you specified with setDefaultAlphaPixelFormat to convert the image for texture.
    NOTE: It will not convert the pvr image file.
	*/
    bool initWithImage(Image * image);
    
    /** 
	Initializes a texture from a UIImage object.
	we will use the format you passed to the function to convert the image format to the texture format.
    If you pass PixelFormat::Automatic, we will auto detect the image render type and use that type for texture to render.
    **/
    bool initWithImage(Image * image, PixelFormat format);

    /** Initializes a texture from a string with dimensions, alignment, font name and font size */
    bool initWithString(const char *text,  const std::string &fontName, float fontSize, const Size& dimensions = Size(0, 0), TextHAlignment hAlignment = TextHAlignment::CENTER, TextVAlignment vAlignment = TextVAlignment::TOP);
    /** Initializes a texture from a string using a text definition*/
    bool initWithString(const char *text, const FontDefinition& textDefinition);

    /** sets the min filter, mag filter, wrap s and wrap t texture parameters.
    If the texture size is NPOT (non power of 2), then in can only use GL_CLAMP_TO_EDGE in GL_TEXTURE_WRAP_{S,T}.

    @warning Calling this method could allocate additional texture memory.

    @since v0.8
    * @code 
    * When this function bound into js or lua,the input parameter will be changed
    * In js: var setBlendFunc(var arg1, var arg2, var arg3, var arg4)
    * In lua: local setBlendFunc(local arg1, local arg2, local arg3, local arg4)
    * @endcode
    */
    void setTexParameters(const TexParams& texParams);

    /** sets antialias texture parameters:
    - GL_TEXTURE_MIN_FILTER = GL_LINEAR
    - GL_TEXTURE_MAG_FILTER = GL_LINEAR

    @warning Calling this method could allocate additional texture memory.

    @since v0.8
    */
    void setAntiAliasTexParameters();

    /** sets alias texture parameters:
    - GL_TEXTURE_MIN_FILTER = GL_NEAREST
    - GL_TEXTURE_MAG_FILTER = GL_NEAREST

    @warning Calling this method could allocate additional texture memory.

    @since v0.8
    */
    void setAliasTexParameters();


    /** Generates mipmap images for the texture.
    It only works if the texture size is POT (power of 2).
    @since v0.99.0
    */
    void generateMipmap();

    /** returns the pixel format.
     @since v2.0
     */
    const char* getStringForFormat() const;

    /** returns the bits-per-pixel of the in-memory OpenGL texture
    @since v1.0
    */
    unsigned int getBitsPerPixelForFormat() const;

    /** Helper functions that returns bits per pixels for a given format.
     @since v2.0
     */
    unsigned int getBitsPerPixelForFormat(Texture2D::PixelFormat format) const;

    /** content size */
    const Size& getContentSizeInPixels();

    bool hasPremultipliedAlpha() const;
    bool hasMipmaps() const;

    /** Gets the pixel format of the texture */
    Texture2D::PixelFormat getPixelFormat() const;
    
    /** Gets the width of the texture in pixels */
    int getPixelsWide() const;
    
    /** Gets the height of the texture in pixels */
    int getPixelsHigh() const;
    
    /** Gets the texture name */
    GLuint getName() const;
    
    /** Gets max S */
    GLfloat getMaxS() const;
    /** Sets max S */
    void setMaxS(GLfloat maxS);
    
    /** Gets max T */
    GLfloat getMaxT() const;
    /** Sets max T */
    void setMaxT(GLfloat maxT);
    
    Size getContentSize() const;
    
    void setGLProgram(GLProgram* program);
    GLProgram* getGLProgram() const;
    
public:
    static const PixelFormatInfoMap& getPixelFormatInfoMap();
    
private:

    /**convert functions*/

    /**
    Convert the format to the format param you specified, if the format is PixelFormat::Automatic, it will detect it automatically and convert to the closest format for you.
    It will return the converted format to you. if the outData != data, you must delete it manually.
    */
    static PixelFormat convertDataToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);

    static PixelFormat convertI8ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
    static PixelFormat convertAI88ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
    static PixelFormat convertRGB888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
    static PixelFormat convertRGBA8888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);

    //I8 to XXX
    static void convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //AI88 to XXX
    static void convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //RGB888 to XXX
    static void convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //RGBA8888 to XXX
    static void convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
    static void convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

protected:
    /** pixel format of the texture */
    Texture2D::PixelFormat _pixelFormat;

    /** width in pixels */
    int _pixelsWide;

    /** height in pixels */
    int _pixelsHigh;

    /** texture name */
    GLuint _name;

    /** texture max S */
    GLfloat _maxS;
    
    /** texture max T */
    GLfloat _maxT;

    /** content size */
    Size _contentSize;

    /** whether or not the texture has their Alpha premultiplied */
    bool _hasPremultipliedAlpha;

    bool _hasMipmaps;

    /** shader program used by drawAtPoint and drawInRect */
    GLProgram* _shaderProgram;

    static const PixelFormatInfoMap _pixelFormatInfoTables;

    bool _antialiasEnabled;
};


// end of textures group
/// @}

NS_CC_END

#endif //__CCTEXTURE2D_H__

