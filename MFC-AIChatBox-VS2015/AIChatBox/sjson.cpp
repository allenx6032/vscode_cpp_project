#include "pch.h"
#include "sjson.h"

// 构造函数，使用已有的yyjson_doc对象来初始化
sjson::sjson(yyjson_doc* json_doc) {
    // 将传入的JSON文档指针赋值给当前文档指针
    current_doc = json_doc;
    // 获取该文档的根节点
    current_doc_root = yyjson_doc_get_root(current_doc);
}

// 构造函数，从指定文件名的文件中读取JSON数据来初始化
sjson::sjson(const char* filename) {
    // 保存文件名
    doc_filename = filename;
    // 调用open函数读取文件
    open(filename);
}

// 静态方法，从原始JSON字符串解析出yyjson_doc对象
yyjson_doc* sjson::read(const char* raw_json) {
    // 调用yyjson库的read函数进行解析
    return yyjson_read(raw_json, strlen(raw_json), 0);
}

// 打开指定文件名的JSON文件，并解析
yyjson_doc* sjson::open(const char* filename) {
    // 使用yyjson库的read_file函数读取文件，带上允许注释和尾随逗号的标志
    current_doc = yyjson_read_file(filename, flg, NULL, &err);
    // 如果读取失败
    if (!current_doc) {
        // 输出错误信息，包括文件名、错误码、错误信息以及出错位置
        std::cout << "Problem reading the file \"" << filename << "\": read error (" << err.code << "): " << err.msg << " at position:" << err.pos << std::endl;
        return NULL;
    }

    // 获取文档的根节点
    current_doc_root = yyjson_doc_get_root(current_doc);
    return current_doc;
}

// 获取JSON文件中第一级的键
std::vector<std::string> sjson::get_top_level_keys() {
    std::vector<std::string> keys;
    if (!current_doc_root) {
        std::cerr << "Error: JSON document is not opened or root is null." << std::endl;
        return keys;
    }

    if (yyjson_is_obj(current_doc_root)) {
        // Iterate over object keys
        yyjson_obj_iter iter;
        yyjson_obj_iter_init(current_doc_root, &iter);
        while (yyjson_obj_iter_has_next(&iter)) {
            yyjson_val* key = yyjson_obj_iter_next(&iter);
            keys.push_back(yyjson_get_str(key));
        }
    } else {
        std::cerr << "Error: Root is not a JSON object." << std::endl;
    }

    return keys;
}

// 获取JSON文件中所有的键
std::vector<std::string> sjson::get_all_keys() {
    std::vector<std::string> keys;
    if (!current_doc_root) {
        std::cerr << "Error: JSON document is not opened or root is null." << std::endl;
        return keys;
    }

    // Helper function to recursively collect keys
    std::function<void(yyjson_val*, std::string)> collect_keys = [&](yyjson_val* val, std::string prefix) {
        if (yyjson_is_obj(val)) {
            // Iterate over object keys
            yyjson_obj_iter iter;
            yyjson_obj_iter_init(val, &iter);
            while (yyjson_obj_iter_has_next(&iter)) {
                yyjson_val* key = yyjson_obj_iter_next(&iter);
                yyjson_val* child = yyjson_obj_iter_get_val(key);
                std::string full_key = prefix.empty() ? yyjson_get_str(key) : prefix + "." + yyjson_get_str(key);
                keys.push_back(full_key);

                // Recursively collect keys from nested objects/arrays
                collect_keys(child, full_key);
            }
        }
        else if (yyjson_is_arr(val)) {
            // Iterate over array elements
            size_t arr_size = yyjson_arr_size(val);
            for (size_t i = 0; i < arr_size; ++i) {
                yyjson_val* child = yyjson_arr_get(val, i);
                std::string full_key = prefix.empty() ? std::to_string(i) : prefix + "." + std::to_string(i);
                collect_keys(child, full_key);
            }
        }
        };

    // Start collecting keys from the root
    collect_keys(current_doc_root, "");

    return keys;
}

// 获取多层嵌套的JSON值
yyjson_val* sjson::get_value(const char* keys[]) {
    yyjson_val* tmp = NULL;
    // 遍历键数组，逐步获取嵌套的值
    for (int i = 0; *keys[i] != '\0'; i++) {
        // 如果是首次查找，从根节点开始
        if (tmp == NULL)
            tmp = yyjson_obj_get(current_doc_root, keys[i]);
        else
            // 后续从中间结果继续查找
            tmp = yyjson_obj_get(tmp, keys[i]);
        // 如果没找到对应的值
        if (tmp == NULL) {
            // 输出错误信息，指明在哪个文件中获取哪个键值失败
            std::cerr << "Error getting the value \"" << keys[i] << "\" from \"" << doc_filename << "\"" << std::endl;
            return NULL;
        }
    }
    return tmp;
}


yyjson_val* sjson::get_value(std::string keys) {
    yyjson_val* tmp = NULL;
    std::istringstream key_stream(keys);
    std::string key;
    
    // 遍历键字符串，逐步获取嵌套的值
    while (std::getline(key_stream, key, '.')) {
        // 如果是首次查找，从根节点开始
        if (tmp == NULL)
            tmp = yyjson_obj_get(current_doc_root, key.c_str());
        else
            // 后续从中间结果继续查找
            tmp = yyjson_obj_get(tmp, key.c_str());
        // 如果没找到对应的值
        if (tmp == NULL) {
            // 输出错误信息，指明在哪个文件中获取哪个键值失败
            std::cerr << "Error getting the value \"" << key << "\" from \"" << doc_filename << "\"" << std::endl;
            return NULL;
        }
    }
    return tmp;
}

// 获取多层嵌套的JSON字符串值
const char* sjson::get_str(const char* keys[]) {
    // 先获取对应的值
    yyjson_val* value = get_value(keys);
    // 再尝试获取其字符串表示，如果为空则返回空字符串
    const char* string = yyjson_get_str(value);
    return string ? string : "";
}

// 获取多层嵌套的JSON整数值
int64_t sjson::get_int(const char* keys[]) {
    // 获取对应的值并转换为有符号整数
    return yyjson_get_sint(get_value(keys));
}

// 判断当前JSON文档是否成功打开
bool sjson::is_opened() {
    // 如果当前文档指针为空，说明未成功打开，返回false；否则返回true
    return current_doc == NULL ? false : true;
}

// 获取多层嵌套的JSON布尔值
bool sjson::get_bool(const char* keys[]) {
    // 获取对应的值并转换为布尔值
    return yyjson_get_bool(get_value(keys));
}

// 获取多层嵌套的JSON实数值
double sjson::get_real(const char* keys[]) {
    // 获取对应的值并转换为实数
    return yyjson_get_real(get_value(keys));
}

// 获取当前的JSON文档指针
yyjson_doc* sjson::get_current_doc() {
    return current_doc;
}

// 获取当前JSON文档的根节点指针
yyjson_val* sjson::get_current_root() {
    return current_doc_root;
}

// 获取多层嵌套的JSON值的类型
const char* sjson::get_type(const char* keys[]) {
    // 先获取对应的值
    yyjson_val* value = get_value(keys);
    if (value) {
        // 获取其类型描述
        return yyjson_get_type_desc(value);
    }
    else {
        // 如果获取失败，输出错误信息
        std::cerr << "Failed to get type" << std::endl;
        return NULL;
    }
}

// 析构函数，释放当前的JSON文档内存
sjson::~sjson() { yyjson_doc_free(current_doc); }