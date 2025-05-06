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
 * Since    : 2013-11-17
 * Update   : 2019-1-25
 * Author   : scott.cgi
 */


#include <stdio.h>
#include "MojoC_Color.h"


static void SetWithString(char* colorStr, Color* outColor)
{
    int rgba[4];

    sscanf(colorStr,     "%2x", rgba);
    sscanf(colorStr + 2, "%2x", rgba + 1);
    sscanf(colorStr + 4, "%2x", rgba + 2);
    sscanf(colorStr + 6, "%2x", rgba + 3);

    outColor->r = rgba[0] / 255.0f;
    outColor->g = rgba[1] / 255.0f;
    outColor->b = rgba[2] / 255.0f;
    outColor->a = rgba[3] / 255.0f;
}


struct AColor AColor[1] =
{{
    SetWithString,
}};
