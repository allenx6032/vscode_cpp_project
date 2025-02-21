#include "pch.h"
#include "logging.h"
// 用于抑制 Visual Studio 中的一些安全警告，不过这里没实际启用相关宏
//use _CRT_SECURE_NO_WARNINGS

namespace Logging {
    // 保存的日志级别，初始化为0
    uint8_t SAVED_LOG_LEVEL = 0;
    // 当前生效的日志级别，默认启用了多种日志类型及文件写入
    uint8_t LOG_LEVEL =
        ERROR_LVL | SUCCESS_LVL | DEBUG_LVL | WARN_LVL | INFO_LVL | LOG_WRITE;

    // 获取当前系统时间，格式化为字符串
    std::string getCurrentTime() {
        // 获取当前时间的时间戳
        std::time_t now = std::time(nullptr);
        // 将时间戳转换为本地时间
        std::tm* local_time = std::localtime(&now);
        char buffer[80];
        // 按照指定格式将时间写入缓冲区
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
        return std::string(buffer);
    }

    // 将日志信息写入文件
    void writeToFile(const char* prefix, const char* msg, va_list args) {
        // 如果启用了文件写入日志功能
        if (LOG_LEVEL & LOG_WRITE) {
            // 以追加模式打开日志文件log.txt
            std::ofstream log_file("log.txt", std::ios_base::app);
            if (log_file.is_open()) {
                // 先写入当前时间和日志前缀
                log_file << getCurrentTime() << " " << prefix << " ";
                char buffer[1024];
                // 使用vsnprintf格式化日志内容到缓冲区
                vsnprintf(buffer, sizeof(buffer), msg, args);
                // 将缓冲区内容写入文件并换行
                log_file << buffer << std::endl;
                // 关闭文件
                log_file.close();
            }
        }
    }

    // 禁用所有日志消息输出到控制台
    void disable_msg() {
        // 保存当前日志级别
        SAVED_LOG_LEVEL = LOG_LEVEL;
        // 将当前日志级别清零，即禁用所有控制台输出
        LOG_LEVEL = 0;
    }

    // 禁用日志写入文件功能
    void disable_file_logging() {
        // 通过按位与非操作，清除LOG_WRITE标志位
        LOG_LEVEL &= ~LOG_WRITE;
    }

    // 恢复之前保存的日志级别，重新启用控制台日志输出
    void enable_msg() {
        LOG_LEVEL = SAVED_LOG_LEVEL;
    }

    // 输出错误日志
    void error(const char* msg, ...) {
        // 如果启用了错误日志级别
        if (LOG_LEVEL & ERROR_LVL) {
            // 向标准错误输出打印错误前缀，原注释里有颜色相关代码但被注释掉了
            //fprintf(stderr, "%s ", RED "[ERR] " ANSI_COLOR_RESET);
            fprintf(stderr, "%s ", "[ERR] ");
            va_list args;
            // 初始化可变参数列表
            va_start(args, msg);
            // 打印格式化的错误信息
            vprintf(msg, args);
            printf("\n");
            // 将错误日志写入文件
            writeToFile("[ERR]", msg, args);
            // 清理可变参数列表
            va_end(args);
        }
    }

    // 输出成功日志
    void success(const char* msg, ...) {
        // 如果启用了成功日志级别
        if (LOG_LEVEL & SUCCESS_LVL) {
            // 向标准输出打印成功前缀，原注释里有颜色相关代码但被注释掉了
            //printf("%s", GRN "[SUCCESS] " ANSI_COLOR_RESET);
            printf("%s", "[SUCCESS] ");
            va_list args;
            va_start(args, msg);
            vprintf(msg, args);
            printf("\n");
            writeToFile("[SUCCESS]", msg, args);
            va_end(args);
        }
    }

    // 输出普通信息日志
    void info(const char* msg, ...) {
        // 如果启用了信息日志级别
        if (LOG_LEVEL & INFO_LVL) {
            // 向标准输出打印信息前缀，原注释里有颜色相关代码但被注释掉了
            //printf("%s", CYN "[INFO] " ANSI_COLOR_RESET);
            printf("%s", "[INFO] ");
            va_list args;
            va_start(args, msg);
            vprintf(msg, args);
            printf("\n");
            writeToFile("[INFO]", msg, args);
            va_end(args);
        }
    }

    // 输出调试日志
    void debug(const char* msg, ...) {
        // 如果启用了调试日志级别
        if (LOG_LEVEL & DEBUG_LVL) {
            // 向标准输出打印调试前缀，原注释里有颜色相关代码但被注释掉了
            //printf("%s", MAG "[DEBUG] " ANSI_COLOR_RESET);
            printf("%s", "[DEBUG] ");
            va_list args;
            va_start(args, msg);
            vprintf(msg, args);
            printf("\n");
            writeToFile("[DEBUG]", msg, args);
            va_end(args);
        }
    }

    // 通用的日志写入文件函数，不输出到控制台
    void log(const char* msg, ...) {
        if (LOG_LEVEL & DEBUG_LVL) {
            va_list args;
            va_start(args, msg);
            writeToFile("[LOG]", msg, args);
            va_end(args);
        }
    }

    // 输出警告日志
    void warn(const char* msg, ...) {
        // 如果启用了警告日志级别
        if (LOG_LEVEL & WARN_LVL) {
            // 向标准输出打印警告前缀，原注释里有颜色相关代码但被注释掉了
            //printf("%s", YEL "[WARN] " ANSI_COLOR_RESET);
            printf("%s", "[WARN] ");
            va_list args;
            va_start(args, msg);
            vprintf(msg, args);
            printf("\n");
            writeToFile("[WARN]", msg, args);
            va_end(args);
        }
    }

    // 输出严重错误日志
    void critical(const char* msg, ...) {
        // 如果启用了严重错误日志级别
        if (LOG_LEVEL & CRITICAL_LVL) {
            // 向标准输出打印严重错误前缀，原注释里有颜色相关代码但被注释掉了
            //printf("%s", RED "[CRITICAL] " ANSI_COLOR_RESET);
            printf("%s", "[CRITICAL] ");
            va_list args;
            va_start(args, msg);
            vprintf(msg, args);
            printf("\n");
            writeToFile("[CRITICAL]", msg, args);
            va_end(args);
        }
    }
}