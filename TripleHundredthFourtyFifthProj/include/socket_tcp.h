#ifndef TCP_H
#define TCP_H
/*=========================================================================*\
* TCP object
* LuaSocket toolkit
*
* The tcp.h module is basicly a glue that puts together modules buffer.h,
* timeout.h socket.h and inet.h to provide the LuaSocket TCP (AF_INET,
* SOCK_STREAM) support.
*
* Three classes are defined: master, client and server. The master class is
* a newly created tcp object, that has not been bound or connected. Server
* objects are tcp objects bound to some local address. Client objects are
* tcp objects either connected to some address or returned by the accept
* method of a server object.
\*=========================================================================*/
#include "lua.h"

#include "socket_buffer.h"
#include "socket_timeout.h"
#include "socket_socket.h"

typedef struct t_tcp_ {
    t_socket sock;
    t_io io;
    t_buffer buf;
    t_timeout tm;
    int family;
    int asynConnStat;
} t_tcp;

typedef t_tcp *p_tcp;

int tcp_open(lua_State *L);

#endif /* TCP_H */
