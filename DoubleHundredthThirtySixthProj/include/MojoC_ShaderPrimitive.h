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


#ifndef SHADER_PRIMITIVE_H
#define SHADER_PRIMITIVE_H


#include "MojoC_gl3.h"
#include "MojoC_Matrix.h"
#include "MojoC_Math.h"
#include "MojoC_Color.h"


/**
 * Shader control for GLPrimitive.
 */
struct AShaderPrimitive
{
    GLuint program;
    GLint  attribPosition;
    GLint  uniformMVPMatrix;
    GLint  uniformColor;
    GLint  uniformPointSize;

    void (*Use) (Matrix4* mvpMatrix, Color* color, GLfloat pointSize);
    void (*Init)(void);
};


extern struct AShaderPrimitive AShaderPrimitive[1];


#endif
