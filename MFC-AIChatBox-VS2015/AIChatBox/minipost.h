#ifndef __MINIPOST_H__
#define __MINIPOST_H__

#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <winsock2.h>
#include <windows.h>

// 引入日志和终端相关的头文件
#include "logging.h"
//#include "terminal.h"

// 给SOCKET类型起个别名，方便后续代码书写
typedef SOCKET SocketType;
// 给SOCKET_ERROR起个别名，用于表示socket相关错误
#define Socket_error SOCKET_ERROR  
// 给INVALID_SOCKET起个别名，用于表示无效的socket
#define Invalid_socket INVALID_SOCKET  

// 定义缓冲区大小为100KB
#define BUFFER_SIZE 100000  
// 定义HTTP内部错误码
#define HTTP_INTERNAL_ERROR 500  
// 定义HTTP成功状态码
#define HTTP_OK 200  

// 给std::string起个别名，用于表示JSON数据类型
typedef std::string json;

// 定义响应结构体，包含状态码和响应体
struct Response {
    int16_t Status = -1;
    std::string body;
};

// 定义回调总线结构体，用于处理数据回调
struct CallbackBus {
    // 虚析构函数，确保派生类能正确析构
    virtual ~CallbackBus() = default;
    // 用于存储接收到的数据缓冲区
    std::string buffer;
    // 标记是否为流式数据
    bool stream;
};

// HTTP请求类
class httpRequest {
public:
    // 默认构造函数
    httpRequest();
    // 发起POST请求，支持流式数据回调读取
    // ipaddr: 目标IP地址
    // port: 目标端口号
    // endpoint: 请求的端点路径
    // payload: 要发送的JSON数据负载
    // reader_callback: 处理接收到的数据块的回调函数
    // bus: 回调总线指针
    Response post(const char* ipaddr,
        const int16_t port,
        const char* endpoint,
        json payload,
        const std::function<bool(std::string chunck, const CallbackBus* bus, CRichEditCtrl& edit)>& reader_callback,
        const CallbackBus* bus,
        CRichEditCtrl& edit);

    // 连接到指定IP和端口
    SocketType  connectTo(const char* ipaddr, int16_t port);
    // 通过给定连接发送数据
    bool        sendData(SocketType connection, const char* data);
    // 关闭给定连接
    bool        closeConnection(SocketType connection);
    // 获取最后发生的错误码
    int         getLastError();
    // 清理资源
    void        cleanUp();

    // 析构函数
    ~httpRequest();
};

#endif