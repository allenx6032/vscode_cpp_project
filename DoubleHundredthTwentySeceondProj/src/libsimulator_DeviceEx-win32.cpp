#include "libsimulator_DeviceEx.h"

// for mac address
#include <WinSock2.h>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

using namespace std;
PLAYER_NS_BEGIN

DeviceEx *DeviceEx::getInstance()
{
    static DeviceEx *instance = NULL;
    if (!instance)
    {
        instance = new DeviceEx();
        instance->init();
    }
    return instance;
}

std::string DeviceEx::getCurrentUILangName()
{
    return _uiLangName;
}

std::string DeviceEx::getUserGUID()
{
    return _userGUID;
}


//////////  private  //////////

DeviceEx::DeviceEx()
    : _uiLangName("en")
{

}

void DeviceEx::init()
{
    makeUILangName();
    makeUserGUID();
}

void DeviceEx::makeUILangName()
{
    //
    // get language
    // http://msdn.microsoft.com/en-us/library/windows/apps/jj244362(v=vs.105).aspx
    //
    ULONG numLanguages = 0;
    DWORD cchLanguagesBuffer = 0;
    BOOL hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, NULL, &cchLanguagesBuffer);
    if (hr)
    {
        WCHAR* pwszLanguagesBuffer = new WCHAR[cchLanguagesBuffer];
        hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, pwszLanguagesBuffer, &cchLanguagesBuffer);
        if (hr)
        {
            size_t size = wcslen(pwszLanguagesBuffer) * 3 + 1;
            char* dest = new char[size];
            memset(dest, 0, size);
            WideCharToMultiByte(CP_UTF8, 0, pwszLanguagesBuffer, -1, dest, size, NULL, NULL);
            _uiLangName = dest;
        }
        delete [] pwszLanguagesBuffer;
    }
}

static bool getMacAddress(string& macstring)
{
    bool ret = false;
    ULONG ipInfoLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO adapterInfo = (IP_ADAPTER_INFO *)malloc(ipInfoLen);
    if (adapterInfo == NULL)
    {
        return false;
    }

    if (GetAdaptersInfo(adapterInfo, &ipInfoLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(adapterInfo);
        adapterInfo = (IP_ADAPTER_INFO *)malloc(ipInfoLen);
        if (adapterInfo == NULL)
        {
            return false;
        }
    }

    if (GetAdaptersInfo(adapterInfo, &ipInfoLen) == NO_ERROR)
    {
        for (PIP_ADAPTER_INFO pAdapter = adapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
        {
            if (pAdapter->Type != MIB_IF_TYPE_ETHERNET)
            {
                continue;
            }

            if (pAdapter->AddressLength != 6)
            {
                continue;
            }

            char buf32[32];
            sprintf(buf32, "%02X-%02X-%02X-%02X-%02X-%02X",
                int(pAdapter->Address[0]),
                int(pAdapter->Address[1]),
                int(pAdapter->Address[2]),
                int(pAdapter->Address[3]),
                int(pAdapter->Address[4]),
                int(pAdapter->Address[5]));
            macstring = buf32;
            ret = true;
            break;
        }
    }

    free(adapterInfo);
    return ret;
}

std::string DeviceEx::makeUserGUID()
{
    if (_userGUID.length() <= 0)
    {
        if (!getMacAddress(_userGUID))
        {
            _userGUID = "guid-fixed-1234567890";
        }
    }

    return _userGUID;
}

PLAYER_NS_END
