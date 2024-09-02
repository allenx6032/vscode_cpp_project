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

#include "fmt/core.h"
#include "lua_socket_node.h"
#include "socket_helper.h"
#include <iostream>

lua_socket_node::lua_socket_node(uint32_t token, lua_State* L, stdsptr<socket_mgr>& mgr,
	stdsptr<socket_router> router, bool blisten, eproto_type proto_type)
	: m_token(token), m_mgr(mgr), m_router(router), m_proto_type(proto_type) {
	m_mgr->get_remote_ip(m_token, m_ip);
	m_luakit = std::make_shared<luakit::kit_state>(L);
	if (blisten) {
		m_mgr->set_accept_callback(token, [=](uint32_t steam_token, eproto_type proto_type) {
			auto stream = new lua_socket_node(steam_token, m_luakit->L(), m_mgr, m_router, false, proto_type);
			stream->set_codec(m_codec);
			m_luakit->object_call(this, "on_accept", nullptr, std::tie(), stream);
			});
	}

	m_mgr->set_connect_callback(token, [=](bool ok, const char* reason) {
		if (ok) {
			m_mgr->get_remote_ip(m_token, m_ip);
		}
		m_luakit->object_call(this, "on_connect", nullptr, std::tie(), ok ? "ok" : reason);
		if (!ok) {
			this->m_token = 0;
		}
		});

	m_mgr->set_error_callback(token, [=](const char* err) {
		auto token = m_token;
		m_token = 0;
		m_luakit->object_call(this, "on_error", nullptr, std::tie(), token, err);
		});

	m_mgr->set_package_callback(token, [=](slice* data) {
		return on_recv(data);
		});
}

lua_socket_node::~lua_socket_node() {
	close();
}

int lua_socket_node::call_pb(lua_State* L) {
	int top = lua_gettop(L);
	if (top < 4) {
		lua_pushinteger(L, -1);
		return 1;
	}
	if (m_codec) {
		size_t data_len = 0;
		char* data = (char*)m_codec->encode(L, 1, &data_len);
		if (data_len > 1 && data_len < NET_PACKET_MAX_LEN) {
			m_mgr->send(m_token, data, data_len);
			lua_pushinteger(L, data_len);
			return 1;
		} else {			
			lua_pushinteger(L, -2);
			return 1;			
		}
	}
	lua_pushinteger(L, -1);
	return 1;
}

int lua_socket_node::call_text(lua_State* L) {
	size_t data_len = 0;
	const char* data_ptr = lua_tolstring(L, 1, &data_len);
	auto send_len = m_mgr->send(m_token, data_ptr, data_len);
	lua_pushinteger(L, send_len);
	return 1;
}

int lua_socket_node::call_data(lua_State* L) {
	if (m_codec) {
		size_t data_len = 0;
		char* data = (char*)m_codec->encode(L, 1, &data_len);
		if (data_len < SOCKET_PACKET_MAX) {
			m_mgr->send(m_token, data, data_len);
			lua_pushinteger(L, data_len);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}

int lua_socket_node::call(lua_State* L, uint32_t session_id, uint8_t flag, uint32_t source_id) {
	if (m_codec) {
		size_t data_len = 0;
		void* data = m_codec->encode(L, 4, &data_len);
		if (data_len <= SOCKET_PACKET_MAX) {
			router_header header;
			header.session_id = session_id;
			header.rpc_flag = flag;
			header.source_id = source_id;
			header.msg_id = (uint8_t)rpc_type::remote_call;
			header.len = data_len + sizeof(router_header);
			sendv_item items[] = { {&header, sizeof(router_header)}, {data, data_len} };
			auto send_len = m_mgr->sendv(m_token, items, _countof(items));
			lua_pushinteger(L, send_len);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}

int lua_socket_node::forward_target(lua_State* L, uint32_t session_id, uint8_t flag, uint32_t source_id, uint32_t target) {
	if (m_codec) {
		size_t data_len = 0;
		void* data = m_codec->encode(L, 5, &data_len);
		if (data_len <= SOCKET_PACKET_MAX) {
			router_header header;
			header.session_id = session_id;
			header.rpc_flag = flag;
			header.source_id = source_id;
			header.msg_id = (uint8_t)rpc_type::forward_target;
			header.target_sid = target;
			header.len = data_len + sizeof(router_header);
			sendv_item items[] = { {&header, sizeof(router_header)}, {data, data_len} };
			auto send_len = m_mgr->sendv(m_token, items, _countof(items));
			lua_pushinteger(L, send_len);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}

int lua_socket_node::forward_player(lua_State* L, uint32_t session_id, uint8_t flag, uint32_t source_id, uint16_t service_id, uint32_t player_id) {
	if (m_codec) {
		size_t data_len = 0;
		void* data = m_codec->encode(L, 6, &data_len);
		if (data_len <= SOCKET_PACKET_MAX) {
			router_header header;
			header.session_id = session_id;
			header.rpc_flag = flag;
			header.source_id = source_id;
			header.msg_id = (uint8_t)rpc_type::forward_player;
			header.target_sid = service_id;
			header.target_pid = player_id;
			header.len = data_len + sizeof(router_header);
			sendv_item items[] = { {&header, sizeof(router_header)}, {data, data_len} };
			auto send_len = m_mgr->sendv(m_token, items, _countof(items));
			lua_pushinteger(L, send_len);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}

int lua_socket_node::forward_hash(lua_State* L, uint32_t session_id, uint8_t flag, uint32_t source_id, uint16_t service_id, uint16_t hash) {
	if (m_codec) {
		size_t data_len = 0;
		void* data = m_codec->encode(L, 6, &data_len);
		if (data_len <= SOCKET_PACKET_MAX) {
			router_header header;
			header.session_id = session_id;
			header.rpc_flag = flag;
			header.source_id = source_id;
			header.msg_id = (uint8_t)rpc_type::forward_hash;
			header.target_sid = service_id;
			header.target_pid = hash;
			header.len = data_len + sizeof(router_header);
			sendv_item items[] = { {&header, sizeof(router_header)}, {data, data_len} };
			auto send_len = m_mgr->sendv(m_token, items, _countof(items));
			lua_pushinteger(L, send_len);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}

void lua_socket_node::close() {
	if (m_token != 0) {
		m_mgr->close(m_token);
		m_token = 0;
	}
}

int lua_socket_node::on_recv(slice* slice) {
	if (eproto_type::proto_pb == m_proto_type) {
		return on_call_pb(slice);
	}
	if (eproto_type::proto_text == m_proto_type) {
		return on_call_data(slice);
	}
	size_t data_len;
	size_t header_len = sizeof(router_header);
	auto hdata = slice->peek(header_len);
	router_header* header = (router_header*)hdata;

	slice->erase(header_len);
	m_error_msg = "";
	bool is_router = false;
	auto msg = header->msg_id;
	if (msg >= (uint8_t)(rpc_type::forward_router)) {
		msg -= (uint8_t)rpc_type::forward_router;
		is_router = true;
	}
	auto data = (char*)slice->data(&data_len);
	switch ((rpc_type)msg) {
	case rpc_type::remote_call:
		on_call(header, slice);
		break;
	case rpc_type::forward_target:
		if (!m_router->do_forward_target(header, data, data_len, m_error_msg,is_router))
			on_forward_error(header);
		break;
	case rpc_type::forward_master:
		if (!m_router->do_forward_master(header, data, data_len, m_error_msg,is_router))
			on_forward_error(header);
		break;
	case rpc_type::forward_hash:
		if (!m_router->do_forward_hash(header, data, data_len, m_error_msg,is_router))
			on_forward_error(header);
		break;
	case rpc_type::forward_player:
		if (!m_router->do_forward_player(header, data, data_len, m_error_msg, is_router))
			on_forward_error(header);
		break;
	case rpc_type::forward_broadcast:
		{
			size_t broadcast_num = 0;
			if (m_router->do_forward_broadcast(header, m_token, data, data_len, broadcast_num))
				on_forward_broadcast(header, broadcast_num);
			else
				on_forward_error(header);
		}
		break;
	default:
		break;
	}
	return 0;
}

void lua_socket_node::on_forward_error(router_header* header) {
	if (header->session_id > 0) {
		m_luakit->object_call(this, "on_forward_error", nullptr, std::tie(), header->session_id, m_error_msg, header->source_id, header->msg_id);
	}
}

void lua_socket_node::on_forward_broadcast(router_header* header, size_t broadcast_num) {
	if (header->session_id > 0) {
		m_luakit->object_call(this, "on_forward_broadcast", nullptr, std::tie(), header->session_id, broadcast_num);
	}
}

void lua_socket_node::on_call(router_header* header, slice* slice) {
	m_codec->set_slice(slice);
	m_luakit->object_call(this, "on_call", nullptr, m_codec, std::tie(), slice->size(), header->session_id, header->rpc_flag, header->source_id);
}

int lua_socket_node::on_call_pb(slice* slice) {
	int iRet = 0;
	m_luakit->object_call(this, "on_call_pb", nullptr, m_codec, std::tie(iRet));
	return iRet;
}

int lua_socket_node::on_call_data(slice* slice) {
	if (m_codec) {
		size_t buf_size = slice->size();
		m_luakit->object_call(this, "on_call_data", nullptr, m_codec, std::tie(), buf_size);
	} else {
		m_luakit->object_call(this, "on_call_text", nullptr, std::tie(), slice->size(), slice->contents());
	}
	return 0;
}

