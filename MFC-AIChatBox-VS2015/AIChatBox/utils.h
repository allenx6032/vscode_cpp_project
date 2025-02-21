#pragma once

// �鿴�˿��Ƿ�ռ��
bool IsPortAvailable(int port);

// ��ȡ�ļ����µ������ļ�
std::vector<std::string> getFilesInFolder(const std::string& folderPath);

// ��CStringת��Ϊstd::string
std::string ConvertCStringToUtf8(const CString& cstr);

// ��std::stringת��ΪCString
CString ConvertUtf8ToCString(const std::string& utf8Str);

// ��std::string��������ΪUTF-8���룩ת��ΪCStringW��UTF-16���룩
CStringW ConvertUtf8ToUtf16(const std::string& utf8Str);

// ��������ı����й淶����������淶���������ں���ʵ���ж���
std::string normalizeText(std::string text);

// ��������ַ���ת��Ϊ��д��ʽ
std::string toUpperCase(std::string input);

// ��������ַ���ת��ΪСд��ʽ
std::string toLowerCase(std::string input);

// ��ȡ��ǰ�����ڣ����ظ�ʽ���ں���ʵ���ж���
const std::string getCurrentDate();

// ��ȡ��ǰ��ʱ���
int64_t getTimeStamp();

// �������в����в���ָ��Ŀ�������Ӧ��ֵ
const char* get_arg_value(int argc, char** argv, const char* target_arg);

// ����һ������Python�ֵ��ģ����
template<typename keyT>
class Dict {
public:
    // ���ü�ֵ�ԣ�֧�ֲ�ͬ���͵�value
    template<typename valueT>
    void set(keyT key, valueT value) {
        // ����ֵ�Դ����ڲ���map����
        vals[key] = value;
    }

    // ��ȡָ������Ӧ��ֵ����ת��Ϊָ������
    template<typename valueT>
    valueT get(keyT key) {
        // ʹ��any_cast��������ת��������ֵ
        return any_cast<valueT>(vals[key]);
    }

    // �����ֵ䣬��ÿ����ֵ��ִ�д���ĺ���
    template<typename Function>
    void forEach(Function func) {
        // �����ڲ���map���������ô��뺯������ÿ����ֵ��
        for (auto& pair : vals) {
            func(pair.first, pair.second);
        }
    }

    // ����ֵ�
    void clear() {
        vals.clear();
    }

private:
    // ʹ��map�����洢��ֵ�ԣ�ֵ������Ϊnana::any��֧�ֶ�������
    std::map<keyT, Any> vals;
};