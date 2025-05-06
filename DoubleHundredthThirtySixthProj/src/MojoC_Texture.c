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
 * Since    : 2013-1-2
 * Update   : 2019-1-19
 * Author   : scott.cgi
 */


#include <stdlib.h>
#include "MojoC_Texture.h"
#include "MojoC_GLTool.h"
#include "MojoC_ArrayStrMap.h"
#include "MojoC_Log.h"


static ArrayStrMap(filePath, Texture*) textureCacheMap[1] = AArrayStrMap_Init(Texture*, 25);


static Texture* Get(const char* resourceFilePath)
{
    Texture* texture = AArrayStrMap_Get(textureCacheMap, resourceFilePath, Texture*);

    if (texture == NULL)
    {
        texture = malloc(sizeof(Texture));
        AGLTool->LoadTexture(resourceFilePath, texture);

        texture->filePath = AArrayStrMap_GetKey
                           (
                               AArrayStrMap_TryPut(textureCacheMap, resourceFilePath, texture),
                               textureCacheMap->valueTypeSize
                           );
    }

    return texture;
}


static void Destroy(Texture* texture)
{
    glDeleteTextures(1, (GLuint[1]) {texture->id});

    bool isRemoved = AArrayStrMap->TryRemove(textureCacheMap, texture->filePath);
    ALog_A(isRemoved, "ATexture Destroy not found %s in textureCacheMap", texture->filePath);

    free(texture);
}


struct ATexture ATexture[1] =
{{
    Get,
    Destroy,
}};

