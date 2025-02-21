#include "pch.h"
#include "minipost.h"
// 用于抑制Winsock相关的弃用警告，不过这里没实际启用相关宏
//_WINSOCK_DEPRECATED_NO_WARNINGS

// httpRequest类的构造函数
httpRequest::httpRequest() {
    // 初始化Winsock库
    WSADATA wsaData;
    // WSAStartup函数用于启动Winsock，参数 MAKEWORD(2, 2)表示使用版本2.2
    // 如果初始化失败，记录错误日志
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logging::error("Error to initialize winsock library.");
    }
}

// 关闭给定的socket连接
bool httpRequest::closeConnection(SocketType connection) {
    // 使用closesocket函数关闭连接
    closesocket(connection);
    // 记录连接已关闭的日志
    Logging::log("Conection closed");
    return true;
}

// 通过给定的socket连接发送数据
bool httpRequest::sendData(SocketType connection, const char* data) {
    // 使用send函数发送数据，参数分别为连接、数据指针、数据长度、发送标志
    if (send(connection, data, strlen(data), 0) == Socket_error) {
        // 如果发送出错，记录错误日志
        Logging::error("send() SOCKET ERROR");
        // 获取并处理最后一个错误
        getLastError();
        // 关闭连接
        this->closeConnection(connection);
        return 1;
    }
    else {
        return true;
    }
}

Response httpRequest::post(const char* ipaddr, const int16_t port, const char* endpoint, json payload, const std::function<bool(std::string chunck, const CallbackBus* bus, CRichEditCtrl& edit)>& reader_callback, const CallbackBus* bus, CRichEditCtrl& edit)
{
    Response response;
    // 记录连接信息日志
    Logging::log("Connecting to %s to port %d", ipaddr, port);
    // 记录要发送的数据日志
    Logging::log("Sending:", payload.c_str());
    // 尝试连接到指定IP和端口
    SocketType connection = connectTo(ipaddr, port);
    // 记录socket状态日志
    Logging::log("Socket status: %d", connection);

    // 记录发送POST请求的日志
    Logging::log("Sending POST request");

    // 构建HTTP POST请求体
    std::string request_body = "POST " + std::string(endpoint) + " HTTP/1.1\r\n";   // POST请求行
    request_body += "Host: \r\n";                                                   // 主机名
    request_body += "Accept: text/event-stream\r\n";                                // 接受的数据类型
    request_body += "Content-Type: application/json\r\n";                           // 请求体类型
    request_body += "Content-Length: " + std::to_string(payload.size()) + "\r\n";   // 请求体长度
    request_body += "Connection: keep-alive\r\n";                                   // 保持连接
    request_body += "Sec-Fetch-Dest: empty\r\n";                                    // 请求头
    request_body += "Sec-Fetch-Mode: cors\r\n";                                     // 请求头
    request_body += "\r\n";                                                         // 空行
    request_body += payload;    										            // 请求体                                  

    // 记录完整的请求体日志
    Logging::log(request_body.c_str());

    // 如果发送请求失败
    if (!this->sendData(connection, request_body.c_str())) {
        std::cout << "Error sending request." << std::endl;
        response.Status = 400;
        return response;
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;
    bool streaming = true;
    // 记录等待接收响应的日志
    Logging::log("Waiting recv() answer");

    std::string accumulatedBuffer;

    // 循环接收数据
    while (streaming && (bytesRead = recv(connection, buffer, sizeof(buffer), 0)) > 0) {
        if (bytesRead == 0)
            break;
        // 将接收到的数据追加到缓冲区
        accumulatedBuffer.append(buffer, bytesRead);

        // 查找HTTP响应头中的状态码
        size_t httpPos = accumulatedBuffer.find("HTTP");
        if (httpPos != std::string::npos) {
            response.Status = std::stoi(accumulatedBuffer.substr(9, 3));
        };

        // 逐行处理接收到的数据
        size_t lineEndPos;
        while ((lineEndPos = accumulatedBuffer.find('\n')) != std::string::npos) {
            std::string line = accumulatedBuffer.substr(0, lineEndPos);
            accumulatedBuffer.erase(0, lineEndPos + 1);
            // 如果有回调函数，调用回调处理数据行
            if (reader_callback != nullptr) {
                if (!reader_callback(line, bus, edit))
                    streaming = false;
            }
        }

        // 如果还有剩余数据且仍在流式处理
        if (!accumulatedBuffer.empty() && streaming) {
            if (reader_callback != nullptr) {
                reader_callback(accumulatedBuffer, bus, edit);
            }
        }
    };

    // 如果接收数据出错
    if (bytesRead == Socket_error)
        MessageBox(NULL, TEXT("Error receiving data."), TEXT("Error"), MB_ICONERROR);
    Logging::error("Error receiving data.");

    // 关闭连接
    closeConnection(connection);
    return response;
}

// 连接到指定的IP地址和端口
SocketType httpRequest::connectTo(const char* ipaddr, int16_t port) {
    WSADATA wsaData;
    // 初始化Winsock库
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logging::error("Error initializing winsock.");
        return 1;
    }

    // 创建一个TCP socket
    SocketType clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == Invalid_socket) {
        // 如果创建socket失败，记录错误日志并清理资源
        Logging::error("Error to create a socket.");
        cleanUp();
        return 1;
    }

    sockaddr_in serverAddress;
    // 设置地址族为IPv4
    serverAddress.sin_family = AF_INET;
    // 设置端口号
    serverAddress.sin_port = htons(port);
    // 设置IP地址
    serverAddress.sin_addr.s_addr = inet_addr(ipaddr);

    // 尝试连接到服务器
    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == Socket_error) {
        Logging::error("Error connecting to the server.");
        getLastError();
        closeConnection(clientSocket);
        return -1;
    }
    // 记录socket创建成功的日志
    Logging::log("Socket created.");
    return clientSocket;
}

// 获取并记录最后一个Winsock错误
int httpRequest::getLastError() {
    int errorCode = WSAGetLastError();
    if (errorCode != 0) {
        wchar_t errorBuffer[256];
        //char errorBuffer[256];
        // 使用FormatMessage获取错误描述
        if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, errorBuffer, sizeof(errorBuffer), NULL) != 0) {
            Logging::error("Error in Winsock:\nCode: %d \nMsg:%s", errorCode, errorBuffer);
        }
        else {
            Logging::error("Error in Winsock with Code %d (Not possible get code description).", errorCode);
        }
    }
    return errorCode;
}

// 清理Winsock资源
void httpRequest::cleanUp() {
    WSACleanup();
}

// httpRequest类的析构函数，清理Winsock资源
httpRequest::~httpRequest() {
    cleanUp();
}