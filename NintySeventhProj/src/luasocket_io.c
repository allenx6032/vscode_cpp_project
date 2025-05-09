/*=========================================================================*\
* Input/Output abstraction
* LuaSocket toolkit
*
* RCS ID: $Id: io.c,v 1.6 2005/09/29 06:11:41 diego Exp $
\*=========================================================================*/
#include "luasocket_io.h"

/*=========================================================================*\
* Exported functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Initializes C structure
\*-------------------------------------------------------------------------*/
void io_init(p_io io, p_send send, p_recv recv, p_error error, void *ctx) {
    io->send = send;
    io->recv = recv;
    io->error = error;
    io->ctx = ctx;
}

/*-------------------------------------------------------------------------*\
* I/O error strings
\*-------------------------------------------------------------------------*/
const char *io_strerror(int err) {
    switch (err) {
        case 0: return NULL;
        case -2: return "closed";
        case -1: return "timeout";
        default: return "unknown error"; 
    }
}
