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
 * Since    : 2014-2-26
 * Update   : 2019-1-24
 * Author   : scott.cgi
 */


#ifndef GL_PRIMITIVE_H
#define GL_PRIMITIVE_H


#include "MojoC_gl3.h"
#include "MojoC_Matrix.h"
#include "MojoC_Math.h"
#include "MojoC_Rect.h"
#include "MojoC_Color.h"
#include "MojoC_Array.h"


/**
 * Draw something without texture.
 * can render with Drawable's Render function.
 */
struct AGLPrimitive
{
    /**
     * Render array of points, the point is pair of x, y.
     */
    void (*RenderPoints) (Array(float)* pointArr,  Matrix4* mvpMatrix, Color* color, float pointSize);

    /**
     * Render array of vertices, the vertex is pair of x, y.
     */
    void (*RenderPolygon)(Array(float)* vertexArr, Matrix4* mvpMatrix, Color* color, float lineWidth);

    /**
     * Render array of lines, the line is two pairs of x, y.
     */
    void (*RenderLines)  (Array(float)* lineArr,   Matrix4* mvpMatrix, Color* color, float lineWidth);

    /**
     * Render rect.
     */
    void (*RenderRect)   (Rect*         rect,      Matrix4* mvpMatrix, Color* color, float lineWidth);
};


extern struct AGLPrimitive AGLPrimitive[1];


#endif
