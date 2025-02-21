#include "pch.h"
#include "utils.h"

bool IsPortAvailable(int port)
{
	// 创建一个socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		// 如果创建失败，返回false
		return false;
	}
	// 设置地址复用选项
	int opt = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	// 设置地址族和端口
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	// 绑定地址
	int result = bind(s, (sockaddr*)&addr, sizeof(addr));
	// 关闭socket
	closesocket(s);
	// 如果绑定成功，返回true，否则返回false
	return result != SOCKET_ERROR;
}

std::vector<std::string> getFilesInFolder(const std::string& folderPath)
{
	std::vector<std::string> files;
	// 使用Windows API的FindFirstFile和FindNextFile函数遍历文件夹
	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA((folderPath + "\\*").c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		// 如果查找失败，返回空文件列表
		return files;
	}
	do {
		// 将文件名添加到文件列表中
		files.push_back(findFileData.cFileName);
	} while (FindNextFileA(hFind, &findFileData) != 0);
	FindClose(hFind);
	return files;
}

std::string ConvertCStringToUtf8(const CString& cstr)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, cstr.GetString(), -1, nullptr, 0, nullptr, nullptr);
    std::string ansiString(bufferSize - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, cstr.GetString(), -1, &ansiString[0], bufferSize, nullptr, nullptr);
	return ansiString;
}

CString ConvertUtf8ToCString(const std::string& utf8Str)
{
	// 计算CString需要的缓冲区大小
	int cstrLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
	if (cstrLength == 0) {
		// 转换失败，返回空CString
		return CString();
	}
	// 分配CString的缓冲区
	CString cstr;
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, cstr.GetBuffer(cstrLength), cstrLength);
	cstr.ReleaseBuffer();
	return cstr;
}

CStringW ConvertUtf8ToUtf16(const std::string& utf8Str)
{
    // 计算UTF-16所需的缓冲区大小
    int utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    if (utf16Length == 0) {
        // 转换失败，返回空CStringW
        return CStringW();
    }

    // 分配UTF-16字符串的缓冲区
    CStringW utf16Str;
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, utf16Str.GetBuffer(utf16Length), utf16Length);
    utf16Str.ReleaseBuffer();

    return utf16Str;
}

// 规范化文本，对一些特殊字符进行转义
std::string normalizeText(std::string text) {
    std::string result;
    // 预先分配一些额外空间，避免频繁扩容
    result.reserve(text.size() * 1.1);
    for (char ch : text) {
        switch (ch) {
        case '\n':
            // 将换行符转义
            result.append("\\n");
            break;
        case '"':
            // 将双引号转义
            result.append("\\\"");
            break;
        case '\t':
            // 将制表符转义
            result.append("\\t");
            break;
        default:
            // 普通字符直接添加
            result.push_back(ch);
        }
    }
    return result;
}

// 将输入字符串转为大写形式
std::string toUpperCase(std::string input) {
    std::string result(input);
    // 使用标准库的transform函数结合toupper函数进行转换
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// 将输入字符串转为小写形式
std::string toLowerCase(std::string input) {
    std::string result(input);
    // 使用标准库的transform函数结合tolower函数进行转换
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// 获取当前日期，格式为 "YYYY-MM-DD-HH-MM-SS"
const std::string getCurrentDate() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    // 获取本地时间
    tstruct = *localtime(&now);
    // 按照指定格式格式化时间
    strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct);

    return buf;
}

// 获取当前时间戳
int64_t getTimeStamp() {
    return static_cast<int64_t>(time(0));
}

// 在命令行参数数组中查找目标参数对应的值
const char* get_arg_value(int argc, char** argv, const char* target_arg) {
    for (int arg_idx = 0; arg_idx < argc; arg_idx++) {
        // 如果找到目标参数
        if (!strcmp(argv[arg_idx], target_arg))
            // 返回下一个参数值，若不存在则返回空字符串
            return argv[arg_idx + 1] == nullptr ? "" : argv[arg_idx + 1];
    }
    return nullptr;
}
