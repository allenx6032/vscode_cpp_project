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


#include "socket_dns.h"
#include "socket_udp.h"
#include "socket_tcp.h"
#include "lua_socket_mgr.h"
#include "lua_socket_node.h"

namespace luabus {
    thread_local lua_socket_mgr socket_mgr;

	static bool init_socket_mgr(lua_State* L, uint32_t max_fd) {
        return socket_mgr.setup(L, max_fd);
	}

	static socket_udp* create_udp() {
		socket_udp* udp = new socket_udp();
		if (!udp->setup()) {
			delete udp;
			return nullptr;
		}
		return udp;
	}

	static socket_tcp* create_tcp() {
		socket_tcp* tcp = new socket_tcp();
		if (!tcp->setup()) {
			delete tcp;
			return nullptr;
		}
		return tcp;
	}

    //管理器接口
    static int listen(lua_State* L, const char* ip, int port) {
        return socket_mgr.listen(L, ip, port);
    }
    static int connect(lua_State* L, const char* ip, const char* port, int timeout) {
        return socket_mgr.connect(L, ip, port, timeout);
    }

    luakit::lua_table open_luabus(lua_State* L) {
        luakit::kit_state kit_state(L);
        auto lluabus = kit_state.new_table();

        lluabus.set_function("udp", create_udp);
        lluabus.set_function("tcp", create_tcp);
        lluabus.set_function("host", gethostip);
        lluabus.set_function("dns", gethostbydomain);
        lluabus.set_function("init_socket_mgr", init_socket_mgr);
        lluabus.set_function("port_is_used", port_is_used);

        //管理器接口
        lluabus.set_function("wait", [](int64_t now, int ms) { return socket_mgr.wait(now,ms); });
        lluabus.set_function("listen", listen);
        lluabus.set_function("connect", connect);
        lluabus.set_function("map_token", [](uint32_t node_id, uint32_t token, uint16_t hash) { return socket_mgr.map_token(node_id, token, hash); });
        lluabus.set_function("set_node_status", [](uint32_t node_id, uint8_t status) { return socket_mgr.set_node_status(node_id, status); });
        lluabus.set_function("map_router_node", [](uint32_t router_id, uint32_t target_id, uint8_t status) { return socket_mgr.map_router_node(router_id, target_id, status); });
        lluabus.set_function("set_router_id", [](int id) { return socket_mgr.set_router_id(id); });
        lluabus.set_function("set_rpc_key", [](std::string key) { return socket_mgr.set_rpc_key(key); });
        lluabus.set_function("get_rpc_key", []() { return socket_mgr.get_rpc_key(); });
        lluabus.set_function("broad_group", [](lua_State* L, codec_base* codec) { return socket_mgr.broad_group(L,codec); });
        lluabus.set_function("broad_rpc", [](lua_State* L) { return socket_mgr.broad_rpc(L); });
        lluabus.set_function("set_service_name", [](uint32_t service_id, std::string service_name) { return socket_mgr.set_service_name(service_id,service_name); });
        lluabus.set_function("set_player_service", [](uint32_t player_id, uint32_t sid, uint8_t login) { return socket_mgr.set_player_service(player_id, sid,login); });
        lluabus.set_function("find_player_sid", [](uint32_t player_id, uint16_t service_id) { return socket_mgr.find_player_sid(player_id, service_id); });
        lluabus.set_function("clean_player_sid", [](uint32_t sid) { return socket_mgr.clean_player_sid(sid); });

        lluabus.new_enum("eproto_type",
            "rpc", eproto_type::proto_rpc,
            "pb", eproto_type::proto_pb,
            "text", eproto_type::proto_text
        );
        kit_state.new_class<socket_udp>(
            "send", &socket_udp::send,
            "recv", &socket_udp::recv,
            "close", &socket_udp::close,
            "listen", &socket_udp::listen
            );
        kit_state.new_class<socket_tcp>(
            "send", &socket_tcp::send,
            "recv", &socket_tcp::recv,
            "close", &socket_tcp::close,
            "accept", &socket_tcp::accept,
            "listen", &socket_tcp::listen,
            "invalid", &socket_tcp::invalid,
            "connect", &socket_tcp::connect
            );
        kit_state.new_class<lua_socket_node>(
            "ip", &lua_socket_node::m_ip,
            "token", &lua_socket_node::m_token,
            "call", &lua_socket_node::call,
            "call_pb",&lua_socket_node::call_pb,
            "call_text",&lua_socket_node::call_text,
            "call_data", &lua_socket_node::call_data,
            "forward_hash", &lua_socket_node::forward_hash,
            "forward_player",&lua_socket_node::forward_player,
            "forward_target", &lua_socket_node::forward_target,
            "forward_master", &lua_socket_node::forward_by_group<rpc_type::forward_master>,
            "forward_broadcast", &lua_socket_node::forward_by_group < rpc_type::forward_broadcast>,
            "close", &lua_socket_node::close,
            "set_nodelay", &lua_socket_node::set_nodelay,
            "set_timeout", &lua_socket_node::set_timeout,
            "set_codec", &lua_socket_node::set_codec,
            "set_flow_ctrl",&lua_socket_node::set_flow_ctrl,
            "can_send",&lua_socket_node::can_send,
            "is_command_cd",&lua_socket_node::is_command_cd
            );
        return lluabus;
    }
}

extern "C" {
	LUALIB_API int luaopen_luabus(lua_State* L) {
		auto lluabus = luabus::open_luabus(L);
		return lluabus.push_stack();
	}
}
