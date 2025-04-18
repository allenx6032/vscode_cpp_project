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

#include "lua_socket_mgr.h"
#include "lua_socket_node.h"

bool lua_socket_mgr::setup(lua_State* L, uint32_t max_fd) {
	m_luakit = std::make_shared<kit_state>(L);
	m_mgr = std::make_shared<socket_mgr>();
	m_codec = m_luakit->create_codec();
	m_router = std::make_shared<socket_router>(m_mgr);
	return m_mgr->setup(max_fd);
}

int lua_socket_mgr::listen(lua_State* L, const char* ip, int port) {
	if (ip == nullptr || port <= 0) {
		return luakit::variadic_return(L, nullptr, "invalid param");
	}
	std::string err;
	eproto_type proto_type = (eproto_type)luaL_optinteger(L, 3, 0);
	auto token = m_mgr->listen(err, ip, port, proto_type);
	if (token == 0) {
		return luakit::variadic_return(L, nullptr, err);
	}

	auto listener = new lua_socket_node(token, m_luakit->L(), m_mgr, m_router, true, proto_type);
	if (proto_type == eproto_type::proto_rpc || proto_type == eproto_type::proto_pb) {
		listener->set_codec(m_codec);
	}
	return luakit::variadic_return(L, listener, "ok");
}

int lua_socket_mgr::connect(lua_State* L, const char* ip, const char* port, int timeout) {
	if (ip == nullptr || port == nullptr) {
		return luakit::variadic_return(L, nullptr, "invalid param");
	}

	std::string err;
	eproto_type proto_type = (eproto_type)luaL_optinteger(L, 4, 0);
	auto token = m_mgr->connect(err, ip, port, timeout, proto_type);
	if (token == 0) {
		return luakit::variadic_return(L, nullptr, err);
	}

	auto stream = new lua_socket_node(token, m_luakit->L(), m_mgr, m_router, false, proto_type);
	if (proto_type == eproto_type::proto_rpc || proto_type == eproto_type::proto_pb) {
		stream->set_codec(m_codec);
	}
	return luakit::variadic_return(L, stream, "ok");
}

int lua_socket_mgr::map_token(uint32_t node_id, uint32_t token,uint16_t hash) {
	return m_router->map_token(node_id, token, hash);
}

int lua_socket_mgr::set_node_status(uint32_t node_id, uint8_t status) {
	return m_router->set_node_status(node_id, status);
}

void lua_socket_mgr::map_router_node(uint32_t router_id, uint32_t target_id, uint8_t status) {
	return m_router->map_router_node(router_id,target_id,status);
}

void lua_socket_mgr::set_router_id(int id) {
	m_router->set_router_id(id);
}

void lua_socket_mgr::set_service_name(uint32_t service_id, std::string service_name) {
	m_router->set_service_name(service_id, service_name);
}

void lua_socket_mgr::set_rpc_key(std::string key) {
	m_mgr->set_handshake_verify(key);
}

const std::string lua_socket_mgr::get_rpc_key() {
	return m_mgr->get_handshake_verify();
}
int lua_socket_mgr::broad_group(lua_State* L, codec_base* codec) {
	size_t data_len = 0;
	std::vector<uint32_t> groups;
	if (!lua_to_native(L, 2, groups)) {
		lua_pushboolean(L, false);
		return 1;
	}
	char* data = (char*)codec->encode(L, 3, &data_len);
	if (data_len > 1 && data_len < NET_PACKET_MAX_LEN) {
		//发送数据
		m_mgr->broadgroup(groups, data, data_len);
		lua_pushboolean(L, true);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
int lua_socket_mgr::broad_rpc(lua_State* L) {
	if (m_codec) {
		std::vector<uint32_t> groups;
		if (!lua_to_native(L, 1, groups)) {
			lua_pushboolean(L, false);
			return 1;
		}
		uint8_t flag = lua_tointeger(L, 2);
		uint32_t source_id = lua_tointeger(L, 3);
		size_t data_len = 0;
		void* data = m_codec->encode(L, 4, &data_len);
		if (data_len <= SOCKET_PACKET_MAX) {
			router_header header;
			header.session_id = 0;
			header.rpc_flag = flag;
			header.source_id = source_id;
			header.msg_id = (uint8_t)rpc_type::remote_call;
			header.len = data_len + sizeof(router_header);
			sendv_item items[] = { {&header, sizeof(router_header)}, {data, data_len} };
			m_mgr->broadgroupv(groups, items, _countof(items));
			lua_pushboolean(L, true);
			return 1;
		}
	}
	lua_pushboolean(L, false);
	return 1;
}


//玩家路由
void lua_socket_mgr::set_player_service(uint32_t player_id, uint32_t sid, uint8_t login) {
	m_router->set_player_service(player_id, sid, login);
}
uint32_t lua_socket_mgr::find_player_sid(uint32_t player_id, uint16_t service_id) {
	return m_router->find_player_sid(player_id, service_id);
}
void lua_socket_mgr::clean_player_sid(uint32_t sid) {
	m_router->clean_player_sid(sid);
}