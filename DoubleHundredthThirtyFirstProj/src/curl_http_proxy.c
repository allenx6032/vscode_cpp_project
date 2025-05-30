/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "curl_setup.h"

#include "curl_http_proxy.h"

#if !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#include <curl.h>
#include "curl_sendf.h"
#include "http.h"
#include "curl_url.h"
#include "curl_select.h"
#include "curl_progress.h"
#include "curl_non-ascii.h"
#include "curl_connect.h"
#include "curlx.h"
#include "curl_vtls.h"

/* The last 3 #include files should be in this order */
#include "curl_printf.h"
#include "curl_memory.h"
#include "curl_memdebug.h"

/*
 * Perform SSL initialization for HTTPS proxy.  Sets
 * proxy_ssl_connected connection bit when complete.  Can be
 * called multiple times.
 */
static CURLcode https_proxy_connect(struct connectdata *conn, int sockindex)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  DEBUGASSERT(conn->http_proxy.proxytype == CURLPROXY_HTTPS);
  if(!conn->bits.proxy_ssl_connected[sockindex]) {
    /* perform SSL initialization for this socket */
    result =
      Curl_ssl_connect_nonblocking(conn, sockindex,
                                   &conn->bits.proxy_ssl_connected[sockindex]);
    if(result)
      /* a failed connection is marked for closure to prevent (bad) re-use or
         similar */
      connclose(conn, "TLS handshake failed");
  }
  return result;
#else
  (void) conn;
  (void) sockindex;
  return CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_proxy_connect(struct connectdata *conn, int sockindex)
{
  struct Curl_easy *data = conn->data;
  if(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    const CURLcode result = https_proxy_connect(conn, sockindex);
    if(result)
      return result;
    if(!conn->bits.proxy_ssl_connected[sockindex])
      return result; /* wait for HTTPS proxy SSL initialization to complete */
  }

  if(conn->bits.tunnel_proxy && conn->bits.httpproxy) {
#ifndef CURL_DISABLE_PROXY
    /* for [protocol] tunneled through HTTP proxy */
    struct HTTP http_proxy;
    void *prot_save;
    const char *hostname;
    int remote_port;
    CURLcode result;

    /* BLOCKING */
    /* We want "seamless" operations through HTTP proxy tunnel */

    /* Curl_proxyCONNECT is based on a pointer to a struct HTTP at the
     * member conn->proto.http; we want [protocol] through HTTP and we have
     * to change the member temporarily for connecting to the HTTP
     * proxy. After Curl_proxyCONNECT we have to set back the member to the
     * original pointer
     *
     * This function might be called several times in the multi interface case
     * if the proxy's CONNECT response is not instant.
     */
    prot_save = conn->data->req.protop;
    memset(&http_proxy, 0, sizeof(http_proxy));
    conn->data->req.protop = &http_proxy;
    connkeep(conn, "HTTP proxy CONNECT");

    /* for the secondary socket (FTP), use the "connect to host"
     * but ignore the "connect to port" (use the secondary port)
     */

    if(conn->bits.conn_to_host)
      hostname = conn->conn_to_host.name;
    else if(sockindex == SECONDARYSOCKET)
      hostname = conn->secondaryhostname;
    else
      hostname = conn->host.name;

    if(sockindex == SECONDARYSOCKET)
      remote_port = conn->secondary_port;
    else if(conn->bits.conn_to_port)
      remote_port = conn->conn_to_port;
    else
      remote_port = conn->remote_port;
    result = Curl_proxyCONNECT(conn, sockindex, hostname, remote_port);
    conn->data->req.protop = prot_save;
    if(CURLE_OK != result)
      return result;
    Curl_safefree(data->state.aptr.proxyuserpwd);
#else
    return CURLE_NOT_BUILT_IN;
#endif
  }
  /* no HTTP tunnel proxy, just return */
  return CURLE_OK;
}

bool Curl_connect_complete(struct connectdata *conn)
{
  return !conn->connect_state ||
    (conn->connect_state->tunnel_state == TUNNEL_COMPLETE);
}

bool Curl_connect_ongoing(struct connectdata *conn)
{
  return conn->connect_state &&
    (conn->connect_state->tunnel_state != TUNNEL_COMPLETE);
}

static CURLcode connect_init(struct connectdata *conn, bool reinit)
{
  struct http_connect_state *s;
  if(!reinit) {
    DEBUGASSERT(!conn->connect_state);
    s = calloc(1, sizeof(struct http_connect_state));
    if(!s)
      return CURLE_OUT_OF_MEMORY;
    infof(conn->data, "allocate connect buffer!\n");
    conn->connect_state = s;
    Curl_dyn_init(&s->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  }
  else {
    DEBUGASSERT(conn->connect_state);
    s = conn->connect_state;
    Curl_dyn_reset(&s->rcvbuf);
  }
  s->tunnel_state = TUNNEL_INIT;
  s->keepon = TRUE;
  s->cl = 0;
  s->close_connection = FALSE;
  return CURLE_OK;
}

static void connect_done(struct connectdata *conn)
{
  struct http_connect_state *s = conn->connect_state;
  s->tunnel_state = TUNNEL_COMPLETE;
  Curl_dyn_free(&s->rcvbuf);
  infof(conn->data, "CONNECT phase completed!\n");
}

static CURLcode CONNECT(struct connectdata *conn,
                        int sockindex,
                        const char *hostname,
                        int remote_port)
{
  int subversion = 0;
  struct Curl_easy *data = conn->data;
  struct SingleRequest *k = &data->req;
  CURLcode result;
  curl_socket_t tunnelsocket = conn->sock[sockindex];
  struct http_connect_state *s = conn->connect_state;
  char *linep;
  size_t perline;

#define SELECT_OK      0
#define SELECT_ERROR   1

  if(Curl_connect_complete(conn))
    return CURLE_OK; /* CONNECT is already completed */

  conn->bits.proxy_connect_closed = FALSE;

  do {
    timediff_t check;
    if(TUNNEL_INIT == s->tunnel_state) {
      /* BEGIN CONNECT PHASE */
      char *host_port;
      struct dynbuf req;

      infof(data, "Establish HTTP proxy tunnel to %s:%d\n",
            hostname, remote_port);

        /* This only happens if we've looped here due to authentication
           reasons, and we don't really use the newly cloned URL here
           then. Just free() it. */
      free(data->req.newurl);
      data->req.newurl = NULL;

      host_port = aprintf("%s:%d", hostname, remote_port);
      if(!host_port)
        return CURLE_OUT_OF_MEMORY;

      /* initialize a dynamic send-buffer */
      Curl_dyn_init(&req, DYN_HTTP_REQUEST);

      /* Setup the proxy-authorization header, if any */
      result = Curl_http_output_auth(conn, "CONNECT", host_port, TRUE);

      free(host_port);

      if(!result) {
        char *host = NULL;
        const char *proxyconn = "";
        const char *useragent = "";
        const char *httpv =
          (conn->http_proxy.proxytype == CURLPROXY_HTTP_1_0) ? "1.0" : "1.1";
        bool ipv6_ip = conn->bits.ipv6_ip;
        char *hostheader;

        /* the hostname may be different */
        if(hostname != conn->host.name)
          ipv6_ip = (strchr(hostname, ':') != NULL);
        hostheader = /* host:port with IPv6 support */
          aprintf("%s%s%s:%d", ipv6_ip?"[":"", hostname, ipv6_ip?"]":"",
                  remote_port);
        if(!hostheader) {
          Curl_dyn_free(&req);
          return CURLE_OUT_OF_MEMORY;
        }

        if(!Curl_checkProxyheaders(conn, "Host")) {
          host = aprintf("Host: %s\r\n", hostheader);
          if(!host) {
            free(hostheader);
            Curl_dyn_free(&req);
            return CURLE_OUT_OF_MEMORY;
          }
        }
        if(!Curl_checkProxyheaders(conn, "Proxy-Connection"))
          proxyconn = "Proxy-Connection: Keep-Alive\r\n";

        if(!Curl_checkProxyheaders(conn, "User-Agent") &&
           data->set.str[STRING_USERAGENT])
          useragent = data->state.aptr.uagent;

        result =
          Curl_dyn_addf(&req,
                        "CONNECT %s HTTP/%s\r\n"
                        "%s"  /* Host: */
                        "%s"  /* Proxy-Authorization */
                        "%s"  /* User-Agent */
                        "%s", /* Proxy-Connection */
                        hostheader,
                        httpv,
                        host?host:"",
                        data->state.aptr.proxyuserpwd?
                        data->state.aptr.proxyuserpwd:"",
                        useragent,
                        proxyconn);

        if(host)
          free(host);
        free(hostheader);

        if(!result)
          result = Curl_add_custom_headers(conn, TRUE, &req);

        if(!result)
          /* CRLF terminate the request */
          result = Curl_dyn_add(&req, "\r\n");

        if(!result) {
          /* Send the connect request to the proxy */
          /* BLOCKING */
          result = Curl_buffer_send(&req, conn, &data->info.request_size, 0,
                                    sockindex);
        }
        if(result)
          failf(data, "Failed sending CONNECT to proxy");
      }

      Curl_dyn_free(&req);
      if(result)
        return result;

      s->tunnel_state = TUNNEL_CONNECT;
    } /* END CONNECT PHASE */

    check = Curl_timeleft(data, NULL, TRUE);
    if(check <= 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      return CURLE_OPERATION_TIMEDOUT;
    }

    if(!Curl_conn_data_pending(conn, sockindex))
      /* return so we'll be called again polling-style */
      return CURLE_OK;

    /* at this point, the tunnel_connecting phase is over. */

    { /* READING RESPONSE PHASE */
      int error = SELECT_OK;

      while(s->keepon) {
        ssize_t gotbytes;
        char byte;

        /* Read one byte at a time to avoid a race condition. Wait at most one
           second before looping to ensure continuous pgrsUpdates. */
        result = Curl_read(conn, tunnelsocket, &byte, 1, &gotbytes);
        if(result == CURLE_AGAIN)
          /* socket buffer drained, return */
          return CURLE_OK;

        if(Curl_pgrsUpdate(conn))
          return CURLE_ABORTED_BY_CALLBACK;

        if(result) {
          s->keepon = FALSE;
          break;
        }
        else if(gotbytes <= 0) {
          if(data->set.proxyauth && data->state.authproxy.avail) {
            /* proxy auth was requested and there was proxy auth available,
               then deem this as "mere" proxy disconnect */
            conn->bits.proxy_connect_closed = TRUE;
            infof(data, "Proxy CONNECT connection closed\n");
          }
          else {
            error = SELECT_ERROR;
            failf(data, "Proxy CONNECT aborted");
          }
          s->keepon = FALSE;
          break;
        }

        if(s->keepon > TRUE) {
          /* This means we are currently ignoring a response-body */

          if(s->cl) {
            /* A Content-Length based body: simply count down the counter
               and make sure to break out of the loop when we're done! */
            s->cl--;
            if(s->cl <= 0) {
              s->keepon = FALSE;
              s->tunnel_state = TUNNEL_COMPLETE;
              break;
            }
          }
          else {
            /* chunked-encoded body, so we need to do the chunked dance
               properly to know when the end of the body is reached */
            CHUNKcode r;
            CURLcode extra;
            ssize_t tookcareof = 0;

            /* now parse the chunked piece of data so that we can
               properly tell when the stream ends */
            r = Curl_httpchunk_read(conn, &byte, 1, &tookcareof, &extra);
            if(r == CHUNKE_STOP) {
              /* we're done reading chunks! */
              infof(data, "chunk reading DONE\n");
              s->keepon = FALSE;
              /* we did the full CONNECT treatment, go COMPLETE */
              s->tunnel_state = TUNNEL_COMPLETE;
            }
          }
          continue;
        }

        if(Curl_dyn_addn(&s->rcvbuf, &byte, 1)) {
          failf(data, "CONNECT response too large!");
          return CURLE_RECV_ERROR;
        }

        /* if this is not the end of a header line then continue */
        if(byte != 0x0a)
          continue;

        linep = Curl_dyn_ptr(&s->rcvbuf);
        perline = Curl_dyn_len(&s->rcvbuf); /* amount of bytes in this line */

        /* convert from the network encoding */
        result = Curl_convert_from_network(data, linep, perline);
        /* Curl_convert_from_network calls failf if unsuccessful */
        if(result)
          return result;

        /* output debug if that is requested */
        if(data->set.verbose)
          Curl_debug(data, CURLINFO_HEADER_IN, linep, perline);

        if(!data->set.suppress_connect_headers) {
          /* send the header to the callback */
          int writetype = CLIENTWRITE_HEADER;
          if(data->set.include_header)
            writetype |= CLIENTWRITE_BODY;

          result = Curl_client_write(conn, writetype, linep, perline);
          if(result)
            return result;
        }

        data->info.header_size += (long)perline;
        data->req.headerbytecount += (long)perline;

        /* Newlines are CRLF, so the CR is ignored as the line isn't
           really terminated until the LF comes. Treat a following CR
           as end-of-headers as well.*/

        if(('\r' == linep[0]) ||
           ('\n' == linep[0])) {
          /* end of response-headers from the proxy */

          if((407 == k->httpcode) && !data->state.authproblem) {
            /* If we get a 407 response code with content length
               when we have no auth problem, we must ignore the
               whole response-body */
            s->keepon = 2;

            if(s->cl) {
              infof(data, "Ignore %" CURL_FORMAT_CURL_OFF_T
                    " bytes of response-body\n", s->cl);
            }
            else if(s->chunked_encoding) {
              CHUNKcode r;
              CURLcode extra;

              infof(data, "Ignore chunked response-body\n");

              /* We set ignorebody true here since the chunked decoder
                 function will acknowledge that. Pay attention so that this is
                 cleared again when this function returns! */
              k->ignorebody = TRUE;

              if(linep[1] == '\n')
                /* this can only be a LF if the letter at index 0 was a CR */
                linep++;

              /* now parse the chunked piece of data so that we can properly
                 tell when the stream ends */
              r = Curl_httpchunk_read(conn, linep + 1, 1, &gotbytes,
                                      &extra);
              if(r == CHUNKE_STOP) {
                /* we're done reading chunks! */
                infof(data, "chunk reading DONE\n");
                s->keepon = FALSE;
                /* we did the full CONNECT treatment, go to COMPLETE */
                s->tunnel_state = TUNNEL_COMPLETE;
              }
            }
            else {
              /* without content-length or chunked encoding, we
                 can't keep the connection alive since the close is
                 the end signal so we bail out at once instead */
              s->keepon = FALSE;
            }
          }
          else
            s->keepon = FALSE;
          if(!s->cl)
            /* we did the full CONNECT treatment, go to COMPLETE */
            s->tunnel_state = TUNNEL_COMPLETE;
          continue;
        }

        if((checkprefix("WWW-Authenticate:", linep) &&
            (401 == k->httpcode)) ||
           (checkprefix("Proxy-authenticate:", linep) &&
            (407 == k->httpcode))) {

          bool proxy = (k->httpcode == 407) ? TRUE : FALSE;
          char *auth = Curl_copy_header_value(linep);
          if(!auth)
            return CURLE_OUT_OF_MEMORY;

          result = Curl_http_input_auth(conn, proxy, auth);

          free(auth);

          if(result)
            return result;
        }
        else if(checkprefix("Content-Length:", linep)) {
          if(k->httpcode/100 == 2) {
            /* A client MUST ignore any Content-Length or Transfer-Encoding
               header fields received in a successful response to CONNECT.
               "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
            infof(data, "Ignoring Content-Length in CONNECT %03d response\n",
                  k->httpcode);
          }
          else {
            (void)curlx_strtoofft(linep +
                                  strlen("Content-Length:"), NULL, 10, &s->cl);
          }
        }
        else if(Curl_compareheader(linep, "Connection:", "close"))
          s->close_connection = TRUE;
        else if(checkprefix("Transfer-Encoding:", linep)) {
          if(k->httpcode/100 == 2) {
            /* A client MUST ignore any Content-Length or Transfer-Encoding
               header fields received in a successful response to CONNECT.
               "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
            infof(data, "Ignoring Transfer-Encoding in "
                  "CONNECT %03d response\n", k->httpcode);
          }
          else if(Curl_compareheader(linep,
                                     "Transfer-Encoding:", "chunked")) {
            infof(data, "CONNECT responded chunked\n");
            s->chunked_encoding = TRUE;
            /* init our chunky engine */
            Curl_httpchunk_init(conn);
          }
        }
        else if(Curl_compareheader(linep, "Proxy-Connection:", "close"))
          s->close_connection = TRUE;
        else if(2 == sscanf(linep, "HTTP/1.%d %d",
                            &subversion,
                            &k->httpcode)) {
          /* store the HTTP code from the proxy */
          data->info.httpproxycode = k->httpcode;
        }

        Curl_dyn_reset(&s->rcvbuf);
      } /* while there's buffer left and loop is requested */

      if(Curl_pgrsUpdate(conn))
        return CURLE_ABORTED_BY_CALLBACK;

      if(error)
        return CURLE_RECV_ERROR;

      if(data->info.httpproxycode/100 != 2) {
        /* Deal with the possibly already received authenticate
           headers. 'newurl' is set to a new URL if we must loop. */
        result = Curl_http_auth_act(conn);
        if(result)
          return result;

        if(conn->bits.close)
          /* the connection has been marked for closure, most likely in the
             Curl_http_auth_act() function and thus we can kill it at once
             below */
          s->close_connection = TRUE;
      }

      if(s->close_connection && data->req.newurl) {
        /* Connection closed by server. Don't use it anymore */
        Curl_closesocket(conn, conn->sock[sockindex]);
        conn->sock[sockindex] = CURL_SOCKET_BAD;
        break;
      }
    } /* END READING RESPONSE PHASE */

    /* If we are supposed to continue and request a new URL, which basically
     * means the HTTP authentication is still going on so if the tunnel
     * is complete we start over in INIT state */
    if(data->req.newurl && (TUNNEL_COMPLETE == s->tunnel_state)) {
      connect_init(conn, TRUE); /* reinit */
    }

  } while(data->req.newurl);

  if(data->info.httpproxycode/100 != 2) {
    if(s->close_connection && data->req.newurl) {
      conn->bits.proxy_connect_closed = TRUE;
      infof(data, "Connect me again please\n");
      connect_done(conn);
    }
    else {
      free(data->req.newurl);
      data->req.newurl = NULL;
      /* failure, close this connection to avoid re-use */
      streamclose(conn, "proxy CONNECT failure");
      Curl_closesocket(conn, conn->sock[sockindex]);
      conn->sock[sockindex] = CURL_SOCKET_BAD;
    }

    /* to back to init state */
    s->tunnel_state = TUNNEL_INIT;

    if(conn->bits.proxy_connect_closed)
      /* this is not an error, just part of the connection negotiation */
      return CURLE_OK;
    Curl_dyn_free(&s->rcvbuf);
    failf(data, "Received HTTP code %d from proxy after CONNECT",
          data->req.httpcode);
    return CURLE_RECV_ERROR;
  }

  s->tunnel_state = TUNNEL_COMPLETE;

  /* If a proxy-authorization header was used for the proxy, then we should
     make sure that it isn't accidentally used for the document request
     after we've connected. So let's free and clear it here. */
  Curl_safefree(data->state.aptr.proxyuserpwd);
  data->state.aptr.proxyuserpwd = NULL;

  data->state.authproxy.done = TRUE;
  data->state.authproxy.multipass = FALSE;

  infof(data, "Proxy replied %d to CONNECT request\n",
        data->info.httpproxycode);
  data->req.ignorebody = FALSE; /* put it (back) to non-ignore state */
  conn->bits.rewindaftersend = FALSE; /* make sure this isn't set for the
                                         document request  */
  Curl_dyn_free(&s->rcvbuf);
  return CURLE_OK;
}

void Curl_connect_free(struct Curl_easy *data)
{
  struct connectdata *conn = data->conn;
  struct http_connect_state *s = conn->connect_state;
  if(s) {
    free(s);
    conn->connect_state = NULL;
  }
}

/*
 * Curl_proxyCONNECT() requires that we're connected to a HTTP proxy. This
 * function will issue the necessary commands to get a seamless tunnel through
 * this proxy. After that, the socket can be used just as a normal socket.
 */

CURLcode Curl_proxyCONNECT(struct connectdata *conn,
                           int sockindex,
                           const char *hostname,
                           int remote_port)
{
  CURLcode result;
  if(!conn->connect_state) {
    result = connect_init(conn, FALSE);
    if(result)
      return result;
  }
  result = CONNECT(conn, sockindex, hostname, remote_port);

  if(result || Curl_connect_complete(conn))
    connect_done(conn);

  return result;
}

#else
void Curl_connect_free(struct Curl_easy *data)
{
  (void)data;
}

#endif /* CURL_DISABLE_PROXY */
