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
 * Since    : 2013-4-23
 * Update   : 2019-1-24
 * Author   : scott.cgi
 */


#ifndef SHADER_SPRITE_H
#define SHADER_SPRITE_H


#include "MojoC_gl3.h"
#include "MojoC_Matrix.h"
#include "MojoC_Math.h"
#include "MojoC_Color.h"


/**
 * Shader control for Sprite.
 */
struct AShaderSprite
{
    GLuint program;
    GLint  attribPositionTexcoord;
    GLint  uniformSampler2D;
    GLint  uniformMVPMatrix;
    GLint  uniformColor;

    void (*Use) (Matrix4* mvpMatrix, Color* color);
    void (*Init)(void);
};


extern struct AShaderSprite AShaderSprite[1];


#endif
