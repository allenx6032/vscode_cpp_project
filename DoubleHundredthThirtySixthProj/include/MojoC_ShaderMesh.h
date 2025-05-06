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
 * Since    : 2016-8-13
 * Update   : 2019-1-24
 * Author   : scott.cgi
 */


#ifndef SHADER_MESH_H
#define SHADER_MESH_H


#include "MojoC_gl3.h"
#include "MojoC_Matrix.h"
#include "MojoC_Math.h"


/**
 * Shader control for Mesh.
 */
struct AShaderMesh
{
    GLuint program;
    GLint  attribPosition;
    GLint  attribTexcoord;
    GLint  attribOpacity;
    GLint  attribRGB;

    GLint  uniformMVPMatrix;
    GLint  uniformSample2D;

    void (*Use) (Matrix4* mvpMatrix);
    void (*Init)(void);
};


extern struct AShaderMesh AShaderMesh[1];


#endif
