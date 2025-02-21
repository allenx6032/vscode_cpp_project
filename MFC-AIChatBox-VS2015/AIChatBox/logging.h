#ifndef __MINI_LOG__
#define __MINI_LOG__

#include <stdarg.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <ctime>

// �궨�壬���ڼ򻯿ɱ�����Ĵ�ӡ
// ����һ����ʽ���ַ����Ϳɱ�����б������Ǵ��ݸ�vprintf���и�ʽ�������������һ�����з�
#define PRINT_VAR_ARGS(format,...) \
  do {                              \
    va_list arg_list;               \
    va_start(arg_list, format);     \
    vprintf(format, arg_list);      \
    printf("\n");                   \
    va_end(arg_list);               \
  } while (0)

namespace Logging {
    // ���������־������λ�����ʾ������������ʹ��
    constexpr uint8_t ERROR_LVL = 1 << 0;
    // ����ɹ���־����
    constexpr uint8_t SUCCESS_LVL = 1 << 1;
    // ���������־����
    constexpr uint8_t DEBUG_LVL = 1 << 2;
    // ���徯����־����
    constexpr uint8_t WARN_LVL = 1 << 3;
    // �������ش�����־����
    constexpr uint8_t CRITICAL_LVL = 1 << 4;
    // ������ͨ��Ϣ��־����
    constexpr uint8_t INFO_LVL = 1 << 5;
    // ����д����־�ļ��ı��
    constexpr uint8_t LOG_WRITE = 1 << 6;

    // �ⲿ���������ڱ��浱ǰ���õ���־����
    extern uint8_t SAVED_LOG_LEVEL;
    // �ⲿ���������ڵ�ǰ��Ч����־����
    extern uint8_t LOG_LEVEL;

    // ��ȡ��ǰʱ�䣬���ظ�ʽ�����ʱ���ַ���
    std::string getCurrentTime();

    // ����־��Ϣд���ļ�
    // prefix: ��־ǰ׺���������ֲ�ͬ���͵���־
    // msg: ��־�������ݵĸ�ʽ���ַ���
    // args: ��msg��Ӧ�Ŀɱ�����б�
    void writeToFile(const char* prefix, const char* msg, va_list args);

    // ������־��Ϣ���������̨
    void disable_msg();

    // ������־д���ļ�����
    void disable_file_logging();

    // ������־��Ϣ���������̨
    void enable_msg();

    // ���������־
    // msg: ������Ϣ�ĸ�ʽ���ַ���������ɽӿɱ����
    void error(const char* msg, ...);

    // ����ɹ���־
    void success(const char* msg, ...);

    // �����ͨ��Ϣ��־
    void info(const char* msg, ...);

    // ���������־
    void debug(const char* msg, ...);

    // ͨ�õ���־�������
    void log(const char* msg, ...);

    // ���������־
    void warn(const char* msg, ...);

    // ������ش�����־
    void critical(const char* msg, ...);
}

#endif