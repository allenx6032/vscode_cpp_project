// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef WIN32

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0603		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0603	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0510 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0A00	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>

#endif

// TODO: reference additional headers your program requires here
#pragma warning(disable: 4996)
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4819)
#pragma warning(disable: 4313)
#pragma warning(disable: 4251)

#include <string>
#include <exception>
#include <assert.h>

#ifdef __linux
#include <sys/epoll.h>
#endif
#ifdef __APPLE__
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif
#if defined(__linux) || defined(__APPLE__)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include <iostream>
#include "lua_kit.h"

#include "socket_udp.h"

socket_udp::~socket_udp() {
    close();
}

void socket_udp::close() {
    if (m_fd > 0) {
        closesocket(m_fd);
        m_fd = INVALID_SOCKET;
    }
}

bool socket_udp::setup() {
    socket_t fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd <= 0) {
        return false;
    }
    m_fd = fd;
    init_socket_option(fd);
    return true;
}

int socket_udp::listen(lua_State* L, const char* ip, int port) {
    size_t addr_len = 0;
    sockaddr_storage addr;
    make_ip_addr(&addr, &addr_len, ip, port);
    if (::bind(m_fd, (sockaddr*)&addr, (int)addr_len) == SOCKET_ERROR) {
        lua_pushboolean(L, false);
        lua_pushstring(L, "udp bind failed!");
        return 2;
    }
    lua_pushboolean(L, true);
    return 1;
}

int socket_udp::send(lua_State* L, const char* buf, size_t len, const char* ip, int port) {
    size_t addr_len = 0;
    sockaddr_storage addr;
    make_ip_addr(&addr, &addr_len, ip, port);
    int send_len = sendto(m_fd, buf, len, 0, (sockaddr*)&addr, sizeof(sockaddr));
    if (send_len == SOCKET_ERROR) {
        lua_pushboolean(L, false);
        lua_pushstring(L, "send failed!");
        lua_pushinteger(L, get_socket_error());
        return 3;
    }
    lua_pushboolean(L, true);
    return 1;
}

int socket_udp::recv(lua_State* L) {
    sockaddr_in addr;
    socklen_t addr_len = (socklen_t)sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    memset(m_recv_buf, 0, SOCKET_RECV_LEN);
    int recv_len = recvfrom(m_fd, m_recv_buf, SOCKET_RECV_LEN, 0, (sockaddr*)&addr, &addr_len);
    if (recv_len == SOCKET_ERROR) {
        lua_pushboolean(L, false);
        if (get_socket_error() == WSAEWOULDBLOCK) {
            lua_pushstring(L, "EWOULDBLOCK");
        }
        else {
            lua_pushstring(L, "recv failed");
        }
        return 2;
    }
    lua_pushboolean(L, true);
    lua_pushlstring(L, m_recv_buf, recv_len);
    lua_pushstring(L, inet_ntoa(addr.sin_addr));
    lua_pushinteger(L, ntohs(addr.sin_port));
    return 4;
}
