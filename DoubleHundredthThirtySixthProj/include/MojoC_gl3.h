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
 * Since    : 2017-3-12
 * Update   : 2019-2-23
 * Author   : scott.cgi
 */


#include "MojoC_Platform.h"


#ifdef IS_PLATFORM_ANDROID
    #include <GLES3/gl3.h>
#elif defined(IS_PLATFORM_IOS)
    #include <OpenGLES/ES3/gl.h>
#elif defined(IS_PLATFORM_WINDOWS)
    #include <glad.h>
#endif
