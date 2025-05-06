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
 * Since    : 2019-5-8
 * Update   : 2019-8-8
 * Author   : scott.cgi
 */


#include "MojoC_Platform.h"


//--------------------
#ifdef IS_PLATFORM_IOS
//--------------------


#include <Foundation/Foundation.h>
#include "MojoC_Log.h"
#include "MojoC_SystemInfo.h"


static void GetLanguageCode(char* outLanguageCode)
{
    NSString* languageCode = [NSLocale preferredLanguages][0];

    if (languageCode != nil)
    {
        outLanguageCode[0] = languageCode.UTF8String[0];
        outLanguageCode[1] = languageCode.UTF8String[1];
    }
    else
    {
        outLanguageCode[0] = 'e';
        outLanguageCode[1] = 'n';
    }
}


struct ASystemInfo ASystemInfo[1] =
{{
    GetLanguageCode,
}};


//-----------------------
#endif // IS_PLATFORM_IOS
//-----------------------
