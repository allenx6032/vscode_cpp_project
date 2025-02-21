#ifndef __MINIPOST_H__
#define __MINIPOST_H__

#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <winsock2.h>
#include <windows.h>

// ������־���ն���ص�ͷ�ļ�
#include "logging.h"
//#include "terminal.h"

// ��SOCKET��������������������������д
typedef SOCKET SocketType;
// ��SOCKET_ERROR������������ڱ�ʾsocket��ش���
#define Socket_error SOCKET_ERROR  
// ��INVALID_SOCKET������������ڱ�ʾ��Ч��socket
#define Invalid_socket INVALID_SOCKET  

// ���建������СΪ100KB
#define BUFFER_SIZE 100000  
// ����HTTP�ڲ�������
#define HTTP_INTERNAL_ERROR 500  
// ����HTTP�ɹ�״̬��
#define HTTP_OK 200  

// ��std::string������������ڱ�ʾJSON��������
typedef std::string json;

// ������Ӧ�ṹ�壬����״̬�����Ӧ��
struct Response {
    int16_t Status = -1;
    std::string body;
};

// ����ص����߽ṹ�壬���ڴ������ݻص�
struct CallbackBus {
    // ������������ȷ������������ȷ����
    virtual ~CallbackBus() = default;
    // ���ڴ洢���յ������ݻ�����
    std::string buffer;
    // ����Ƿ�Ϊ��ʽ����
    bool stream;
};

// HTTP������
class httpRequest {
public:
    // Ĭ�Ϲ��캯��
    httpRequest();
    // ����POST����֧����ʽ���ݻص���ȡ
    // ipaddr: Ŀ��IP��ַ
    // port: Ŀ��˿ں�
    // endpoint: ����Ķ˵�·��
    // payload: Ҫ���͵�JSON���ݸ���
    // reader_callback: ������յ������ݿ�Ļص�����
    // bus: �ص�����ָ��
    Response post(const char* ipaddr,
        const int16_t port,
        const char* endpoint,
        json payload,
        const std::function<bool(std::string chunck, const CallbackBus* bus, CRichEditCtrl& edit)>& reader_callback,
        const CallbackBus* bus,
        CRichEditCtrl& edit);

    // ���ӵ�ָ��IP�Ͷ˿�
    SocketType  connectTo(const char* ipaddr, int16_t port);
    // ͨ���������ӷ�������
    bool        sendData(SocketType connection, const char* data);
    // �رո�������
    bool        closeConnection(SocketType connection);
    // ��ȡ������Ĵ�����
    int         getLastError();
    // ������Դ
    void        cleanUp();

    // ��������
    ~httpRequest();
};

#endif