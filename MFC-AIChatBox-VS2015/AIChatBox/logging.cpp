#include "pch.h"
#include "logging.h"
// �������� Visual Studio �е�һЩ��ȫ���棬��������ûʵ��������غ�
//use _CRT_SECURE_NO_WARNINGS

namespace Logging {
    // �������־���𣬳�ʼ��Ϊ0
    uint8_t SAVED_LOG_LEVEL = 0;
    // ��ǰ��Ч����־����Ĭ�������˶�����־���ͼ��ļ�д��
    uint8_t LOG_LEVEL =
        ERROR_LVL | SUCCESS_LVL | DEBUG_LVL | WARN_LVL | INFO_LVL | LOG_WRITE;

    // ��ȡ��ǰϵͳʱ�䣬��ʽ��Ϊ�ַ���
    std::string getCurrentTime() {
        // ��ȡ��ǰʱ���ʱ���
        std::time_t now = std::time(nullptr);
        // ��ʱ���ת��Ϊ����ʱ��
        std::tm* local_time = std::localtime(&now);
        char buffer[80];
        // ����ָ����ʽ��ʱ��д�뻺����
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
        return std::string(buffer);
    }

    // ����־��Ϣд���ļ�
    void writeToFile(const char* prefix, const char* msg, va_list args) {
        // ����������ļ�д����־����
        if (LOG_LEVEL & LOG_WRITE) {
            // ��׷��ģʽ����־�ļ�log.txt
            std::ofstream log_file("log.txt", std::ios_base::app);
            if (log_file.is_open()) {
                // ��д�뵱ǰʱ�����־ǰ׺
                log_file << getCurrentTime() << " " << prefix << " ";
                char buffer[1024];
                // ʹ��vsnprintf��ʽ����־���ݵ�������
                vsnprintf(buffer, sizeof(buffer), msg, args);
                // ������������д���ļ�������
                log_file << buffer << std::endl;
                // �ر��ļ�
                log_file.close();
            }
        }
    }

    // ����������־��Ϣ���������̨
    void disable_msg() {
        // ���浱ǰ��־����
        SAVED_LOG_LEVEL = LOG_LEVEL;
        // ����ǰ��־�������㣬���������п���̨���
        LOG_LEVEL = 0;
    }

    // ������־д���ļ�����
    void disable_file_logging() {
        // ͨ����λ��ǲ��������LOG_WRITE��־λ
        LOG_LEVEL &= ~LOG_WRITE;
    }

    // �ָ�֮ǰ�������־�����������ÿ���̨��־���
    void enable_msg() {
        LOG_LEVEL = SAVED_LOG_LEVEL;
    }

    // ���������־
    void error(const char* msg, ...) {
        // ��������˴�����־����
        if (LOG_LEVEL & ERROR_LVL) {
            // ���׼���������ӡ����ǰ׺��ԭע��������ɫ��ش��뵫��ע�͵���
            //fprintf(stderr, "%s ", RED "[ERR] " ANSI_COLOR_RESET);
            fprintf(stderr, "%s ", "[ERR] ");
            va_list args;
            // ��ʼ���ɱ�����б�
            va_start(args, msg);
            // ��ӡ��ʽ���Ĵ�����Ϣ
            vprintf(msg, args);
            printf("\n");
            // ��������־д���ļ�
            writeToFile("[ERR]", msg, args);
            // ����ɱ�����б�
            va_end(args);
        }
    }

    // ����ɹ���־
    void success(const char* msg, ...) {
        // ��������˳ɹ���־����
        if (LOG_LEVEL & SUCCESS_LVL) {
            // ���׼�����ӡ�ɹ�ǰ׺��ԭע��������ɫ��ش��뵫��ע�͵���
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

    // �����ͨ��Ϣ��־
    void info(const char* msg, ...) {
        // �����������Ϣ��־����
        if (LOG_LEVEL & INFO_LVL) {
            // ���׼�����ӡ��Ϣǰ׺��ԭע��������ɫ��ش��뵫��ע�͵���
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

    // ���������־
    void debug(const char* msg, ...) {
        // ��������˵�����־����
        if (LOG_LEVEL & DEBUG_LVL) {
            // ���׼�����ӡ����ǰ׺��ԭע��������ɫ��ش��뵫��ע�͵���
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

    // ͨ�õ���־д���ļ������������������̨
    void log(const char* msg, ...) {
        if (LOG_LEVEL & DEBUG_LVL) {
            va_list args;
            va_start(args, msg);
            writeToFile("[LOG]", msg, args);
            va_end(args);
        }
    }

    // ���������־
    void warn(const char* msg, ...) {
        // ��������˾�����־����
        if (LOG_LEVEL & WARN_LVL) {
            // ���׼�����ӡ����ǰ׺��ԭע��������ɫ��ش��뵫��ע�͵���
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

    // ������ش�����־
    void critical(const char* msg, ...) {
        // ������������ش�����־����
        if (LOG_LEVEL & CRITICAL_LVL) {
            // ���׼�����ӡ���ش���ǰ׺��ԭע��������ɫ��ش��뵫��ע�͵���
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