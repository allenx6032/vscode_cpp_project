/* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/*
 * Vio Lite.
 * Purpose: include file for Vio that will work with C and C++
 */

#ifndef vio_violite_h_
#define	vio_violite_h_

#include "libmysql_my_net.h"			/* needed because of struct in_addr */


/* Simple vio interface in C;  The functions are implemented in violite.c */

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

enum enum_vio_type
{
  VIO_CLOSED, VIO_TYPE_TCPIP, VIO_TYPE_SOCKET, VIO_TYPE_NAMEDPIPE,
  VIO_TYPE_SSL, VIO_TYPE_SHARED_MEMORY
};


#define VIO_LOCALHOST 1                         /* a localhost connection */
#define VIO_BUFFERED_READ 2                     /* use buffered read */
#define VIO_READ_BUFFER_SIZE 16384              /* size of read buffer */

Vio*	vio_new(my_socket sd, enum enum_vio_type type, uint flags);
#ifdef __WIN__
Vio* vio_new_win32pipe(HANDLE hPipe);
Vio* vio_new_win32shared_memory(NET *net,HANDLE handle_file_map,
                                HANDLE handle_map,
                                HANDLE event_server_wrote,
                                HANDLE event_server_read,
                                HANDLE event_client_wrote,
                                HANDLE event_client_read,
                                HANDLE event_conn_closed);
size_t vio_read_pipe(Vio *vio, uchar * buf, size_t size);
size_t vio_write_pipe(Vio *vio, const uchar * buf, size_t size);
int vio_close_pipe(Vio * vio);
#else
#define HANDLE void *
#endif /* __WIN__ */

void	vio_delete(Vio* vio);
int	vio_close(Vio* vio);
void    vio_reset(Vio* vio, enum enum_vio_type type,
                  my_socket sd, HANDLE hPipe, uint flags);
size_t	vio_read(Vio *vio, uchar *	buf, size_t size);
size_t  vio_read_buff(Vio *vio, uchar * buf, size_t size);
size_t	vio_write(Vio *vio, const uchar * buf, size_t size);
int	vio_blocking(Vio *vio, my_bool onoff, my_bool *old_mode);
my_bool	vio_is_blocking(Vio *vio);
/* setsockopt TCP_NODELAY at IPPROTO_TCP level, when possible */
int	vio_fastsend(Vio *vio);
/* setsockopt SO_KEEPALIVE at SOL_SOCKET level, when possible */
int	vio_keepalive(Vio *vio, my_bool	onoff);
/* Whenever we should retry the last read/write operation. */
my_bool	vio_should_retry(Vio *vio);
/* Check that operation was timed out */
my_bool	vio_was_interrupted(Vio *vio);
/* Short text description of the socket for those, who are curious.. */
const char* vio_description(Vio *vio);
/* Return the type of the connection */
enum enum_vio_type vio_type(Vio* vio);
/* Return last error number */
int	vio_errno(Vio*vio);
/* Get socket number */
my_socket vio_fd(Vio*vio);
/* Remote peer's address and name in text form */
my_bool vio_peer_addr(Vio *vio, char *buf, uint16 *port, size_t buflen);
my_bool	vio_poll_read(Vio *vio,uint timeout);
my_bool vio_peek_read(Vio *vio, uint *bytes);
ssize_t vio_pending(Vio *vio);

#ifdef HAVE_OPENSSL
#include <yassl_opensslv.h>
#if OPENSSL_VERSION_NUMBER < 0x0090700f
#define DES_cblock des_cblock
#define DES_key_schedule des_key_schedule
#define DES_set_key_unchecked(k,ks) des_set_key_unchecked((k),*(ks))
#define DES_ede3_cbc_encrypt(i,o,l,k1,k2,k3,iv,e) des_ede3_cbc_encrypt((i),(o),(l),*(k1),*(k2),*(k3),(iv),(e))
#endif

#define HEADER_DES_LOCL_H dummy_something
#define YASSL_MYSQL_COMPATIBLE
#ifndef YASSL_PREFIX
#define YASSL_PREFIX
#endif
/* Set yaSSL to use same type as MySQL do for socket handles */
typedef my_socket YASSL_SOCKET_T;
#define YASSL_SOCKET_T_DEFINED
#include <yassl_ssl.h>
#include <yassl_err.h>

#ifndef EMBEDDED_LIBRARY

struct st_VioSSLFd
{
  SSL_CTX *ssl_context;
};

int sslaccept(struct st_VioSSLFd*, Vio *, long timeout);
int sslconnect(struct st_VioSSLFd*, Vio *, long timeout);

struct st_VioSSLFd
*new_VioSSLConnectorFd(const char *key_file, const char *cert_file,
		       const char *ca_file,  const char *ca_path,
		       const char *cipher);
struct st_VioSSLFd
*new_VioSSLAcceptorFd(const char *key_file, const char *cert_file,
		      const char *ca_file,const char *ca_path,
		      const char *cipher);
void free_vio_ssl_acceptor_fd(struct st_VioSSLFd *fd);
#endif /* ! EMBEDDED_LIBRARY */
#endif /* HAVE_OPENSSL */

#ifdef HAVE_SMEM
size_t vio_read_shared_memory(Vio *vio, uchar * buf, size_t size);
size_t vio_write_shared_memory(Vio *vio, const uchar * buf, size_t size);
int vio_close_shared_memory(Vio * vio);
#endif

void vio_end(void);

#ifdef	__cplusplus
}
#endif

#if !defined(DONT_MAP_VIO)
#define vio_delete(vio) 			(vio)->viodelete(vio)
#define vio_errno(vio)	 			(vio)->vioerrno(vio)
#define vio_read(vio, buf, size)                ((vio)->read)(vio,buf,size)
#define vio_write(vio, buf, size)               ((vio)->write)(vio, buf, size)
#define vio_blocking(vio, set_blocking_mode, old_mode)\
 	(vio)->vioblocking(vio, set_blocking_mode, old_mode)
#define vio_is_blocking(vio) 			(vio)->is_blocking(vio)
#define vio_fastsend(vio)			(vio)->fastsend(vio)
#define vio_keepalive(vio, set_keep_alive)	(vio)->viokeepalive(vio, set_keep_alive)
#define vio_should_retry(vio) 			(vio)->should_retry(vio)
#define vio_was_interrupted(vio) 		(vio)->was_interrupted(vio)
#define vio_close(vio)				((vio)->vioclose)(vio)
#define vio_peer_addr(vio, buf, prt, buflen)	(vio)->peer_addr(vio, buf, prt, buflen)
#define vio_timeout(vio, which, seconds)	(vio)->timeout(vio, which, seconds)
#endif /* !defined(DONT_MAP_VIO) */

/* This enumerator is used in parser - should be always visible */
enum SSL_type
{
  SSL_TYPE_NOT_SPECIFIED= -1,
  SSL_TYPE_NONE,
  SSL_TYPE_ANY,
  SSL_TYPE_X509,
  SSL_TYPE_SPECIFIED
};


/* HFTODO - hide this if we don't want client in embedded server */
/* This structure is for every connection on both sides */
struct st_vio
{
  my_socket		sd;		/* my_socket - real or imaginary */
  HANDLE hPipe;
  my_bool		localhost;	/* Are we from localhost? */
  int			fcntl_mode;	/* Buffered fcntl(sd,F_GETFL) */
  struct sockaddr_storage	local;		/* Local internet address */
  struct sockaddr_storage	remote;		/* Remote internet address */
  int addrLen;                          /* Length of remote address */
  enum enum_vio_type	type;		/* Type of connection */
  char			desc[30];	/* String description */
  char                  *read_buffer;   /* buffer for vio_read_buff */
  char                  *read_pos;      /* start of unfetched data in the
                                           read buffer */
  char                  *read_end;      /* end of unfetched data */
  /* function pointers. They are similar for socket/SSL/whatever */
  void    (*viodelete)(Vio*);
  int     (*vioerrno)(Vio*);
  size_t  (*read)(Vio*, uchar *, size_t);
  size_t  (*write)(Vio*, const uchar *, size_t);
  int     (*vioblocking)(Vio*, my_bool, my_bool *);
  my_bool (*is_blocking)(Vio*);
  int     (*viokeepalive)(Vio*, my_bool);
  int     (*fastsend)(Vio*);
  my_bool (*peer_addr)(Vio*, char *, uint16*, size_t);
  void    (*in_addr)(Vio*, struct sockaddr_storage*);
  my_bool (*should_retry)(Vio*);
  my_bool (*was_interrupted)(Vio*);
  int     (*vioclose)(Vio*);
  void	  (*timeout)(Vio*, unsigned int which, unsigned int timeout);
#ifdef HAVE_OPENSSL
  void	  *ssl_arg;
#endif
#ifdef HAVE_SMEM
  HANDLE  handle_file_map;
  char    *handle_map;
  HANDLE  event_server_wrote;
  HANDLE  event_server_read;
  HANDLE  event_client_wrote;
  HANDLE  event_client_read;
  HANDLE  event_conn_closed;
  size_t  shared_memory_remain;
  char    *shared_memory_pos;
  NET     *net;
#endif /* HAVE_SMEM */
};
#endif /* vio_violite_h_ */
