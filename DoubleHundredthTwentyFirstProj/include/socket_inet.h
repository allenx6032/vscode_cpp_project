#ifndef INET_H 
#define INET_H 
/*=========================================================================*\
* Internet domain functions
* LuaSocket toolkit
*
* This module implements the creation and connection of internet domain
* sockets, on top of the socket.h interface, and the interface of with the
* resolver. 
*
* The function inet_aton is provided for the platforms where it is not
* available. The module also implements the interface of the internet
* getpeername and getsockname functions as seen by Lua programs.
*
* The Lua functions toip and tohostname are also implemented here.
\*=========================================================================*/
#include "lua.h"
#include "socket.h"
#include "socket_timeout.h"

#ifdef _WIN32
#define INET_ATON
#endif

int inet_open(lua_State *L);

const char *inet_trycreate(p_socket ps, int family, int type);
const char *inet_tryconnect(p_socket ps, const char *address,
        const char *serv, p_timeout tm, struct addrinfo *connecthints);
const char *inet_trybind(p_socket ps, const char *address, const char *serv,
        struct addrinfo *bindhints);
const char *inet_trydisconnect(p_socket ps, int family, p_timeout tm);
const char *inet_tryaccept(p_socket server, int family, p_socket client, p_timeout tm);

int inet_meth_getpeername(lua_State *L, p_socket ps, int family);
int inet_meth_getsockname(lua_State *L, p_socket ps, int family);

int inet_optfamily(lua_State* L, int narg, const char* def);
int inet_optsocktype(lua_State* L, int narg, const char* def);

#ifdef INET_ATON
int inet_aton(const char *cp, struct in_addr *inp);
#endif

#ifndef _WINDOWS_
#define my_inet_ntop(a,b,c,d) inet_ntop(a,b,c,d)
#define my_inet_pton(a,b,c) inet_pton(a,b,c)
#else
int win32xp_inet_pton(int family, const char* string, PVOID dest);
char* win32xp_inet_ntop(int family, PVOID src, char* dest, size_t length);
#define my_inet_ntop(a,b,c,d) win32xp_inet_ntop(a,b,c,d)
#define my_inet_pton(a,b,c) win32xp_inet_pton(a,b,c)
#endif


#endif /* INET_H */
