/*
 * Copyright (c) scott.cgi All Rights Reserved.
 *
 * This source code belongs to project Mojoc, which is a pure C Game Engine hosted on GitHub.
 * The Mojoc Game Engine is licensed under the MIT License, and will continue to be iterated with coding passion.
 *
 * License  : https://github.com/scottcgi/Mojoc/blob/master/LICENSE
 * GitHub   : https://github.com/scottcgi/Mojoc
 * CodeStyle: https://github.com/scottcgi/Mojoc/blob/master/Docs/CodeStyle.md
 *
 * Since    : 2016-7-27
 * Update   : 2021-2-8
 * Author   : scott.cgi
 */


#ifndef FONT_H
#define FONT_H


#include "MojoC_ArrayIntSet.h"
#include "MojoC_TextureAtlas.h"
#include "MojoC_Mesh.h"


/**
 * Font implementation by TextureAtlas.
 */
typedef struct
{
    /**
     * the TextureAtlas that Font used.
     */
    TextureAtlas*          textureAtlas;

    /**
     * The Mesh that textureAtlas used.
     */
    Mesh                   mesh[1];

    /**
     * Cache all FontTexts.
     */
    ArrayIntSet(FontText*) fontTextSet[1];

    /**
     * Font Mesh unused SubMesh list.
     * released FontText's SubMesh will add into this.
     */
    ArrayList  (SubMesh*)  unusedSubMeshList[1];
}
Font;


/**
 * FontText alignment type.
 */
typedef enum
{
    FontTextAlignment_HorizontalLeft,
    FontTextAlignment_HorizontalRight,
    FontTextAlignment_VerticalTop,
    FontTextAlignment_VerticalBottom,
}
FontTextAlignment;


/**
 * The text in font textureAtlas.
 */
typedef struct
{
    /**
     * The base class for provide draw functions.
     */
    Drawable            drawable[1];

    /**
     * Default FontTextAlignment_HorizontalLeft.
     */
    FontTextAlignment   alignment;

    /**
     * The spacing between chars, default 0.
     */
    float               charSpacing;

    /**
     * The Font that FontText belongs to.
     */
    Font*               font;

    /**
     * The SubMesh list that FontText used.
     */
    ArrayList(SubMesh*) usedSubMeshList[1];
}
FontText;


/**
 * Control Font and FontText.
 */
struct AFont
{
    /**
     * Get Font by filePath.
     *
     * filePath:
     *     Android: assets
     *     IOS    : NSBundle
     */
    Font*     (*Get)           (const char* filePath);
    FontText* (*GetText)       (Font* font);
    void      (*Draw)          (Font* font);

    void      (*SetString)     (FontText* text, const char* str);
    void      (*SetInt)        (FontText* text, int         num);
    void      (*SetFloat)      (FontText* text, float       num);

    /**
     * Make Font can reuse in Get method,
     * and release all FontTexts.
     */
    void      (*Release)       (Font* font);

    /**
     * Make FontText can reuse in GetText method.
     * and add FontText's SubMesh into Font->unusedSubMeshList.
     */
    void      (*ReleaseText)   (FontText* text);
};


extern struct AFont AFont[1];


/**
 * Set FontText color.
 */
static inline void AFont_SetTextColor(FontText* text, Color* color)
{
    ADrawable_SetColor(text->drawable, color);
}


#endif
