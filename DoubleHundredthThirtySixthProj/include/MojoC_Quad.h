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
 * Since    : 2012-12-25
 * Update   : 2019-1-23
 * Author   : scott.cgi
 */


#ifndef QUAD_H
#define QUAD_H


#include "MojoC_Array.h"
#include "MojoC_Texture.h"


enum
{
    /**
     * One quad has 4 vertices
     */
    Quad_VertexNum       = 4,

    /**
     * Two triangles (index order: 012, 230) has 6 vertices (index 2,0 repeats).
     */
    Quad_IndexNum        = 6,
    Quad_IndexSize       = Quad_IndexNum       * sizeof(short),

    /**
     *  3 (x, y, z) * 4 (vertices)
     */
    Quad_Position3Num    = 12,
    Quad_Position3Size   = Quad_Position3Num   * sizeof(float),

    /**
     * 2 (u, v) * 4 (vertices)
     */
    Quad_UVNum           = 8,
    Quad_UVSize          = Quad_UVNum          * sizeof(float),

    /**
     * 1 * 4 (vertices)
     */
    Quad_OpacityNum      = 4,
    Quad_OpacitySize     = Quad_OpacityNum     * sizeof(float),

    /**
     * 3 (r, g ,b) * 4 (vertices)
     */
    Quad_RGBNum          = 12,
    Quad_RGBSize         = Quad_RGBNum         * sizeof(float),

    /**
     * 2 (x, y) * 4 (vertices)
     */
    Quad_Position2Num    = 8,
    Quad_Position2UVNum  = Quad_Position2Num   + Quad_UVNum,
    Quad_Position2UVSize = Quad_Position2UVNum * sizeof(float),
};


/**
 * Rect in texture.
 */
typedef struct
{
    /**
     * The width in openGL coordinate.
     */
    float width;

    /**
     * The height in openGL coordinate.
     */
    float height;

    /**
     * The top left x of quad in texture, openGL coordinate, default 0.
     */
    float offsetTextureX;

    /**
     * The top left y of quad in texture, openGL coordinate, default 0.
     */
    float offsetTextureY;

    /**
     * The top left x offset the center.
     * default Quad center born in center of openGL coordinate,
     * so default value is (-width / 2).
     */
    float offsetCenterX;
    
    /**
     * The top left y offset the center.
     * default Quad center born in center of openGL coordinate,
     * so default value is (height / 2)
     */
    float offsetCenterY;
}
Quad;


/**
 * Control Quad.
 */
struct AQuad
{
    Quad* (*Create)       (float width, float height);
    void  (*Init)         (float width, float height, Quad* outQuad);

    /**
     * Get max size of all quads in array.
     */
    void (*GetMaxSize)    (Array(Quad)* quadArr, float* outWidth, float* outHeight);

    /**
     * Get Quad 4 vertices position and uv data into vertexData.
     */
    void (*GetPosition2UV)(Quad* quad, Texture* texture, float outPosition2UVData[Quad_Position2UVNum]);

    /**
     * Get Quad 4 vertices position data into bornPositionData.
     */
    void (*GetPosition3)  (Quad* quad, float outBornPositionData[Quad_Position3Num]);

    /**
     * Get Quad 4 vertices uv data into uvData.
     */
    void (*GetUV)         (Quad* quad, Texture* texture, float outUVData[Quad_UVNum]);

    /**
     * Get Quad 4 vertices index data into indexData.
     */
    void (*GetIndex)      (int vertexNumBefore, short outIndexData[Quad_IndexNum]);
};


extern struct AQuad AQuad[1];


#endif
