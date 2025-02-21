#include "pch.h"
#include "minipost.h"
// ��������Winsock��ص����þ��棬��������ûʵ��������غ�
//_WINSOCK_DEPRECATED_NO_WARNINGS

// httpRequest��Ĺ��캯��
httpRequest::httpRequest() {
    // ��ʼ��Winsock��
    WSADATA wsaData;
    // WSAStartup������������Winsock������ MAKEWORD(2, 2)��ʾʹ�ð汾2.2
    // �����ʼ��ʧ�ܣ���¼������־
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logging::error("Error to initialize winsock library.");
    }
}

// �رո�����socket����
bool httpRequest::closeConnection(SocketType connection) {
    // ʹ��closesocket�����ر�����
    closesocket(connection);
    // ��¼�����ѹرյ���־
    Logging::log("Conection closed");
    return true;
}

// ͨ��������socket���ӷ�������
bool httpRequest::sendData(SocketType connection, const char* data) {
    // ʹ��send�����������ݣ������ֱ�Ϊ���ӡ�����ָ�롢���ݳ��ȡ����ͱ�־
    if (send(connection, data, strlen(data), 0) == Socket_error) {
        // ������ͳ�����¼������־
        Logging::error("send() SOCKET ERROR");
        // ��ȡ���������һ������
        getLastError();
        // �ر�����
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
    // ��¼������Ϣ��־
    Logging::log("Connecting to %s to port %d", ipaddr, port);
    // ��¼Ҫ���͵�������־
    Logging::log("Sending:", payload.c_str());
    // �������ӵ�ָ��IP�Ͷ˿�
    SocketType connection = connectTo(ipaddr, port);
    // ��¼socket״̬��־
    Logging::log("Socket status: %d", connection);

    // ��¼����POST�������־
    Logging::log("Sending POST request");

    // ����HTTP POST������
    std::string request_body = "POST " + std::string(endpoint) + " HTTP/1.1\r\n";   // POST������
    request_body += "Host: \r\n";                                                   // ������
    request_body += "Accept: text/event-stream\r\n";                                // ���ܵ���������
    request_body += "Content-Type: application/json\r\n";                           // ����������
    request_body += "Content-Length: " + std::to_string(payload.size()) + "\r\n";   // �����峤��
    request_body += "Connection: keep-alive\r\n";                                   // ��������
    request_body += "Sec-Fetch-Dest: empty\r\n";                                    // ����ͷ
    request_body += "Sec-Fetch-Mode: cors\r\n";                                     // ����ͷ
    request_body += "\r\n";                                                         // ����
    request_body += payload;    										            // ������                                  

    // ��¼��������������־
    Logging::log(request_body.c_str());

    // �����������ʧ��
    if (!this->sendData(connection, request_body.c_str())) {
        std::cout << "Error sending request." << std::endl;
        response.Status = 400;
        return response;
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;
    bool streaming = true;
    // ��¼�ȴ�������Ӧ����־
    Logging::log("Waiting recv() answer");

    std::string accumulatedBuffer;

    // ѭ����������
    while (streaming && (bytesRead = recv(connection, buffer, sizeof(buffer), 0)) > 0) {
        if (bytesRead == 0)
            break;
        // �����յ�������׷�ӵ�������
        accumulatedBuffer.append(buffer, bytesRead);

        // ����HTTP��Ӧͷ�е�״̬��
        size_t httpPos = accumulatedBuffer.find("HTTP");
        if (httpPos != std::string::npos) {
            response.Status = std::stoi(accumulatedBuffer.substr(9, 3));
        };

        // ���д�����յ�������
        size_t lineEndPos;
        while ((lineEndPos = accumulatedBuffer.find('\n')) != std::string::npos) {
            std::string line = accumulatedBuffer.substr(0, lineEndPos);
            accumulatedBuffer.erase(0, lineEndPos + 1);
            // ����лص����������ûص�����������
            if (reader_callback != nullptr) {
                if (!reader_callback(line, bus, edit))
                    streaming = false;
            }
        }

        // �������ʣ��������������ʽ����
        if (!accumulatedBuffer.empty() && streaming) {
            if (reader_callback != nullptr) {
                reader_callback(accumulatedBuffer, bus, edit);
            }
        }
    };

    // ����������ݳ���
    if (bytesRead == Socket_error)
        MessageBox(NULL, TEXT("Error receiving data."), TEXT("Error"), MB_ICONERROR);
    Logging::error("Error receiving data.");

    // �ر�����
    closeConnection(connection);
    return response;
}

// ���ӵ�ָ����IP��ַ�Ͷ˿�
SocketType httpRequest::connectTo(const char* ipaddr, int16_t port) {
    WSADATA wsaData;
    // ��ʼ��Winsock��
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logging::error("Error initializing winsock.");
        return 1;
    }

    // ����һ��TCP socket
    SocketType clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == Invalid_socket) {
        // �������socketʧ�ܣ���¼������־��������Դ
        Logging::error("Error to create a socket.");
        cleanUp();
        return 1;
    }

    sockaddr_in serverAddress;
    // ���õ�ַ��ΪIPv4
    serverAddress.sin_family = AF_INET;
    // ���ö˿ں�
    serverAddress.sin_port = htons(port);
    // ����IP��ַ
    serverAddress.sin_addr.s_addr = inet_addr(ipaddr);

    // �������ӵ�������
    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == Socket_error) {
        Logging::error("Error connecting to the server.");
        getLastError();
        closeConnection(clientSocket);
        return -1;
    }
    // ��¼socket�����ɹ�����־
    Logging::log("Socket created.");
    return clientSocket;
}

// ��ȡ����¼���һ��Winsock����
int httpRequest::getLastError() {
    int errorCode = WSAGetLastError();
    if (errorCode != 0) {
        wchar_t errorBuffer[256];
        //char errorBuffer[256];
        // ʹ��FormatMessage��ȡ��������
        if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, errorBuffer, sizeof(errorBuffer), NULL) != 0) {
            Logging::error("Error in Winsock:\nCode: %d \nMsg:%s", errorCode, errorBuffer);
        }
        else {
            Logging::error("Error in Winsock with Code %d (Not possible get code description).", errorCode);
        }
    }
    return errorCode;
}

// ����Winsock��Դ
void httpRequest::cleanUp() {
    WSACleanup();
}

// httpRequest�����������������Winsock��Դ
httpRequest::~httpRequest() {
    cleanUp();
}