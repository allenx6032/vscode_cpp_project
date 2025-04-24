
#ifndef __COCOSSTUDIOEXTENSION_H__
#define __COCOSSTUDIOEXTENSION_H__

#include "cocos2d-CCMacros.h"

NS_CC_BEGIN

struct CC_DLL ResourceData
{
    int         type;
    std::string file;
    std::string plist;

    ResourceData()
    {
        type = 0;
        file = "";
        plist = "";
    }

    ResourceData(int iType, std::string sFile, std::string sPlist)
    {
        type = iType;
        file = sFile;
        plist = sPlist;
    }
};

NS_CC_END


#endif
