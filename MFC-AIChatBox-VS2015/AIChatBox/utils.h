#pragma once

// 查看端口是否被占用
bool IsPortAvailable(int port);

// 获取文件夹下的所有文件
std::vector<std::string> getFilesInFolder(const std::string& folderPath);

// 将CString转换为std::string
std::string ConvertCStringToUtf8(const CString& cstr);

// 将std::string转换为CString
CString ConvertUtf8ToCString(const std::string& utf8Str);

// 将std::string（假设其为UTF-8编码）转换为CStringW（UTF-16编码）
CStringW ConvertUtf8ToUtf16(const std::string& utf8Str);

// 对输入的文本进行规范化处理，具体规范化规则需在函数实现中定义
std::string normalizeText(std::string text);

// 将输入的字符串转换为大写形式
std::string toUpperCase(std::string input);

// 将输入的字符串转换为小写形式
std::string toLowerCase(std::string input);

// 获取当前的日期，返回格式需在函数实现中定义
const std::string getCurrentDate();

// 获取当前的时间戳
int64_t getTimeStamp();

// 在命令行参数中查找指定目标参数对应的值
const char* get_arg_value(int argc, char** argv, const char* target_arg);

// 定义一个类似Python字典的模板类
template<typename keyT>
class Dict {
public:
    // 设置键值对，支持不同类型的value
    template<typename valueT>
    void set(keyT key, valueT value) {
        // 将键值对存入内部的map容器
        vals[key] = value;
    }

    // 获取指定键对应的值，并转换为指定类型
    template<typename valueT>
    valueT get(keyT key) {
        // 使用any_cast进行类型转换并返回值
        return any_cast<valueT>(vals[key]);
    }

    // 遍历字典，对每个键值对执行传入的函数
    template<typename Function>
    void forEach(Function func) {
        // 遍历内部的map容器，调用传入函数处理每个键值对
        for (auto& pair : vals) {
            func(pair.first, pair.second);
        }
    }

    // 清空字典
    void clear() {
        vals.clear();
    }

private:
    // 使用map容器存储键值对，值的类型为nana::any，支持多种类型
    std::map<keyT, Any> vals;
};