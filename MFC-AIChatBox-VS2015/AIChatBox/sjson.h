#ifndef __SJSON_H__
#define __SJSON_H__

#include <iostream>
#include <vector>
#include <string>
// 引入外部C语言的yyjson库
extern "C" {
#include "yyjson.h"  
}

// sjson类，是对yyjson的基础包装类
class sjson {
public:
    // 构造函数，从普通文件读取JSON数据
    sjson(const char* filename);
    // 构造函数，从原始JSON数据构造，通过 'json::open(json::read(rawstr))' 方式使用
    sjson(yyjson_doc* json_doc);

    // 打开指定文件名的JSON文件
    yyjson_doc* open(const char* filename);
    // 静态方法，从原始JSON字符串读取并解析
    static yyjson_doc* read(const char* raw_json);
    // 判断当前JSON文档是否成功打开
    bool is_opened();

	// 获取JSON文件中第一级的键
    std::vector<std::string> get_top_level_keys();
	// 获取JSON文件中所有的键
	std::vector<std::string> get_all_keys();
    // 获取多层嵌套的JSON值，例如 sjson::get_value({"id", "name"}) 对应 file.json ['id']['name']
    yyjson_val* get_value(const char* keys[]);
	yyjson_val* get_value(std::string keys);
    // 获取多层嵌套的JSON字符串值
    const char* get_str(const char* keys[]);
    // 获取多层嵌套的JSON整数值
    int64_t get_int(const char* keys[]);
    // 获取多层嵌套的JSON布尔值
    bool get_bool(const char* keys[]);
    // 获取多层嵌套的JSON实数值
    double get_real(const char* keys[]);

    // 获取当前的JSON文档指针
    yyjson_doc* get_current_doc();
    // 获取当前JSON文档的根节点指针
    yyjson_val* get_current_root();
    // 获取多层嵌套的JSON值的类型
    const char* get_type(const char* keys[]);

    // 析构函数
    ~sjson();

private:
    // 存储JSON文件的文件名
    std::string doc_filename;
    // 当前的JSON文档指针
    yyjson_doc* current_doc = NULL;
    // 当前JSON文档的根节点指针
    yyjson_val* current_doc_root = NULL;

    // 读取JSON文件的标志，允许注释和尾随逗号
    yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
    // 读取JSON文件时的错误信息
    yyjson_read_err err;
};

#endif