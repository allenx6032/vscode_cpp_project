

#include "cocos2d-CCConfig.h"
#if CC_USE_CCS > 0


#ifndef __COCOSSTUDIOEXTENSION_H__
#define __COCOSSTUDIOEXTENSION_H__

#include "cocos2d-CCAffineTransform.h"

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

class CC_DLL NodeExtension
{
public:
    NodeExtension();
    ~NodeExtension();

private:

};


NS_CC_END


#endif

#endif // CC_USE_CCS

