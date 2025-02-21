#include "pch.h"
#include "utils.h"

bool IsPortAvailable(int port)
{
	// ����һ��socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		// �������ʧ�ܣ�����false
		return false;
	}
	// ���õ�ַ����ѡ��
	int opt = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	// ���õ�ַ��Ͷ˿�
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	// �󶨵�ַ
	int result = bind(s, (sockaddr*)&addr, sizeof(addr));
	// �ر�socket
	closesocket(s);
	// ����󶨳ɹ�������true�����򷵻�false
	return result != SOCKET_ERROR;
}

std::vector<std::string> getFilesInFolder(const std::string& folderPath)
{
	std::vector<std::string> files;
	// ʹ��Windows API��FindFirstFile��FindNextFile���������ļ���
	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA((folderPath + "\\*").c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		// �������ʧ�ܣ����ؿ��ļ��б�
		return files;
	}
	do {
		// ���ļ�����ӵ��ļ��б���
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
	// ����CString��Ҫ�Ļ�������С
	int cstrLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
	if (cstrLength == 0) {
		// ת��ʧ�ܣ����ؿ�CString
		return CString();
	}
	// ����CString�Ļ�����
	CString cstr;
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, cstr.GetBuffer(cstrLength), cstrLength);
	cstr.ReleaseBuffer();
	return cstr;
}

CStringW ConvertUtf8ToUtf16(const std::string& utf8Str)
{
    // ����UTF-16����Ļ�������С
    int utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    if (utf16Length == 0) {
        // ת��ʧ�ܣ����ؿ�CStringW
        return CStringW();
    }

    // ����UTF-16�ַ����Ļ�����
    CStringW utf16Str;
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, utf16Str.GetBuffer(utf16Length), utf16Length);
    utf16Str.ReleaseBuffer();

    return utf16Str;
}

// �淶���ı�����һЩ�����ַ�����ת��
std::string normalizeText(std::string text) {
    std::string result;
    // Ԥ�ȷ���һЩ����ռ䣬����Ƶ������
    result.reserve(text.size() * 1.1);
    for (char ch : text) {
        switch (ch) {
        case '\n':
            // �����з�ת��
            result.append("\\n");
            break;
        case '"':
            // ��˫����ת��
            result.append("\\\"");
            break;
        case '\t':
            // ���Ʊ��ת��
            result.append("\\t");
            break;
        default:
            // ��ͨ�ַ�ֱ�����
            result.push_back(ch);
        }
    }
    return result;
}

// �������ַ���תΪ��д��ʽ
std::string toUpperCase(std::string input) {
    std::string result(input);
    // ʹ�ñ�׼���transform�������toupper��������ת��
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// �������ַ���תΪСд��ʽ
std::string toLowerCase(std::string input) {
    std::string result(input);
    // ʹ�ñ�׼���transform�������tolower��������ת��
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ��ȡ��ǰ���ڣ���ʽΪ "YYYY-MM-DD-HH-MM-SS"
const std::string getCurrentDate() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    // ��ȡ����ʱ��
    tstruct = *localtime(&now);
    // ����ָ����ʽ��ʽ��ʱ��
    strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct);

    return buf;
}

// ��ȡ��ǰʱ���
int64_t getTimeStamp() {
    return static_cast<int64_t>(time(0));
}

// �������в��������в���Ŀ�������Ӧ��ֵ
const char* get_arg_value(int argc, char** argv, const char* target_arg) {
    for (int arg_idx = 0; arg_idx < argc; arg_idx++) {
        // ����ҵ�Ŀ�����
        if (!strcmp(argv[arg_idx], target_arg))
            // ������һ������ֵ�����������򷵻ؿ��ַ���
            return argv[arg_idx + 1] == nullptr ? "" : argv[arg_idx + 1];
    }
    return nullptr;
}
