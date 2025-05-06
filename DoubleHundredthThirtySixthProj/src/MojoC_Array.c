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
 * Since    : 2013-3-11
 * Update   : 2019-1-17
 * Author   : scott.cgi
 */


#include <stdlib.h>
#include "MojoC_Array.h"


static Array* Create(int elementTypeSize, int length)
{
    Array* array  = malloc(sizeof(Array) + elementTypeSize * length);
    array->data   = (char*) array + sizeof(Array);
    array->length = length;

    return array;
}


struct AArray AArray[1] =
{{
    Create,
}};

