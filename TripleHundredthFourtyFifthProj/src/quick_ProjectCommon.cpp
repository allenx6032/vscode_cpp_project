#include "quick_ProjectCommon.h"

extern char* ProjectConfigConvertTCharToANSI(const TCHAR* src)
// static char* ProjectConfigConvertTCharToANSI(const TCHAR* src)
{
	char* pWideCharStr;
	int nLenOfWideChar, nReturnlen;
	nLenOfWideChar = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	if (!nLenOfWideChar) {
		return NULL;
	}
	pWideCharStr = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nLenOfWideChar + 1);
	nReturnlen = WideCharToMultiByte(CP_ACP, 0, src, -1, pWideCharStr, nLenOfWideChar, NULL, NULL);
	if (!nReturnlen){
		HeapFree(GetProcessHeap(), 0, pWideCharStr);
		return NULL;
	}

	return pWideCharStr;
}

//
extern std::string ProjectConfigGetApplicationExePath()
// static std::string ProjectConfigGetApplicationExePath()
{
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    std::u16string u16ApplicationName;
	char *applicationExePath = ProjectConfigConvertTCharToANSI(szFileName);
	if (!applicationExePath) {
		return std::string("Error on convertTCharToANSI");
	}
	std::string path(applicationExePath);
	HeapFree(GetProcessHeap(), 0, applicationExePath);
	applicationExePath = NULL;

    return path;
}

extern std::string ProjectConfigRemoveFilename(const std::string& path) 
// static std::string ProjectConfigRemoveFilename(const std::string& path)
{
    // 统一处理路径分隔符为 '/'
    std::string normalized_path = path;
    std::replace(normalized_path.begin(), normalized_path.end(), '\\', '/');

    // 查找最后一个 '/'
    size_t last_slash = normalized_path.find_last_of('/');
    if (last_slash != std::string::npos) {
        // 如果 '/' 后还有字符（文件名），则截取到该位置
        return path.substr(0, last_slash + 1);
    }
    // 无分隔符时返回空（可能是当前目录或仅文件名）
    return "";
}