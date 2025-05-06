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
 * Since    : 2018-12-6
 * Update   : 2019-7-8
 * Author   : scott.cgi
 */


#include "MojoC_Platform.h"


//------------------------
#ifdef IS_PLATFORM_WINDOWS
//------------------------



#include <android/configuration.h>
#include "MojoC_Log.h"
#include "MojoC_SystemInfo.h"


extern AConfiguration* nativeActivityConfig;


static void GetLanguageCode(char* outLanguageCode)
{
    AConfiguration_getLanguage(nativeActivityConfig, outLanguageCode);
}


struct ASystemInfo ASystemInfo[1] =
{{
    GetLanguageCode,
}};


//---------------------------
#endif // IS_PLATFORM_ANDROID
//---------------------------
