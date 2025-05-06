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
 * Since    : 2013-9-8
 * Update   : 2019-1-19
 * Author   : scott.cgi
 */


#include "MojoC_Graphics.h"
#include "MojoC_ShaderMesh.h"
#include "MojoC_ShaderSprite.h"
#include "MojoC_ShaderPrimitive.h"
#include "MojoC_GLInfo.h"


static void Init()
{
    AGLInfo         ->Init();
    AShaderMesh     ->Init();
    AShaderSprite   ->Init();
    AShaderPrimitive->Init();

    AGraphics->isUseVBO       = false;
    AGraphics->isUseVAO       = false;
    AGraphics->isUseMapBuffer = false;
}


static void SetUseVAO(bool isUseVAO)
{
    if (isUseVAO && AGLInfo->version > 2.0f)
    {
        AGraphics->isUseVAO = true;
        AGraphics->isUseVBO = true;
    }
    else
    {
        AGraphics->isUseVAO = false;
        AGraphics->isUseVBO = false;
    }
}


static void SetUseMapBuffer(bool isUseMapBuffer)
{
    if (isUseMapBuffer && AGLInfo->version > 2.0f)
    {
        AGraphics->isUseMapBuffer = true;
    }
    else
    {
        AGraphics->isUseMapBuffer = false;
    }
}


struct AGraphics AGraphics[1] =
{{
    false,
    false,
    false,
    Init,
    SetUseVAO,
    SetUseMapBuffer,
}};
