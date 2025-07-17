#ifndef project_common_h
#define project_common_h


#include <Shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shellapi.h>
#include <windows.h>
#include <iostream>
#include <algorithm>

// for mac address
#include <WinSock2.h>
#include <Iphlpapi.h>

extern char* ProjectConfigConvertTCharToANSI(const TCHAR* src);
// static char* ProjectConfigConvertTCharToANSI(const TCHAR* src);
extern std::string ProjectConfigGetApplicationExePath();
// static std::string ProjectConfigGetApplicationExePath();
extern std::string ProjectConfigRemoveFilename(const std::string& path);
// static std::string ProjectConfigRemoveFilename(const std::string& path);



#endif