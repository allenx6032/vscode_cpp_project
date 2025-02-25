#ifndef IMAGE_H
#define IMAGE_H

#include "e2d_widget.h"
#include "e2d_color.h"
#include "e2d_texture2d.h"

namespace Easy2D
{
    class TextureComponent;
    class UIImage : public UIWidget
    {
    public:
        UIImage(const String& name);

        virtual ~UIImage();

        virtual bool setup();

        void setColor(const Color& color);

        const Color& getColor() const;

        bool loadTexture(const String& texPath);

        void setTexture(SPtr<Texture2D> mTexture);

        bool isScale9Tile() const;

        void disableScale9Tile();

        const Vec4& getScale9Tile() const;

        void setScale9Tile(const Vec4& tiles);

        void setScale9Tile(const float32 beginX, float32 beginY, float32 endX, float32 endY);

        void setUVCoords(const Vec4& coords);

        void setUVCoords(float32 beginX, float32 beginY, float32 endX, float32 endY);

    protected:
        SPtr<TextureComponent> mTextureComponent;
    };
}

#endif
