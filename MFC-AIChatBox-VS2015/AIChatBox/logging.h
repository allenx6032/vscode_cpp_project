#ifndef __MINI_LOG__
#define __MINI_LOG__

#include <stdarg.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <ctime>

// 宏定义，用于简化可变参数的打印
// 接受一个格式化字符串和可变参数列表，将它们传递给vprintf进行格式化输出，最后输出一个换行符
#define PRINT_VAR_ARGS(format,...) \
  do {                              \
    va_list arg_list;               \
    va_start(arg_list, format);     \
    vprintf(format, arg_list);      \
    printf("\n");                   \
    va_end(arg_list);               \
  } while (0)

namespace Logging {
    // 定义错误日志级别，用位运算表示，方便后续组合使用
    constexpr uint8_t ERROR_LVL = 1 << 0;
    // 定义成功日志级别
    constexpr uint8_t SUCCESS_LVL = 1 << 1;
    // 定义调试日志级别
    constexpr uint8_t DEBUG_LVL = 1 << 2;
    // 定义警告日志级别
    constexpr uint8_t WARN_LVL = 1 << 3;
    // 定义严重错误日志级别
    constexpr uint8_t CRITICAL_LVL = 1 << 4;
    // 定义普通信息日志级别
    constexpr uint8_t INFO_LVL = 1 << 5;
    // 定义写入日志文件的标记
    constexpr uint8_t LOG_WRITE = 1 << 6;

    // 外部声明，用于保存当前设置的日志级别
    extern uint8_t SAVED_LOG_LEVEL;
    // 外部声明，用于当前生效的日志级别
    extern uint8_t LOG_LEVEL;

    // 获取当前时间，返回格式化后的时间字符串
    std::string getCurrentTime();

    // 将日志信息写入文件
    // prefix: 日志前缀，用于区分不同类型的日志
    // msg: 日志主体内容的格式化字符串
    // args: 与msg对应的可变参数列表
    void writeToFile(const char* prefix, const char* msg, va_list args);

    // 禁用日志消息输出到控制台
    void disable_msg();

    // 禁用日志写入文件功能
    void disable_file_logging();

    // 启用日志消息输出到控制台
    void enable_msg();

    // 输出错误日志
    // msg: 错误信息的格式化字符串，后面可接可变参数
    void error(const char* msg, ...);

    // 输出成功日志
    void success(const char* msg, ...);

    // 输出普通信息日志
    void info(const char* msg, ...);

    // 输出调试日志
    void debug(const char* msg, ...);

    // 通用的日志输出函数
    void log(const char* msg, ...);

    // 输出警告日志
    void warn(const char* msg, ...);

    // 输出严重错误日志
    void critical(const char* msg, ...);
}

#endif