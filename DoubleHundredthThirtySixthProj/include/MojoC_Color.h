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
 * Since    : 2013-2-1
 * Update   : 2019-1-25
 * Author   : scott.cgi
 */


#ifndef COLOR_H
#define COLOR_H


/**
 * RGBA between in [0.0f, 1.0f].
 */
typedef struct
{
    float r;
    float g;
    float b;
    float a;
}
Color;


struct AColor
{
    /**
     * String format to 000000 - ffffffff
     */
    void (*SetWithString)(char* colorStr, Color* outColor);
};


extern struct AColor AColor[1];


#define COLOR_RED   {1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREED {0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE  {0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_WHITE {1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_BLACK {0.0f, 0.0f, 0.0f, 1.0f}

#define COLOR_RED_ARRAY   (Color[1]) {1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREED_ARRAY (Color[1]) {0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE_ARRAY  (Color[1]) {0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_WHITE_ARRAY (Color[1]) {1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_BLACK_ARRAY (Color[1]) {0.0f, 0.0f, 0.0f, 1.0f}


/**
 * Construct Color composite literal.
 */
#define AColor_Make(r, g, b, a) \
   (Color[1]) {{r, g, b, a}}


#endif
