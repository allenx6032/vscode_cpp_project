/*
 * libwebsockets - small server side websockets and web server implementation
 *
 * Copyright (C) 2010-2016 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include "libwebsockets_private-libwebsockets.h"

#ifdef LWS_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#else
#include <sys/wait.h>
#endif

#ifdef LWS_USE_IPV6
#if defined(WIN32) || defined(_WIN32)
#include <Iphlpapi.h>
#else
#include <net/if.h>
#endif
#endif

int log_level_ = LLL_ERR | LLL_WARN | LLL_NOTICE;
static void (*lwsl_emit)(int level, const char *line)
#ifndef LWS_PLAT_OPTEE
	= lwsl_emit_stderr
#endif
	;
#ifndef LWS_PLAT_OPTEE
static const char * const log_level_names[] = {
	"ERR",
	"WARN",
	"NOTICE",
	"INFO",
	"DEBUG",
	"PARSER",
	"HEADER",
	"EXTENSION",
	"CLIENT",
	"LATENCY",
	"USER",
	"?",
	"?"
};
#endif

void
lws_free_wsi(struct lws *wsi)
{
	struct lws_context_per_thread *pt;
	int n;

	if (!wsi)
		return;
	
	pt = &wsi->context->pt[(int)wsi->tsi];

	/* Protocol user data may be allocated either internally by lws
	 * or by specified the user.
	 * We should only free what we allocated. */
	if (wsi->protocol && wsi->protocol->per_session_data_size &&
	    wsi->user_space && !wsi->user_space_externally_allocated)
		lws_free(wsi->user_space);

	lws_free_set_NULL(wsi->rxflow_buffer);
	lws_free_set_NULL(wsi->trunc_alloc);

	/* we may not have an ah, but may be on the waiting list... */
	lwsl_info("ah det due to close\n");
	/* we're closing, losing some rx is OK */
	lws_header_table_force_to_detachable_state(wsi);
	lws_header_table_detach(wsi, 0);

	lws_pt_lock(pt);
	for (n = 0; n < wsi->context->max_http_header_pool; n++) {
		if (pt->ah_pool[n].in_use &&
		    pt->ah_pool[n].wsi == wsi) {
			lwsl_err("%s: ah leak: wsi %p\n", __func__, wsi);
			pt->ah_pool[n].in_use = 0;
			pt->ah_pool[n].wsi = NULL;
			pt->ah_count_in_use--;
		}
	}
	lws_pt_unlock(pt);

	/* since we will destroy the wsi, make absolutely sure now */

	lws_ssl_remove_wsi_from_buffered_list(wsi);
	lws_remove_from_timeout_list(wsi);

	wsi->context->count_wsi_allocated--;
	lwsl_debug("%s: %p, remaining wsi %d\n", __func__, wsi,
			wsi->context->count_wsi_allocated);

	lws_free(wsi);
}

void
lws_remove_from_timeout_list(struct lws *wsi)
{
	struct lws_context_per_thread *pt = &wsi->context->pt[(int)wsi->tsi];

	if (!wsi->timeout_list_prev) /* ie, not part of the list */
		return;

	lws_pt_lock(pt);
	/* if we have a next guy, set his prev to our prev */
	if (wsi->timeout_list)
		wsi->timeout_list->timeout_list_prev = wsi->timeout_list_prev;
	/* set our prev guy to our next guy instead of us */
	*wsi->timeout_list_prev = wsi->timeout_list;

	/* we're out of the list, we should not point anywhere any more */
	wsi->timeout_list_prev = NULL;
	wsi->timeout_list = NULL;
	lws_pt_unlock(pt);
}

LWS_VISIBLE void
lws_set_timeout(struct lws *wsi, enum pending_timeout reason, int secs)
{
	struct lws_context_per_thread *pt = &wsi->context->pt[(int)wsi->tsi];
	time_t now;

	if (secs == LWS_TO_KILL_SYNC) {
		lws_remove_from_timeout_list(wsi);
		lwsl_debug("synchronously killing %p\n", wsi);
		lws_close_free_wsi(wsi, LWS_CLOSE_STATUS_NOSTATUS);
		return;
	}

	lws_pt_lock(pt);

	time(&now);

	if (reason && !wsi->timeout_list_prev) {
		/* our next guy is current first guy */
		wsi->timeout_list = pt->timeout_list;
		/* if there is a next guy, set his prev ptr to our next ptr */
		if (wsi->timeout_list)
			wsi->timeout_list->timeout_list_prev = &wsi->timeout_list;
		/* our prev ptr is first ptr */
		wsi->timeout_list_prev = &pt->timeout_list;
		/* set the first guy to be us */
		*wsi->timeout_list_prev = wsi;
	}

	lwsl_debug("%s: %p: %d secs\n", __func__, wsi, secs);
	wsi->pending_timeout_limit = now + secs;
	wsi->pending_timeout = reason;

	lws_pt_unlock(pt);

	if (!reason)
		lws_remove_from_timeout_list(wsi);
}

static void
lws_remove_child_from_any_parent(struct lws *wsi)
{
	struct lws **pwsi;
	int seen = 0;

	if (!wsi->parent)
		return;

	/* detach ourselves from parent's child list */
	pwsi = &wsi->parent->child_list;
	while (*pwsi) {
		if (*pwsi == wsi) {
			lwsl_info("%s: detach %p from parent %p\n",
					__func__, wsi, wsi->parent);

			if (wsi->parent->protocol)
				wsi->parent->protocol->callback(wsi,
						LWS_CALLBACK_CHILD_CLOSING,
					       wsi->parent->user_space, wsi, 0);

			*pwsi = wsi->sibling_list;
			seen = 1;
			break;
		}
		pwsi = &(*pwsi)->sibling_list;
	}
	if (!seen)
		lwsl_err("%s: failed to detach from parent\n", __func__);

	wsi->parent = NULL;
}

int
lws_bind_protocol(struct lws *wsi, const struct lws_protocols *p)
{
//	if (wsi->protocol == p)
//		return 0;
	const struct lws_protocols *vp = wsi->vhost->protocols, *vpo;

	if (wsi->protocol)
		wsi->protocol->callback(wsi, LWS_CALLBACK_HTTP_DROP_PROTOCOL,
					wsi->user_space, NULL, 0);
	if (!wsi->user_space_externally_allocated)
		lws_free_set_NULL(wsi->user_space);

	lws_same_vh_protocol_remove(wsi);

	wsi->protocol = p;
	if (!p)
		return 0;

	if (lws_ensure_user_space(wsi))
		return 1;

	if (p > vp && p < &vp[wsi->vhost->count_protocols])
		lws_same_vh_protocol_insert(wsi, p - vp);
	else {
		int n = wsi->vhost->count_protocols;
		int hit = 0;

		vpo = vp;

		while (n--) {
			if (p->name && vp->name && !strcmp(p->name, vp->name)) {
				hit = 1;
				lws_same_vh_protocol_insert(wsi, vp - vpo);
				break;
			}
			vp++;
		}
		if (!hit)
			lwsl_err("%s: protocol %p is not in vhost %s protocols list\n",
			 __func__, p, wsi->vhost->name);
	}

	if (wsi->protocol->callback(wsi, LWS_CALLBACK_HTTP_BIND_PROTOCOL,
				    wsi->user_space, NULL, 0))
		return 1;

	return 0;
}

void
lws_close_free_wsi(struct lws *wsi, enum lws_close_status reason)
{
	struct lws_context_per_thread *pt;
	struct lws *wsi1, *wsi2;
	struct lws_context *context;
	struct lws_tokens eff_buf;
	int n, m, ret;

	lwsl_debug("%s: %p\n", __func__, wsi);

	if (!wsi)
		return;

	lws_access_log(wsi);
#if defined(LWS_WITH_ESP8266)
	if (wsi->premature_rx)
		lws_free(wsi->premature_rx);

	if (wsi->pending_send_completion && !wsi->close_is_pending_send_completion) {
		lwsl_notice("delaying close\n");
		wsi->close_is_pending_send_completion = 1;
		return;
	}
#endif

	/* we're closing, losing some rx is OK */
	lws_header_table_force_to_detachable_state(wsi);

	context = wsi->context;
	pt = &context->pt[(int)wsi->tsi];
	lws_stats_atomic_bump(wsi->context, pt, LWSSTATS_C_API_CLOSE, 1);

	/* if we have children, close them first */
	if (wsi->child_list) {
		wsi2 = wsi->child_list;
		while (wsi2) {
			//lwsl_notice("%s: closing %p: close child %p\n",
			//		__func__, wsi, wsi2);
			wsi1 = wsi2->sibling_list;
			//lwsl_notice("%s: closing %p: next sibling %p\n",
			//		__func__, wsi2, wsi1);
			wsi2->parent = NULL;
			/* stop it doing shutdown processing */
			wsi2->socket_is_permanently_unusable = 1;
			lws_close_free_wsi(wsi2, reason);
			wsi2 = wsi1;
		}
		wsi->child_list = NULL;
	}

	if (wsi->mode == LWSCM_RAW_FILEDESC) {
			lws_remove_child_from_any_parent(wsi);
			remove_wsi_socket_from_fds(wsi);
			wsi->protocol->callback(wsi,
						LWS_CALLBACK_RAW_CLOSE_FILE,
						wsi->user_space, NULL, 0);
			goto async_close;
	}

#ifdef LWS_WITH_CGI
	if (wsi->mode == LWSCM_CGI) {
		/* we are not a network connection, but a handler for CGI io */
		if (wsi->parent && wsi->parent->cgi)
			/* end the binding between us and master */
			wsi->parent->cgi->stdwsi[(int)wsi->cgi_channel] = NULL;
		wsi->socket_is_permanently_unusable = 1;

		lwsl_debug("------ %s: detected cgi fdhandler wsi %p\n", __func__, wsi);
		goto just_kill_connection;
	}

	if (wsi->cgi) {
		struct lws_cgi **pcgi = &pt->cgi_list;
		/* remove us from the cgi list */
		lwsl_debug("%s: remove cgi %p from list\n", __func__, wsi->cgi);
		while (*pcgi) {
			if (*pcgi == wsi->cgi) {
				/* drop us from the pt cgi list */
				*pcgi = (*pcgi)->cgi_list;
				break;
			}
			pcgi = &(*pcgi)->cgi_list;
		}
		if (wsi->cgi->headers_buf) {
			lwsl_debug("close: freed cgi headers\n");
			lws_free_set_NULL(wsi->cgi->headers_buf);
		}
		/* we have a cgi going, we must kill it */
		wsi->cgi->being_closed = 1;
		lws_cgi_kill(wsi);
	}
#endif

#if !defined(LWS_NO_CLIENT)
	if (wsi->mode == LWSCM_HTTP_CLIENT ||
	    wsi->mode == LWSCM_WSCL_WAITING_CONNECT ||
	    wsi->mode == LWSCM_WSCL_WAITING_PROXY_REPLY ||
	    wsi->mode == LWSCM_WSCL_ISSUE_HANDSHAKE ||
	    wsi->mode == LWSCM_WSCL_ISSUE_HANDSHAKE2 ||
	    wsi->mode == LWSCM_WSCL_WAITING_SSL ||
	    wsi->mode == LWSCM_WSCL_WAITING_SERVER_REPLY ||
	    wsi->mode == LWSCM_WSCL_WAITING_EXTENSION_CONNECT ||
	    wsi->mode == LWSCM_WSCL_WAITING_SOCKS_GREETING_REPLY ||
	    wsi->mode == LWSCM_WSCL_WAITING_SOCKS_CONNECT_REPLY ||
	    wsi->mode == LWSCM_WSCL_WAITING_SOCKS_AUTH_REPLY)
		if (wsi->u.hdr.stash)
			lws_free_set_NULL(wsi->u.hdr.stash);
#endif

	if (wsi->mode == LWSCM_RAW) {
		wsi->protocol->callback(wsi,
			LWS_CALLBACK_RAW_CLOSE, wsi->user_space, NULL, 0);
		wsi->socket_is_permanently_unusable = 1;
		goto just_kill_connection;
	}

	if (wsi->mode == LWSCM_HTTP_SERVING_ACCEPTED &&
	    wsi->u.http.fop_fd != NULL) {
		lws_vfs_file_close(&wsi->u.http.fop_fd);
		wsi->vhost->protocols->callback(wsi,
			LWS_CALLBACK_CLOSED_HTTP, wsi->user_space, NULL, 0);
		wsi->told_user_closed = 1;
	}
	if (wsi->socket_is_permanently_unusable ||
	    reason == LWS_CLOSE_STATUS_NOSTATUS_CONTEXT_DESTROY ||
	    wsi->state == LWSS_SHUTDOWN)
		goto just_kill_connection;

	wsi->state_pre_close = wsi->state;

	switch (wsi->state_pre_close) {
	case LWSS_DEAD_SOCKET:
		return;

	/* we tried the polite way... */
	case LWSS_WAITING_TO_SEND_CLOSE_NOTIFICATION:
	case LWSS_AWAITING_CLOSE_ACK:
		goto just_kill_connection;

	case LWSS_FLUSHING_STORED_SEND_BEFORE_CLOSE:
		if (wsi->trunc_len) {
			lws_callback_on_writable(wsi);
			return;
		}
		lwsl_info("wsi %p completed LWSS_FLUSHING_STORED_SEND_BEFORE_CLOSE\n", wsi);
		goto just_kill_connection;
	default:
		if (wsi->trunc_len) {
			lwsl_info("wsi %p entering LWSS_FLUSHING_STORED_SEND_BEFORE_CLOSE\n", wsi);
			wsi->state = LWSS_FLUSHING_STORED_SEND_BEFORE_CLOSE;
			lws_set_timeout(wsi, PENDING_FLUSH_STORED_SEND_BEFORE_CLOSE, 5);
			return;
		}
		break;
	}

	if (wsi->mode == LWSCM_WSCL_WAITING_CONNECT ||
	    wsi->mode == LWSCM_WSCL_ISSUE_HANDSHAKE)
		goto just_kill_connection;

	if (wsi->mode == LWSCM_HTTP_SERVING) {
		if (wsi->user_space)
			wsi->vhost->protocols->callback(wsi,
						LWS_CALLBACK_HTTP_DROP_PROTOCOL,
					       wsi->user_space, NULL, 0);
		wsi->vhost->protocols->callback(wsi, LWS_CALLBACK_CLOSED_HTTP,
					       wsi->user_space, NULL, 0);
		wsi->told_user_closed = 1;
	}
	if (wsi->mode & LWSCM_FLAG_IMPLIES_CALLBACK_CLOSED_CLIENT_HTTP) {
		wsi->vhost->protocols[0].callback(wsi, LWS_CALLBACK_CLOSED_CLIENT_HTTP,
					       wsi->user_space, NULL, 0);
		wsi->told_user_closed = 1;
	}

	/*
	 * are his extensions okay with him closing?  Eg he might be a mux
	 * parent and just his ch1 aspect is closing?
	 */

	if (lws_ext_cb_active(wsi,
		      LWS_EXT_CB_CHECK_OK_TO_REALLY_CLOSE, NULL, 0) > 0) {
		lwsl_ext("extension vetoed close\n");
		return;
	}

	/*
	 * flush any tx pending from extensions, since we may send close packet
	 * if there are problems with send, just nuke the connection
	 */

	do {
		ret = 0;
		eff_buf.token = NULL;
		eff_buf.token_len = 0;

		/* show every extension the new incoming data */

		m = lws_ext_cb_active(wsi,
			  LWS_EXT_CB_FLUSH_PENDING_TX, &eff_buf, 0);
		if (m < 0) {
			lwsl_ext("Extension reports fatal error\n");
			goto just_kill_connection;
		}
		if (m)
			/*
			 * at least one extension told us he has more
			 * to spill, so we will go around again after
			 */
			ret = 1;

		/* assuming they left us something to send, send it */

		if (eff_buf.token_len)
			if (lws_issue_raw(wsi, (unsigned char *)eff_buf.token,
					  eff_buf.token_len) !=
			    eff_buf.token_len) {
				lwsl_debug("close: ext spill failed\n");
				goto just_kill_connection;
			}
	} while (ret);

	/*
	 * signal we are closing, lws_write will
	 * add any necessary version-specific stuff.  If the write fails,
	 * no worries we are closing anyway.  If we didn't initiate this
	 * close, then our state has been changed to
	 * LWSS_RETURNED_CLOSE_ALREADY and we will skip this.
	 *
	 * Likewise if it's a second call to close this connection after we
	 * sent the close indication to the peer already, we are in state
	 * LWSS_AWAITING_CLOSE_ACK and will skip doing this a second time.
	 */

	if (wsi->state_pre_close == LWSS_ESTABLISHED &&
	    (wsi->u.ws.close_in_ping_buffer_len || /* already a reason */
	     (reason != LWS_CLOSE_STATUS_NOSTATUS &&
	     (reason != LWS_CLOSE_STATUS_NOSTATUS_CONTEXT_DESTROY)))) {
		lwsl_debug("sending close indication...\n");

		/* if no prepared close reason, use 1000 and no aux data */
		if (!wsi->u.ws.close_in_ping_buffer_len) {
			wsi->u.ws.close_in_ping_buffer_len = 2;
			wsi->u.ws.ping_payload_buf[LWS_PRE] =
                               (reason >> 8) & 0xff;
			wsi->u.ws.ping_payload_buf[LWS_PRE + 1] =
				reason & 0xff;
		}

#if defined (LWS_WITH_ESP8266)
		wsi->close_is_pending_send_completion = 1;
#endif

		lwsl_debug("waiting for chance to send close\n");
		wsi->waiting_to_send_close_frame = 1;
		wsi->state = LWSS_WAITING_TO_SEND_CLOSE_NOTIFICATION;
		lws_set_timeout(wsi, PENDING_TIMEOUT_CLOSE_SEND, 2);
		lws_callback_on_writable(wsi);

		return;
	}

just_kill_connection:

	lws_remove_child_from_any_parent(wsi);

#if 0
	/* manage the vhost same protocol list entry */

	if (wsi->same_vh_protocol_prev) { // we are on the vh list

		// make guy who pointed to us, point to what our next was pointing to
		*wsi->same_vh_protocol_prev = wsi->same_vh_protocol_next;

		// if we had a next guy...
		if (wsi->same_vh_protocol_next)
			// have him point back to our prev
			wsi->same_vh_protocol_next->same_vh_protocol_prev =
					wsi->same_vh_protocol_prev;
	}
#endif

#if LWS_POSIX
	/*
	 * Testing with ab shows that we have to stage the socket close when
	 * the system is under stress... shutdown any further TX, change the
	 * state to one that won't emit anything more, and wait with a timeout
	 * for the POLLIN to show a zero-size rx before coming back and doing
	 * the actual close.
	 */
	if (wsi->mode != LWSCM_RAW &&
	    !(wsi->mode & LWSCM_FLAG_IMPLIES_CALLBACK_CLOSED_CLIENT_HTTP) &&
	    wsi->state != LWSS_SHUTDOWN &&
	    wsi->state != LWSS_CLIENT_UNCONNECTED &&
	    reason != LWS_CLOSE_STATUS_NOSTATUS_CONTEXT_DESTROY &&
	    !wsi->socket_is_permanently_unusable) {
#ifdef LWS_OPENSSL_SUPPORT
		if (lws_is_ssl(wsi) && wsi->ssl)
		{
			lwsl_info("%s: shutting down SSL connection: %p (ssl %p, sock %d, state %d)\n", __func__, wsi, wsi->ssl, (int)(long)wsi->desc.sockfd, wsi->state);
			n = SSL_shutdown(wsi->ssl);
			if (n == 1) /* If finished the SSL shutdown, then do socket shutdown, else need to retry SSL shutdown */
				n = shutdown(wsi->desc.sockfd, SHUT_WR);
			else if (n == 0)
				lws_change_pollfd(wsi, LWS_POLLOUT, LWS_POLLIN);
			else /* n < 0 */
			{
				int shutdown_error = SSL_get_error(wsi->ssl, n);
				lwsl_debug("SSL_shutdown returned %d, SSL_get_error: %d\n", n, shutdown_error);
				if (shutdown_error == SSL_ERROR_WANT_READ) {
					lws_change_pollfd(wsi, LWS_POLLOUT, LWS_POLLIN);
					n = 0;
				} else if (shutdown_error == SSL_ERROR_WANT_WRITE) {
					lws_change_pollfd(wsi, LWS_POLLOUT, LWS_POLLOUT);
					n = 0;
				} else { // actual error occurred, just close the connection
					n = shutdown(wsi->desc.sockfd, SHUT_WR);
				}
			}
		}
		else
#endif
		{
			lwsl_info("%s: shutting down connection: %p (sock %d, state %d)\n", __func__, wsi, (int)(long)wsi->desc.sockfd, wsi->state);
			n = shutdown(wsi->desc.sockfd, SHUT_WR);
		}
		if (n)
			lwsl_debug("closing: shutdown (state %d) ret %d\n", wsi->state, LWS_ERRNO);

// This causes problems with disconnection when the events are half closing connection
// FD_READ | FD_CLOSE (33)
#if !defined(_WIN32_WCE) && !defined(LWS_WITH_ESP32)
		/* libuv: no event available to guarantee completion */
		if (!LWS_LIBUV_ENABLED(context)) {

			lws_change_pollfd(wsi, LWS_POLLOUT, LWS_POLLIN);
			wsi->state = LWSS_SHUTDOWN;
			lws_set_timeout(wsi, PENDING_TIMEOUT_SHUTDOWN_FLUSH,
					context->timeout_secs);

			return;
		}
#endif
	}
#endif

	lwsl_info("%s: real just_kill_connection: %p (sockfd %d)\n", __func__,
		  wsi, wsi->desc.sockfd);
	
#ifdef LWS_WITH_HTTP_PROXY
	if (wsi->rw) {
		lws_rewrite_destroy(wsi->rw);
		wsi->rw = NULL;
	}
#endif
	/*
	 * we won't be servicing or receiving anything further from this guy
	 * delete socket from the internal poll list if still present
	 */
	lws_ssl_remove_wsi_from_buffered_list(wsi);

	lws_remove_from_timeout_list(wsi);

	/* checking return redundant since we anyway close */
	if (wsi->desc.sockfd != LWS_SOCK_INVALID)
		remove_wsi_socket_from_fds(wsi);
	else
		lws_same_vh_protocol_remove(wsi);

#if defined(LWS_WITH_ESP8266)
	espconn_disconnect(wsi->desc.sockfd);
#endif

	wsi->state = LWSS_DEAD_SOCKET;

	lws_free_set_NULL(wsi->rxflow_buffer);
	if (wsi->state_pre_close == LWSS_ESTABLISHED ||
	    wsi->mode == LWSCM_WS_SERVING ||
	    wsi->mode == LWSCM_WS_CLIENT) {

		if (wsi->u.ws.rx_draining_ext) {
			struct lws **w = &pt->rx_draining_ext_list;

			wsi->u.ws.rx_draining_ext = 0;
			/* remove us from context draining ext list */
			while (*w) {
				if (*w == wsi) {
					*w = wsi->u.ws.rx_draining_ext_list;
					break;
				}
				w = &((*w)->u.ws.rx_draining_ext_list);
			}
			wsi->u.ws.rx_draining_ext_list = NULL;
		}

		if (wsi->u.ws.tx_draining_ext) {
			struct lws **w = &pt->tx_draining_ext_list;

			wsi->u.ws.tx_draining_ext = 0;
			/* remove us from context draining ext list */
			while (*w) {
				if (*w == wsi) {
					*w = wsi->u.ws.tx_draining_ext_list;
					break;
				}
				w = &((*w)->u.ws.tx_draining_ext_list);
			}
			wsi->u.ws.tx_draining_ext_list = NULL;
		}
		lws_free_set_NULL(wsi->u.ws.rx_ubuf);

		if (wsi->trunc_alloc)
			/* not going to be completed... nuke it */
			lws_free_set_NULL(wsi->trunc_alloc);

		wsi->u.ws.ping_payload_len = 0;
		wsi->u.ws.ping_pending_flag = 0;
	}

	/* tell the user it's all over for this guy */

	if (wsi->mode != LWSCM_RAW && wsi->protocol && wsi->protocol->callback &&
	    ((wsi->state_pre_close == LWSS_ESTABLISHED) ||
	    (wsi->state_pre_close == LWSS_RETURNED_CLOSE_ALREADY) ||
	    (wsi->state_pre_close == LWSS_AWAITING_CLOSE_ACK) ||
	    (wsi->state_pre_close == LWSS_WAITING_TO_SEND_CLOSE_NOTIFICATION) ||
	    (wsi->state_pre_close == LWSS_FLUSHING_STORED_SEND_BEFORE_CLOSE) ||
	    (wsi->mode == LWSCM_WS_CLIENT && wsi->state_pre_close == LWSS_HTTP) ||
	    (wsi->mode == LWSCM_WS_SERVING && wsi->state_pre_close == LWSS_HTTP))) {

		if (wsi->user_space) {
                       lwsl_debug("%s: doing LWS_CALLBACK_HTTP_DROP_PROTOCOL for %p prot %s\n", __func__, wsi, wsi->protocol->name);
			wsi->protocol->callback(wsi,
					LWS_CALLBACK_HTTP_DROP_PROTOCOL,
					       wsi->user_space, NULL, 0);
		}
		lwsl_debug("calling back CLOSED\n");
		wsi->protocol->callback(wsi, LWS_CALLBACK_CLOSED,
					wsi->user_space, NULL, 0);
	} else if (wsi->mode == LWSCM_HTTP_SERVING_ACCEPTED) {
		lwsl_debug("calling back CLOSED_HTTP\n");
		wsi->vhost->protocols->callback(wsi, LWS_CALLBACK_CLOSED_HTTP,
					       wsi->user_space, NULL, 0 );
	} else if ((wsi->mode == LWSCM_WSCL_WAITING_SERVER_REPLY ||
		   wsi->mode == LWSCM_WSCL_WAITING_CONNECT) &&
		   !wsi->already_did_cce) {
			wsi->vhost->protocols[0].callback(wsi,
					LWS_CALLBACK_CLIENT_CONNECTION_ERROR,
					wsi->user_space, NULL, 0);
	} else
		lwsl_debug("not calling back closed mode=%d state=%d\n",
			   wsi->mode, wsi->state_pre_close);

	/* deallocate any active extension contexts */

	if (lws_ext_cb_active(wsi, LWS_EXT_CB_DESTROY, NULL, 0) < 0)
		lwsl_warn("extension destruction failed\n");
	/*
	 * inform all extensions in case they tracked this guy out of band
	 * even though not active on him specifically
	 */
	if (lws_ext_cb_all_exts(context, wsi,
		       LWS_EXT_CB_DESTROY_ANY_WSI_CLOSING, NULL, 0) < 0)
		lwsl_warn("ext destroy wsi failed\n");

async_close:

	wsi->socket_is_permanently_unusable = 1;

#ifdef LWS_USE_LIBUV
	if (!wsi->parent_carries_io)
		if (LWS_LIBUV_ENABLED(context)) {
			if (wsi->listener) {
				lwsl_debug("%s: stopping listner libuv poll\n", __func__);
				uv_poll_stop(&wsi->w_read.uv_watcher);
			}
			lwsl_debug("%s: lws_libuv_closehandle: wsi %p\n", __func__, wsi);
			/* libuv has to do his own close handle processing asynchronously */
			lws_libuv_closehandle(wsi);

			return;
		}
#endif

	lws_close_free_wsi_final(wsi);
}

void
lws_close_free_wsi_final(struct lws *wsi)
{
	int n;

	if (!lws_ssl_close(wsi) && lws_socket_is_valid(wsi->desc.sockfd)) {
#if LWS_POSIX
		//lwsl_err("*** closing sockfd %d\n", wsi->desc.sockfd);
		n = compatible_close(wsi->desc.sockfd);
		if (n)
			lwsl_debug("closing: close ret %d\n", LWS_ERRNO);

#else
		compatible_close(wsi->desc.sockfd);
		(void)n;
#endif
		wsi->desc.sockfd = LWS_SOCK_INVALID;
	}

	/* outermost destroy notification for wsi (user_space still intact) */
	wsi->vhost->protocols[0].callback(wsi, LWS_CALLBACK_WSI_DESTROY,
				       wsi->user_space, NULL, 0);

#ifdef LWS_WITH_CGI
	if (wsi->cgi) {
		for (n = 0; n < 6; n++) {
			if (wsi->cgi->pipe_fds[n / 2][n & 1] == 0)
				lwsl_err("ZERO FD IN CGI CLOSE");

			if (wsi->cgi->pipe_fds[n / 2][n & 1] >= 0)
				close(wsi->cgi->pipe_fds[n / 2][n & 1]);
		}

		lws_free(wsi->cgi);
	}
#endif

	lws_free_wsi(wsi);
}

LWS_VISIBLE LWS_EXTERN const char *
lws_get_urlarg_by_name(struct lws *wsi, const char *name, char *buf, int len)
{
	int n = 0, sl = strlen(name);

	while (lws_hdr_copy_fragment(wsi, buf, len,
			  WSI_TOKEN_HTTP_URI_ARGS, n) >= 0) {

		if (!strncmp(buf, name, sl))
			return buf + sl;

		n++;
	}

	return NULL;
}

#if LWS_POSIX && !defined(LWS_WITH_ESP32)
LWS_VISIBLE int
interface_to_sa(struct lws_vhost *vh, const char *ifname, struct sockaddr_in *addr, size_t addrlen)
{
	int ipv6 = 0;
#ifdef LWS_USE_IPV6
	ipv6 = LWS_IPV6_ENABLED(vh);
#endif
	(void)vh;

	return lws_interface_to_sa(ipv6, ifname, addr, addrlen);
}
#endif

#ifndef LWS_PLAT_OPTEE
#if LWS_POSIX
static int
lws_get_addresses(struct lws_vhost *vh, void *ads, char *name,
		  int name_len, char *rip, int rip_len)
{
#if LWS_POSIX
	struct addrinfo ai, *res;
	struct sockaddr_in addr4;

	rip[0] = '\0';
	name[0] = '\0';
	addr4.sin_family = AF_UNSPEC;

#ifdef LWS_USE_IPV6
	if (LWS_IPV6_ENABLED(vh)) {
		if (!lws_plat_inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ads)->sin6_addr, rip, rip_len)) {
			lwsl_err("inet_ntop: %s", strerror(LWS_ERRNO));
			return -1;
		}

		// Strip off the IPv4 to IPv6 header if one exists
		if (strncmp(rip, "::ffff:", 7) == 0)
			memmove(rip, rip + 7, strlen(rip) - 6);

		getnameinfo((struct sockaddr *)ads,
				sizeof(struct sockaddr_in6), name,
							name_len, NULL, 0, 0);

		return 0;
	} else
#endif
	{
		struct addrinfo *result;

		memset(&ai, 0, sizeof ai);
		ai.ai_family = PF_UNSPEC;
		ai.ai_socktype = SOCK_STREAM;
		ai.ai_flags = AI_CANONNAME;
#if !defined(LWS_WITH_ESP32)
		if (getnameinfo((struct sockaddr *)ads,
				sizeof(struct sockaddr_in),
				name, name_len, NULL, 0, 0))
			return -1;
#endif

		if (getaddrinfo(name, NULL, &ai, &result))
			return -1;

		res = result;
		while (addr4.sin_family == AF_UNSPEC && res) {
			switch (res->ai_family) {
			case AF_INET:
				addr4.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
				addr4.sin_family = AF_INET;
				break;
			}

			res = res->ai_next;
		}
		freeaddrinfo(result);
	}

	if (addr4.sin_family == AF_UNSPEC)
		return -1;

	if (lws_plat_inet_ntop(AF_INET, &addr4.sin_addr, rip, rip_len) == NULL)
		return -1;

	return 0;
#else
	(void)vh;
	(void)ads;
	(void)name;
	(void)name_len;
	(void)rip;
	(void)rip_len;

	return -1;
#endif
}
#endif


LWS_VISIBLE const char *
lws_get_peer_simple(struct lws *wsi, char *name, int namelen)
{
#if LWS_POSIX
	socklen_t len, olen;
#ifdef LWS_USE_IPV6
	struct sockaddr_in6 sin6;
#endif
	struct sockaddr_in sin4;
	int af = AF_INET;
	void *p, *q;

	if (wsi->parent_carries_io)
		wsi = wsi->parent;

#ifdef LWS_USE_IPV6
	if (LWS_IPV6_ENABLED(wsi->vhost)) {
		len = sizeof(sin6);
		p = &sin6;
		af = AF_INET6;
		q = &sin6.sin6_addr;
	} else
#endif
	{
		len = sizeof(sin4);
		p = &sin4;
		q = &sin4.sin_addr;
	}

	olen = len;
	if (getpeername(wsi->desc.sockfd, p, &len) < 0 || len > olen) {
		lwsl_warn("getpeername: %s\n", strerror(LWS_ERRNO));
		return NULL;
	}

	return lws_plat_inet_ntop(af, q, name, namelen);
#else
#if defined(LWS_WITH_ESP8266)
	return lws_plat_get_peer_simple(wsi, name, namelen);
#else
	return NULL;
#endif
#endif
}
#endif

LWS_VISIBLE void
lws_get_peer_addresses(struct lws *wsi, lws_sockfd_type fd, char *name,
		       int name_len, char *rip, int rip_len)
{
#ifndef LWS_PLAT_OPTEE
#if LWS_POSIX
	socklen_t len;
#ifdef LWS_USE_IPV6
	struct sockaddr_in6 sin6;
#endif
	struct sockaddr_in sin4;
	struct lws_context *context = wsi->context;
	int ret = -1;
	void *p;

	rip[0] = '\0';
	name[0] = '\0';

	lws_latency_pre(context, wsi);

#ifdef LWS_USE_IPV6
	if (LWS_IPV6_ENABLED(wsi->vhost)) {
		len = sizeof(sin6);
		p = &sin6;
	} else
#endif
	{
		len = sizeof(sin4);
		p = &sin4;
	}

	if (getpeername(fd, p, &len) < 0) {
		lwsl_warn("getpeername: %s\n", strerror(LWS_ERRNO));
		goto bail;
	}

	ret = lws_get_addresses(wsi->vhost, p, name, name_len, rip, rip_len);

bail:
	lws_latency(context, wsi, "lws_get_peer_addresses", ret, 1);
#endif
#endif
	(void)wsi;
	(void)fd;
	(void)name;
	(void)name_len;
	(void)rip;
	(void)rip_len;

}

LWS_EXTERN void *
lws_context_user(struct lws_context *context)
{
	return context->user_space;
}

LWS_VISIBLE struct lws_vhost *
lws_vhost_get(struct lws *wsi)
{
	return wsi->vhost;
}

LWS_VISIBLE struct lws_vhost *
lws_get_vhost(struct lws *wsi)
{
	return wsi->vhost;
}

LWS_VISIBLE const struct lws_protocols *
lws_protocol_get(struct lws *wsi)
{
	return wsi->protocol;
}

LWS_VISIBLE LWS_EXTERN const struct lws_protocols *
lws_vhost_name_to_protocol(struct lws_vhost *vh, const char *name)
{
	int n;

	for (n = 0; n < vh->count_protocols; n++)
		if (!strcmp(name, vh->protocols[n].name))
			return &vh->protocols[n];

	return NULL;
}

LWS_VISIBLE int
lws_callback_all_protocol(struct lws_context *context,
			  const struct lws_protocols *protocol, int reason)
{
	struct lws_context_per_thread *pt = &context->pt[0];
	unsigned int n, m = context->count_threads;
	struct lws *wsi;

	while (m--) {
		for (n = 0; n < pt->fds_count; n++) {
			wsi = wsi_from_fd(context, pt->fds[n].fd);
			if (!wsi)
				continue;
			if (wsi->protocol == protocol)
				protocol->callback(wsi, reason, wsi->user_space,
						   NULL, 0);
		}
		pt++;
	}

	return 0;
}

LWS_VISIBLE int
lws_callback_all_protocol_vhost(struct lws_vhost *vh,
			  const struct lws_protocols *protocol, int reason)
{
	struct lws_context *context = vh->context;
	struct lws_context_per_thread *pt = &context->pt[0];
	unsigned int n, m = context->count_threads;
	struct lws *wsi;

	while (m--) {
		for (n = 0; n < pt->fds_count; n++) {
			wsi = wsi_from_fd(context, pt->fds[n].fd);
			if (!wsi)
				continue;
			if (wsi->vhost == vh && wsi->protocol == protocol)
				protocol->callback(wsi, reason, wsi->user_space,
						   NULL, 0);
		}
		pt++;
	}

	return 0;
}

LWS_VISIBLE LWS_EXTERN int
lws_callback_vhost_protocols(struct lws *wsi, int reason, void *in, int len)
{
	int n;

	for (n = 0; n < wsi->vhost->count_protocols; n++)
		if (wsi->vhost->protocols[n].callback(wsi, reason, NULL, in, len))
			return 1;

	return 0;
}

LWS_VISIBLE LWS_EXTERN void
lws_set_fops(struct lws_context *context, const struct lws_plat_file_ops *fops)
{
	context->fops = fops;
}

LWS_VISIBLE LWS_EXTERN lws_filepos_t
lws_vfs_tell(lws_fop_fd_t fop_fd)
{
	return fop_fd->pos;
}

LWS_VISIBLE LWS_EXTERN lws_filepos_t
lws_vfs_get_length(lws_fop_fd_t fop_fd)
{
	return fop_fd->len;
}

LWS_VISIBLE LWS_EXTERN uint32_t
lws_vfs_get_mod_time(lws_fop_fd_t fop_fd)
{
	return fop_fd->mod_time;
}

LWS_VISIBLE lws_fileofs_t
lws_vfs_file_seek_set(lws_fop_fd_t fop_fd, lws_fileofs_t offset)
{
	lws_fileofs_t ofs;
	lwsl_debug("%s: seeking to %ld, len %ld\n", __func__, (long)offset, (long)fop_fd->len);
	ofs = fop_fd->fops->LWS_FOP_SEEK_CUR(fop_fd, offset - fop_fd->pos);
	lwsl_debug("%s: result %ld, fop_fd pos %ld\n", __func__, (long)ofs, (long)fop_fd->pos);
	return ofs;
}


LWS_VISIBLE lws_fileofs_t
lws_vfs_file_seek_end(lws_fop_fd_t fop_fd, lws_fileofs_t offset)
{
	return fop_fd->fops->LWS_FOP_SEEK_CUR(fop_fd, fop_fd->len + fop_fd->pos + offset);
}


const struct lws_plat_file_ops *
lws_vfs_select_fops(const struct lws_plat_file_ops *fops, const char *vfs_path,
		    const char **vpath)
{
	const struct lws_plat_file_ops *pf;
	const char *p = vfs_path;
	int n;

	*vpath = NULL;

	/* no non-platform fops, just use that */

	if (!fops->next)
		return fops;

	/*
	 *  scan the vfs path looking for indications we are to be
	 * handled by a specific fops
	 */

	while (p && *p) {
		if (*p != '/') {
			p++;
			continue;
		}
		/* the first one is always platform fops, so skip */
		pf = fops->next;
		while (pf) {
			n = 0;
			while (n < ARRAY_SIZE(pf->fi) && pf->fi[n].sig) {
				if (p >= vfs_path + pf->fi[n].len)
					if (!strncmp(p - (pf->fi[n].len - 1),
						    pf->fi[n].sig,
						    pf->fi[n].len - 1)) {
						*vpath = p + 1;
						return pf;
					}

				n++;
			}
			pf = pf->next;
		}
		p++;
	}

	return fops;
}

LWS_VISIBLE LWS_EXTERN lws_fop_fd_t LWS_WARN_UNUSED_RESULT
lws_vfs_file_open(const struct lws_plat_file_ops *fops, const char *vfs_path,
		  lws_fop_flags_t *flags)
{
	const char *vpath = "";
	const struct lws_plat_file_ops *selected = lws_vfs_select_fops(
			fops, vfs_path, &vpath);

	return selected->LWS_FOP_OPEN(fops, vfs_path, vpath, flags);
}


/**
 * lws_now_secs() - seconds since 1970-1-1
 *
 */
LWS_VISIBLE LWS_EXTERN unsigned long
lws_now_secs(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec;
}


#if LWS_POSIX

LWS_VISIBLE int
lws_get_socket_fd(struct lws *wsi)
{
	return wsi->desc.sockfd;
}

#endif

#ifdef LWS_LATENCY
void
lws_latency(struct lws_context *context, struct lws *wsi, const char *action,
	    int ret, int completed)
{
	unsigned long long u;
	char buf[256];

	u = time_in_microseconds();

	if (!action) {
		wsi->latency_start = u;
		if (!wsi->action_start)
			wsi->action_start = u;
		return;
	}
	if (completed) {
		if (wsi->action_start == wsi->latency_start)
			sprintf(buf,
			  "Completion first try lat %lluus: %p: ret %d: %s\n",
					u - wsi->latency_start,
						      (void *)wsi, ret, action);
		else
			sprintf(buf,
			  "Completion %lluus: lat %lluus: %p: ret %d: %s\n",
				u - wsi->action_start,
					u - wsi->latency_start,
						      (void *)wsi, ret, action);
		wsi->action_start = 0;
	} else
		sprintf(buf, "lat %lluus: %p: ret %d: %s\n",
			      u - wsi->latency_start, (void *)wsi, ret, action);

	if (u - wsi->latency_start > context->worst_latency) {
		context->worst_latency = u - wsi->latency_start;
		strcpy(context->worst_latency_info, buf);
	}
	lwsl_latency("%s", buf);
}
#endif

LWS_VISIBLE int
lws_rx_flow_control(struct lws *wsi, int enable)
{
	if (enable == (wsi->rxflow_change_to & LWS_RXFLOW_ALLOW))
		return 0;

	lwsl_info("%s: (0x%p, %d)\n", __func__, wsi, enable);
	wsi->rxflow_change_to = LWS_RXFLOW_PENDING_CHANGE | !!enable;

	return 0;
}

LWS_VISIBLE void
lws_rx_flow_allow_all_protocol(const struct lws_context *context,
			       const struct lws_protocols *protocol)
{
	const struct lws_context_per_thread *pt = &context->pt[0];
	struct lws *wsi;
	unsigned int n, m = context->count_threads;

	while (m--) {
		for (n = 0; n < pt->fds_count; n++) {
			wsi = wsi_from_fd(context, pt->fds[n].fd);
			if (!wsi)
				continue;
			if (wsi->protocol == protocol)
				lws_rx_flow_control(wsi, LWS_RXFLOW_ALLOW);
		}
		pt++;
	}
}

LWS_VISIBLE extern const char *
lws_canonical_hostname(struct lws_context *context)
{
	return (const char *)context->canonical_hostname;
}

int user_callback_handle_rxflow(lws_callback_function callback_function,
				struct lws *wsi,
				enum lws_callback_reasons reason, void *user,
				void *in, size_t len)
{
	int n;

	n = callback_function(wsi, reason, user, in, len);
	if (!n)
		n = _lws_rx_flow_control(wsi);

	return n;
}

#if defined(LWS_WITH_ESP8266)
#undef strchr
#define strchr ets_strchr
#endif

LWS_VISIBLE int
lws_set_proxy(struct lws_vhost *vhost, const char *proxy)
{
#if !defined(LWS_WITH_ESP8266)
	char *p;
	char authstring[96];

	if (!proxy)
		return -1;

	/* we have to deal with a possible redundant leading http:// */
	if (!strncmp(proxy, "http://", 7))
		proxy += 7;

	p = strchr(proxy, '@');
	if (p) { /* auth is around */

		if ((unsigned int)(p - proxy) > sizeof(authstring) - 1)
			goto auth_too_long;

		strncpy(authstring, proxy, p - proxy);
		// null termination not needed on input
		if (lws_b64_encode_string(authstring, (p - proxy),
				vhost->proxy_basic_auth_token,
		    sizeof vhost->proxy_basic_auth_token) < 0)
			goto auth_too_long;

		lwsl_info(" Proxy auth in use\n");

		proxy = p + 1;
	} else
		vhost->proxy_basic_auth_token[0] = '\0';

	strncpy(vhost->http_proxy_address, proxy,
				sizeof(vhost->http_proxy_address) - 1);
	vhost->http_proxy_address[
				sizeof(vhost->http_proxy_address) - 1] = '\0';

	p = strchr(vhost->http_proxy_address, ':');
	if (!p && !vhost->http_proxy_port) {
		lwsl_err("http_proxy needs to be ads:port\n");

		return -1;
	} else {
		if (p) {
			*p = '\0';
			vhost->http_proxy_port = atoi(p + 1);
		}
	}

	lwsl_info(" Proxy %s:%u\n", vhost->http_proxy_address,
			vhost->http_proxy_port);

	return 0;

auth_too_long:
	lwsl_err("proxy auth too long\n");
#endif
	return -1;
}

#if defined(LWS_WITH_SOCKS5)
LWS_VISIBLE int
lws_set_socks(struct lws_vhost *vhost, const char *socks)
{
#if !defined(LWS_WITH_ESP8266)
	char *p_at, *p_colon;
	char user[96];
	char password[96];

	if (!socks)
		return -1;

	vhost->socks_user[0] = '\0';
	vhost->socks_password[0] = '\0';

	p_at = strchr(socks, '@');
	if (p_at) { /* auth is around */
		if ((unsigned int)(p_at - socks) > (sizeof(user)
			+ sizeof(password) - 2)) {
			lwsl_err("Socks auth too long\n");
			goto bail;
		}

		p_colon = strchr(socks, ':');
		if (p_colon) {
			if ((unsigned int)(p_colon - socks) > (sizeof(user)
				- 1) ) {
				lwsl_err("Socks user too long\n");
				goto bail;
			}
			if ((unsigned int)(p_at - p_colon) > (sizeof(password)
				- 1) ) {
				lwsl_err("Socks password too long\n");
				goto bail;
			}
		}
		strncpy(vhost->socks_user, socks, p_colon - socks);
		strncpy(vhost->socks_password, p_colon + 1,
			p_at - (p_colon + 1));

		lwsl_info(" Socks auth, user: %s, password: %s\n",
			vhost->socks_user, vhost->socks_password );

		socks = p_at + 1;
	}

	strncpy(vhost->socks_proxy_address, socks,
				sizeof(vhost->socks_proxy_address) - 1);
	vhost->socks_proxy_address[sizeof(vhost->socks_proxy_address) - 1]
		= '\0';

	p_colon = strchr(vhost->socks_proxy_address, ':');
	if (!p_colon && !vhost->socks_proxy_port) {
		lwsl_err("socks_proxy needs to be address:port\n");
		return -1;
	} else {
		if (p_colon) {
			*p_colon = '\0';
			vhost->socks_proxy_port = atoi(p_colon + 1);
		}
	}

	lwsl_info(" Socks %s:%u\n", vhost->socks_proxy_address,
			vhost->socks_proxy_port);

	return 0;

bail:
#endif
	return -1;
}
#endif

LWS_VISIBLE const struct lws_protocols *
lws_get_protocol(struct lws *wsi)
{
	return wsi->protocol;
}

LWS_VISIBLE int
lws_is_final_fragment(struct lws *wsi)
{
       lwsl_info("%s: final %d, rx pk length %ld, draining %ld\n", __func__,
			wsi->u.ws.final, (long)wsi->u.ws.rx_packet_length,
			(long)wsi->u.ws.rx_draining_ext);
	return wsi->u.ws.final && !wsi->u.ws.rx_packet_length && !wsi->u.ws.rx_draining_ext;
}

LWS_VISIBLE int
lws_is_first_fragment(struct lws *wsi)
{
	return wsi->u.ws.first_fragment;
}

LWS_VISIBLE unsigned char
lws_get_reserved_bits(struct lws *wsi)
{
	return wsi->u.ws.rsv;
}

int
lws_ensure_user_space(struct lws *wsi)
{
	lwsl_info("%s: %p protocol %p\n", __func__, wsi, wsi->protocol);
	if (!wsi->protocol)
		return 1;

	/* allocate the per-connection user memory (if any) */

	if (wsi->protocol->per_session_data_size && !wsi->user_space) {
		wsi->user_space = lws_zalloc(wsi->protocol->per_session_data_size);
		if (wsi->user_space  == NULL) {
			lwsl_err("Out of memory for conn user space\n");
			return 1;
		}
	} else
		lwsl_info("%s: %p protocol pss %lu, user_space=%p\n",
			  __func__, wsi, (long)wsi->protocol->per_session_data_size,
			  wsi->user_space);
	return 0;
}

LWS_VISIBLE int
lwsl_timestamp(int level, char *p, int len)
{
#ifndef LWS_PLAT_OPTEE
	time_t o_now = time(NULL);
	unsigned long long now;
	struct tm *ptm = NULL;
#ifndef WIN32
	struct tm tm;
#endif
	int n;

#ifndef _WIN32_WCE
#ifdef WIN32
	ptm = localtime(&o_now);
#else
	if (localtime_r(&o_now, &tm))
		ptm = &tm;
#endif
#endif
	p[0] = '\0';
	for (n = 0; n < LLL_COUNT; n++) {
		if (level != (1 << n))
			continue;
		now = time_in_microseconds() / 100;
		if (ptm)
			n = lws_snprintf(p, len,
				"[%04d/%02d/%02d %02d:%02d:%02d:%04d] %s: ",
				ptm->tm_year + 1900,
				ptm->tm_mon + 1,
				ptm->tm_mday,
				ptm->tm_hour,
				ptm->tm_min,
				ptm->tm_sec,
				(int)(now % 10000), log_level_names[n]);
		else
			n = lws_snprintf(p, len, "[%llu:%04d] %s: ",
					(unsigned long long) now / 10000,
					(int)(now % 10000), log_level_names[n]);
		return n;
	}
#endif
	return 0;
}

#ifndef LWS_PLAT_OPTEE
LWS_VISIBLE void lwsl_emit_stderr(int level, const char *line)
{
#if !defined(LWS_WITH_ESP8266)
	char buf[50];

	lwsl_timestamp(level, buf, sizeof(buf));
	fprintf(stderr, "%s%s", buf, line);
#endif
}
#endif

LWS_VISIBLE void _lws_logv(int filter, const char *format, va_list vl)
{
#if defined(LWS_WITH_ESP8266)
	char buf[128];
#else
	char buf[256];
#endif
	int n;

	if (!(log_level_ & filter))
		return;

	n = vsnprintf(buf, sizeof(buf) - 1, format, vl);
	(void)n;
#if defined(LWS_WITH_ESP8266)
	buf[sizeof(buf) - 1] = '\0';
#else
	/* vnsprintf returns what it would have written, even if truncated */
	if (n > sizeof(buf) - 1)
		n = sizeof(buf) - 1;
	if (n > 0)
		buf[n] = '\0';
#endif

	lwsl_emit(filter, buf);
}

LWS_VISIBLE void _lws_log(int filter, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	_lws_logv(filter, format, ap);
	va_end(ap);
}

LWS_VISIBLE void lws_set_log_level(int level,
				   void (*func)(int level, const char *line))
{
	log_level_ = level;
	if (func)
		lwsl_emit = func;
}

LWS_VISIBLE int lwsl_visible(int level)
{
	return log_level_ & level;
}

LWS_VISIBLE int
lws_is_ssl(struct lws *wsi)
{
#ifdef LWS_OPENSSL_SUPPORT
	return wsi->use_ssl;
#else
	(void)wsi;
	return 0;
#endif
}

#ifdef LWS_OPENSSL_SUPPORT
LWS_VISIBLE SSL*
lws_get_ssl(struct lws *wsi)
{
	return wsi->ssl;
}
#endif

LWS_VISIBLE int
lws_partial_buffered(struct lws *wsi)
{
	return !!wsi->trunc_len;
}

void lws_set_protocol_write_pending(struct lws *wsi,
				    enum lws_pending_protocol_send pend)
{
	lwsl_info("setting pps %d\n", pend);

	if (wsi->pps)
		lwsl_err("pps overwrite\n");
	wsi->pps = pend;
	lws_rx_flow_control(wsi, 0);
	lws_callback_on_writable(wsi);
}

LWS_VISIBLE size_t
lws_get_peer_write_allowance(struct lws *wsi)
{
#ifdef LWS_USE_HTTP2
	/* only if we are using HTTP2 on this connection */
	if (wsi->mode != LWSCM_HTTP2_SERVING)
		return -1;
	/* user is only interested in how much he can send, or that he can't  */
	if (wsi->u.http2.tx_credit <= 0)
		return 0;

	return wsi->u.http2.tx_credit;
#else
	(void)wsi;
	return -1;
#endif
}

LWS_VISIBLE void
lws_union_transition(struct lws *wsi, enum connection_mode mode)
{
	lwsl_debug("%s: %p: mode %d\n", __func__, wsi, mode);
	memset(&wsi->u, 0, sizeof(wsi->u));
	wsi->mode = mode;
}

LWS_VISIBLE struct lws_plat_file_ops *
lws_get_fops(struct lws_context *context)
{
	return (struct lws_plat_file_ops *)context->fops;
}

LWS_VISIBLE LWS_EXTERN struct lws_context *
lws_get_context(const struct lws *wsi)
{
	return wsi->context;
}

LWS_VISIBLE LWS_EXTERN int
lws_get_count_threads(struct lws_context *context)
{
	return context->count_threads;
}

LWS_VISIBLE LWS_EXTERN void *
lws_wsi_user(struct lws *wsi)
{
	return wsi->user_space;
}

LWS_VISIBLE LWS_EXTERN void
lws_set_wsi_user(struct lws *wsi, void *data)
{
	if (wsi->user_space_externally_allocated)
		wsi->user_space = data;
	else
		lwsl_err("%s: Cannot set internally-allocated user_space\n",
			 __func__);
}

LWS_VISIBLE LWS_EXTERN struct lws *
lws_get_parent(const struct lws *wsi)
{
	return wsi->parent;
}

LWS_VISIBLE LWS_EXTERN struct lws *
lws_get_child(const struct lws *wsi)
{
	return wsi->child_list;
}

LWS_VISIBLE LWS_EXTERN void
lws_set_parent_carries_io(struct lws *wsi)
{
	wsi->parent_carries_io = 1;
}

LWS_VISIBLE LWS_EXTERN void *
lws_get_opaque_parent_data(const struct lws *wsi)
{
	return wsi->opaque_parent_data;
}

LWS_VISIBLE LWS_EXTERN void
lws_set_opaque_parent_data(struct lws *wsi, void *data)
{
	wsi->opaque_parent_data = data;
}

LWS_VISIBLE LWS_EXTERN int
lws_get_child_pending_on_writable(const struct lws *wsi)
{
	return wsi->parent_pending_cb_on_writable;
}

LWS_VISIBLE LWS_EXTERN void
lws_clear_child_pending_on_writable(struct lws *wsi)
{
	wsi->parent_pending_cb_on_writable = 0;
}

LWS_VISIBLE LWS_EXTERN int
lws_get_close_length(struct lws *wsi)
{
	return wsi->u.ws.close_in_ping_buffer_len;
}

LWS_VISIBLE LWS_EXTERN unsigned char *
lws_get_close_payload(struct lws *wsi)
{
	return &wsi->u.ws.ping_payload_buf[LWS_PRE];
}

LWS_VISIBLE LWS_EXTERN void
lws_close_reason(struct lws *wsi, enum lws_close_status status,
		 unsigned char *buf, size_t len)
{
	unsigned char *p, *start;
	int budget = sizeof(wsi->u.ws.ping_payload_buf) - LWS_PRE;

	assert(wsi->mode == LWSCM_WS_SERVING || wsi->mode == LWSCM_WS_CLIENT);

	start = p = &wsi->u.ws.ping_payload_buf[LWS_PRE];

	*p++ = (((int)status) >> 8) & 0xff;
	*p++ = ((int)status) & 0xff;

	if (buf)
		while (len-- && p < start + budget)
			*p++ = *buf++;

	wsi->u.ws.close_in_ping_buffer_len = p - start;
}

LWS_EXTERN int
_lws_rx_flow_control(struct lws *wsi)
{
	struct lws *wsic = wsi->child_list;

	/* if he has children, do those if they were changed */
	while (wsic) {
		if (wsic->rxflow_change_to & LWS_RXFLOW_PENDING_CHANGE)
			_lws_rx_flow_control(wsic);

		wsic = wsic->sibling_list;
	}

	/* there is no pending change */
	if (!(wsi->rxflow_change_to & LWS_RXFLOW_PENDING_CHANGE)) {
//		lwsl_debug("%s: no pending change\n", __func__);
		return 0;
	}

	/* stuff is still buffered, not ready to really accept new input */
	if (wsi->rxflow_buffer) {
		/* get ourselves called back to deal with stashed buffer */
		lws_callback_on_writable(wsi);
		return 0;
	}

	/* pending is cleared, we can change rxflow state */

	wsi->rxflow_change_to &= ~LWS_RXFLOW_PENDING_CHANGE;

	lwsl_info("rxflow: wsi %p change_to %d\n", wsi,
			      wsi->rxflow_change_to & LWS_RXFLOW_ALLOW);

	/* adjust the pollfd for this wsi */

	if (wsi->rxflow_change_to & LWS_RXFLOW_ALLOW) {
		if (lws_change_pollfd(wsi, 0, LWS_POLLIN)) {
			lwsl_info("%s: fail\n", __func__);
			return -1;
		}
	} else
		if (lws_change_pollfd(wsi, LWS_POLLIN, 0))
			return -1;

	return 0;
}

LWS_EXTERN int
lws_check_utf8(unsigned char *state, unsigned char *buf, size_t len)
{
	static const unsigned char e0f4[] = {
		0xa0 | ((2 - 1) << 2) | 1, /* e0 */
		0x80 | ((4 - 1) << 2) | 1, /* e1 */
		0x80 | ((4 - 1) << 2) | 1, /* e2 */
		0x80 | ((4 - 1) << 2) | 1, /* e3 */
		0x80 | ((4 - 1) << 2) | 1, /* e4 */
		0x80 | ((4 - 1) << 2) | 1, /* e5 */
		0x80 | ((4 - 1) << 2) | 1, /* e6 */
		0x80 | ((4 - 1) << 2) | 1, /* e7 */
		0x80 | ((4 - 1) << 2) | 1, /* e8 */
		0x80 | ((4 - 1) << 2) | 1, /* e9 */
		0x80 | ((4 - 1) << 2) | 1, /* ea */
		0x80 | ((4 - 1) << 2) | 1, /* eb */
		0x80 | ((4 - 1) << 2) | 1, /* ec */
		0x80 | ((2 - 1) << 2) | 1, /* ed */
		0x80 | ((4 - 1) << 2) | 1, /* ee */
		0x80 | ((4 - 1) << 2) | 1, /* ef */
		0x90 | ((3 - 1) << 2) | 2, /* f0 */
		0x80 | ((4 - 1) << 2) | 2, /* f1 */
		0x80 | ((4 - 1) << 2) | 2, /* f2 */
		0x80 | ((4 - 1) << 2) | 2, /* f3 */
		0x80 | ((1 - 1) << 2) | 2, /* f4 */

		0,			   /* s0 */
		0x80 | ((4 - 1) << 2) | 0, /* s2 */
		0x80 | ((4 - 1) << 2) | 1, /* s3 */
	};
	unsigned char s = *state;

	while (len--) {
		unsigned char c = *buf++;

		if (!s) {
			if (c >= 0x80) {
				if (c < 0xc2 || c > 0xf4)
					return 1;
				if (c < 0xe0)
					s = 0x80 | ((4 - 1) << 2);
				else
					s = e0f4[c - 0xe0];
			}
		} else {
			if (c < (s & 0xf0) ||
			    c >= (s & 0xf0) + 0x10 + ((s << 2) & 0x30))
				return 1;
			s = e0f4[21 + (s & 3)];
		}
	}

	*state = s;

	return 0;
}

LWS_VISIBLE LWS_EXTERN int
lws_parse_uri(char *p, const char **prot, const char **ads, int *port,
	      const char **path)
{
	const char *end;
	static const char *slash = "/";

	/* cut up the location into address, port and path */
	*prot = p;
	while (*p && (*p != ':' || p[1] != '/' || p[2] != '/'))
		p++;
	if (!*p) {
		end = p;
		p = (char *)*prot;
		*prot = end;
	} else {
		*p = '\0';
		p += 3;
	}
	*ads = p;
	if (!strcmp(*prot, "http") || !strcmp(*prot, "ws"))
		*port = 80;
	else if (!strcmp(*prot, "https") || !strcmp(*prot, "wss"))
		*port = 443;

       if (*p == '[')
       {
               ++(*ads);
               while (*p && *p != ']')
                       p++;
               if (*p)
                       *p++ = '\0';
       }
       else
       {
               while (*p && *p != ':' && *p != '/')
                       p++;
       }
	if (*p == ':') {
		*p++ = '\0';
		*port = atoi(p);
		while (*p && *p != '/')
			p++;
	}
	*path = slash;
	if (*p) {
		*p++ = '\0';
		if (*p)
			*path = p;
	}

	return 0;
}

#ifdef LWS_NO_EXTENSIONS

/* we need to provide dummy callbacks for internal exts
 * so user code runs when faced with a lib compiled with
 * extensions disabled.
 */

int
lws_extension_callback_pm_deflate(struct lws_context *context,
                                  const struct lws_extension *ext,
                                  struct lws *wsi,
                                  enum lws_extension_callback_reasons reason,
                                  void *user, void *in, size_t len)
{
	(void)context;
	(void)ext;
	(void)wsi;
	(void)reason;
	(void)user;
	(void)in;
	(void)len;

	return 0;
}
#endif

LWS_EXTERN int
lws_socket_bind(struct lws_vhost *vhost, lws_sockfd_type sockfd, int port,
		const char *iface)
{
#if LWS_POSIX
#ifdef LWS_USE_UNIX_SOCK
	struct sockaddr_un serv_unix;
#endif
#ifdef LWS_USE_IPV6
	struct sockaddr_in6 serv_addr6;
#endif
	struct sockaddr_in serv_addr4;
#ifndef LWS_PLAT_OPTEE
	socklen_t len = sizeof(struct sockaddr_storage);
#endif
	int n;
	struct sockaddr_storage sin;
	struct sockaddr *v;

#ifdef LWS_USE_UNIX_SOCK
	if (LWS_UNIX_SOCK_ENABLED(vhost)) {
		v = (struct sockaddr *)&serv_unix;
		n = sizeof(struct sockaddr_un);
		bzero((char *) &serv_unix, sizeof(serv_unix));
		serv_unix.sun_family = AF_UNIX;
		if (sizeof(serv_unix.sun_path) <= strlen(iface)) {
			lwsl_err("\"%s\" too long for UNIX domain socket\n",
			         iface);
			return -1;
		}
		strcpy(serv_unix.sun_path, iface);
		if (serv_unix.sun_path[0] == '@')
			serv_unix.sun_path[0] = '\0';

	} else
#endif
#if defined(LWS_USE_IPV6) && !defined(LWS_WITH_ESP32)
	if (LWS_IPV6_ENABLED(vhost)) {
		v = (struct sockaddr *)&serv_addr6;
		n = sizeof(struct sockaddr_in6);
		bzero((char *) &serv_addr6, sizeof(serv_addr6));
		if (iface) {
			if (interface_to_sa(vhost, iface,
				    (struct sockaddr_in *)v, n) < 0) {
				lwsl_err("Unable to find interface %s\n", iface);
				return -1;
			}
			serv_addr6.sin6_scope_id = lws_get_addr_scope(iface);
		}

		serv_addr6.sin6_family = AF_INET6;
		serv_addr6.sin6_port = htons(port);
	} else
#endif
	{
		v = (struct sockaddr *)&serv_addr4;
		n = sizeof(serv_addr4);
		bzero((char *) &serv_addr4, sizeof(serv_addr4));
		serv_addr4.sin_addr.s_addr = INADDR_ANY;
		serv_addr4.sin_family = AF_INET;
#if !defined(LWS_WITH_ESP32)

		if (iface &&
		    interface_to_sa(vhost, iface,
				    (struct sockaddr_in *)v, n) < 0) {
			lwsl_err("Unable to find interface %s\n", iface);
			return -1;
		}
#endif
		serv_addr4.sin_port = htons(port);
	} /* ipv4 */

	n = bind(sockfd, v, n);
#ifdef LWS_USE_UNIX_SOCK
	if (n < 0 && LWS_UNIX_SOCK_ENABLED(vhost)) {
		lwsl_err("ERROR on binding fd %d to \"%s\" (%d %d)\n",
				sockfd, iface, n, LWS_ERRNO);
		return -1;
	} else
#endif
	if (n < 0) {
		lwsl_err("ERROR on binding fd %d to port %d (%d %d)\n",
				sockfd, port, n, LWS_ERRNO);
		return -1;
	}

#ifndef LWS_PLAT_OPTEE
	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
		lwsl_warn("getsockname: %s\n", strerror(LWS_ERRNO));
	else
#endif
#if defined(LWS_USE_IPV6)
		port = (sin.ss_family == AF_INET6) ?
				  ntohs(((struct sockaddr_in6 *) &sin)->sin6_port) :
				  ntohs(((struct sockaddr_in *) &sin)->sin_port);
#else
		port = ntohs(((struct sockaddr_in *) &sin)->sin_port);
#endif
#endif

	return port;
}

#if defined(LWS_USE_IPV6)
LWS_EXTERN unsigned long
lws_get_addr_scope(const char *ipaddr)
{
	unsigned long scope = 0;

#ifndef WIN32
	struct ifaddrs *addrs, *addr;
	char ip[NI_MAXHOST];
	unsigned int i;

	getifaddrs(&addrs);
	for (addr = addrs; addr; addr = addr->ifa_next) {
		if (!addr->ifa_addr ||
			addr->ifa_addr->sa_family != AF_INET6)
			continue;

		getnameinfo(addr->ifa_addr,
				sizeof(struct sockaddr_in6),
				ip, sizeof(ip),
				NULL, 0, NI_NUMERICHOST);

		i = 0;
		while (ip[i])
			if (ip[i++] == '%') {
				ip[i - 1] = '\0';
				break;
			}

		if (!strcmp(ip, ipaddr)) {
			scope = if_nametoindex(addr->ifa_name);
			break;
		}
	}
	freeifaddrs(addrs);
#else
	PIP_ADAPTER_ADDRESSES adapter, addrs = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS addr;
	ULONG size = 0;
	DWORD ret;
	struct sockaddr_in6 *sockaddr;
	char ip[NI_MAXHOST];
	unsigned int i;
	int found = 0;

	for (i = 0; i < 5; i++)
	{
		ret = GetAdaptersAddresses(AF_INET6, GAA_FLAG_INCLUDE_PREFIX,
				NULL, addrs, &size);
		if ((ret == NO_ERROR) || (ret == ERROR_NO_DATA)) {
			break;
		} else if (ret == ERROR_BUFFER_OVERFLOW)
		{
			if (addrs)
				free(addrs);
			addrs = (IP_ADAPTER_ADDRESSES *) malloc(size);
		} else
		{
			if (addrs)
			{
				free(addrs);
				addrs = NULL;
			}
			lwsl_err("Failed to get IPv6 address table (%d)", ret);
			break;
		}
	}

	if ((ret == NO_ERROR) && (addrs))
	{
		adapter = addrs;
		while ((adapter) && (!found))
		{
			addr = adapter->FirstUnicastAddress;
			while ((addr) && (!found))
			{
				if (addr->Address.lpSockaddr->sa_family == AF_INET6)
				{
					sockaddr = (struct sockaddr_in6 *) (addr->Address.lpSockaddr);

					lws_plat_inet_ntop(sockaddr->sin6_family, &sockaddr->sin6_addr,
							ip, sizeof(ip));

					if (!strcmp(ip, ipaddr)) {
						scope = sockaddr->sin6_scope_id;
						found = 1;
						break;
					}
				}
				addr = addr->Next;
			}
			adapter = adapter->Next;
		}
	}
	if (addrs)
		free(addrs);
#endif

	return scope;
}
#endif

LWS_EXTERN void
lws_restart_ws_ping_pong_timer(struct lws *wsi)
{
	if (!wsi->context->ws_ping_pong_interval)
		return;
	if (wsi->state != LWSS_ESTABLISHED)
		return;

	wsi->u.ws.time_next_ping_check = (time_t)lws_now_secs() +
				    wsi->context->ws_ping_pong_interval;
}

static const char *hex = "0123456789ABCDEF";

LWS_VISIBLE LWS_EXTERN const char *
lws_sql_purify(char *escaped, const char *string, int len)
{
	const char *p = string;
	char *q = escaped;

	while (*p && len-- > 2) {
		if (*p == '\'') {
			*q++ = '\'';
			*q++ = '\'';
			len --;
			p++;
		} else
			*q++ = *p++;
	}
	*q = '\0';

	return escaped;
}

LWS_VISIBLE LWS_EXTERN const char *
lws_json_purify(char *escaped, const char *string, int len)
{
	const char *p = string;
	char *q = escaped;

	if (!p) {
		escaped[0] = '\0';
		return escaped;
	}

	while (*p && len-- > 6) {
		if (*p == '\"' || *p == '\\' || *p < 0x20) {
			*q++ = '\\';
			*q++ = 'u';
			*q++ = '0';
			*q++ = '0';
			*q++ = hex[((*p) >> 4) & 15];
			*q++ = hex[(*p) & 15];
			len -= 5;
			p++;
		} else
			*q++ = *p++;
	}
	*q = '\0';

	return escaped;
}

LWS_VISIBLE LWS_EXTERN const char *
lws_urlencode(char *escaped, const char *string, int len)
{
	const char *p = string;
	char *q = escaped;

	while (*p && len-- > 3) {
		if (*p == ' ') {
			*q++ = '+';
			p++;
			continue;
		}
		if ((*p >= '0' && *p <= '9') ||
		    (*p >= 'A' && *p <= 'Z') ||
		    (*p >= 'a' && *p <= 'z')) {
			*q++ = *p++;
			continue;
		}
		*q++ = '%';
		*q++ = hex[(*p >> 4) & 0xf];
		*q++ = hex[*p & 0xf];

		len -= 2;
		p++;
	}
	*q = '\0';

	return escaped;
}

LWS_VISIBLE LWS_EXTERN int
lws_urldecode(char *string, const char *escaped, int len)
{
	int state = 0, n;
	char sum = 0;

	while (*escaped && len) {
		switch (state) {
		case 0:
			if (*escaped == '%') {
				state++;
				escaped++;
				continue;
			}
			if (*escaped == '+') {
				escaped++;
				*string++ = ' ';
				len--;
				continue;
			}
			*string++ = *escaped++;
			len--;
			break;
		case 1:
			n = char_to_hex(*escaped);
			if (n < 0)
				return -1;
			escaped++;
			sum = n << 4;
			state++;
			break;

		case 2:
			n = char_to_hex(*escaped);
			if (n < 0)
				return -1;
			escaped++;
			*string++ = sum | n;
			len--;
			state = 0;
			break;
		}

	}
	*string = '\0';

	return 0;
}

LWS_VISIBLE LWS_EXTERN int
lws_finalize_startup(struct lws_context *context)
{
	struct lws_context_creation_info info;

	info.uid = context->uid;
	info.gid = context->gid;

#if defined(LWS_HAVE_SYS_CAPABILITY_H) && defined(LWS_HAVE_LIBCAP)
	memcpy(info.caps, context->caps, sizeof(info.caps));
	info.count_caps = context->count_caps;
#endif

	if (lws_check_opt(context->options, LWS_SERVER_OPTION_EXPLICIT_VHOSTS))
		lws_plat_drop_app_privileges(&info);

	return 0;
}

int
lws_snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;
	int n;

	if (!size)
		return 0;

	va_start(ap, format);
	n = vsnprintf(str, size, format, ap);
	va_end(ap);

	if (n >= (int)size)
		return size;

	return n;
}


LWS_VISIBLE LWS_EXTERN int
lws_is_cgi(struct lws *wsi) {
#ifdef LWS_WITH_CGI
	return !!wsi->cgi;
#else
	return 0;
#endif
}

#ifdef LWS_WITH_CGI

static int
urlencode(const char *in, int inlen, char *out, int outlen)
{
	char *start = out, *end = out + outlen;

	while (inlen-- && out < end - 4) {
		if ((*in >= 'A' && *in <= 'Z') ||
		    (*in >= 'a' && *in <= 'z') ||
		    (*in >= '0' && *in <= '9') ||
		    *in == '-' ||
		    *in == '_' ||
		    *in == '.' ||
		    *in == '~') {
			*out++ = *in++;
			continue;
		}
		if (*in == ' ') {
			*out++ = '+';
			in++;
			continue;
		}
		*out++ = '%';
		*out++ = hex[(*in) >> 4];
		*out++ = hex[(*in++) & 15];
	}
	*out = '\0';

	if (out >= end - 4)
		return -1;

	return out - start;
}

static struct lws *
lws_create_basic_wsi(struct lws_context *context, int tsi)
{
	struct lws *new_wsi;

	if ((unsigned int)context->pt[tsi].fds_count ==
	    context->fd_limit_per_thread - 1) {
		lwsl_err("no space for new conn\n");
		return NULL;
	}

	new_wsi = lws_zalloc(sizeof(struct lws));
	if (new_wsi == NULL) {
		lwsl_err("Out of memory for new connection\n");
		return NULL;
	}

	new_wsi->tsi = tsi;
	new_wsi->context = context;
	new_wsi->pending_timeout = NO_PENDING_TIMEOUT;
	new_wsi->rxflow_change_to = LWS_RXFLOW_ALLOW;

	/* initialize the instance struct */

	new_wsi->state = LWSS_CGI;
	new_wsi->mode = LWSCM_CGI;
	new_wsi->hdr_parsing_completed = 0;
	new_wsi->position_in_fds_table = -1;

	/*
	 * these can only be set once the protocol is known
	 * we set an unestablished connection's protocol pointer
	 * to the start of the defauly vhost supported list, so it can look
	 * for matching ones during the handshake
	 */
	new_wsi->protocol = context->vhost_list->protocols;
	new_wsi->user_space = NULL;
	new_wsi->ietf_spec_revision = 0;
	new_wsi->desc.sockfd = LWS_SOCK_INVALID;
	context->count_wsi_allocated++;

	return new_wsi;
}

LWS_VISIBLE LWS_EXTERN int
lws_cgi(struct lws *wsi, const char * const *exec_array, int script_uri_path_len,
	int timeout_secs, const struct lws_protocol_vhost_options *mp_cgienv)
{
	struct lws_context_per_thread *pt = &wsi->context->pt[(int)wsi->tsi];
	char *env_array[30], cgi_path[400], e[1024], *p = e,
	     *end = p + sizeof(e) - 1, tok[256], *t;
	struct lws_cgi *cgi;
	int n, m, i, uritok = -1;

	/*
	 * give the master wsi a cgi struct
	 */

	wsi->cgi = lws_zalloc(sizeof(*wsi->cgi));
	if (!wsi->cgi) {
		lwsl_err("%s: OOM\n", __func__);
		return -1;
	}

	wsi->cgi->response_code = HTTP_STATUS_OK;

	cgi = wsi->cgi;
	cgi->wsi = wsi; /* set cgi's owning wsi */

	/* create pipes for [stdin|stdout] and [stderr] */

	for (n = 0; n < 3; n++)
		if (pipe(cgi->pipe_fds[n]) == -1)
			goto bail1;

	/* create cgi wsis for each stdin/out/err fd */

	for (n = 0; n < 3; n++) {
		cgi->stdwsi[n] = lws_create_basic_wsi(wsi->context, wsi->tsi);
		if (!cgi->stdwsi[n])
			goto bail2;
		cgi->stdwsi[n]->cgi_channel = n;
		cgi->stdwsi[n]->vhost = wsi->vhost;

//		lwsl_err("%s: cgi %p: pipe fd %d -> fd %d / %d\n", __func__, wsi, n,
//			 cgi->pipe_fds[n][!!(n == 0)], cgi->pipe_fds[n][!(n == 0)]);

		/* read side is 0, stdin we want the write side, others read */
		cgi->stdwsi[n]->desc.sockfd = cgi->pipe_fds[n][!!(n == 0)];
		if (fcntl(cgi->pipe_fds[n][!!(n == 0)], F_SETFL, O_NONBLOCK) < 0) {
			lwsl_err("%s: setting NONBLOCK failed\n", __func__);
			goto bail2;
		}
	}

	for (n = 0; n < 3; n++) {
		lws_libuv_accept(cgi->stdwsi[n], cgi->stdwsi[n]->desc);
		if (insert_wsi_socket_into_fds(wsi->context, cgi->stdwsi[n]))
			goto bail3;
		cgi->stdwsi[n]->parent = wsi;
		cgi->stdwsi[n]->sibling_list = wsi->child_list;
		wsi->child_list = cgi->stdwsi[n];
	}

	lws_change_pollfd(cgi->stdwsi[LWS_STDIN], LWS_POLLIN, LWS_POLLOUT);
	lws_change_pollfd(cgi->stdwsi[LWS_STDOUT], LWS_POLLOUT, LWS_POLLIN);
	lws_change_pollfd(cgi->stdwsi[LWS_STDERR], LWS_POLLOUT, LWS_POLLIN);

	lwsl_debug("%s: fds in %d, out %d, err %d\n", __func__,
		   cgi->stdwsi[LWS_STDIN]->desc.sockfd,
		   cgi->stdwsi[LWS_STDOUT]->desc.sockfd,
		   cgi->stdwsi[LWS_STDERR]->desc.sockfd);

	lws_set_timeout(wsi, PENDING_TIMEOUT_CGI, timeout_secs);

	/* the cgi stdout is always sending us http1.x header data first */
	wsi->hdr_state = LCHS_HEADER;

	/* add us to the pt list of active cgis */
	lwsl_debug("%s: adding cgi %p to list\n", __func__, wsi->cgi);
	cgi->cgi_list = pt->cgi_list;
	pt->cgi_list = cgi;

	/* prepare his CGI env */

	n = 0;

	if (lws_is_ssl(wsi))
		env_array[n++] = "HTTPS=ON";
	if (wsi->u.hdr.ah) {
		static const unsigned char meths[] = {
			WSI_TOKEN_GET_URI,
			WSI_TOKEN_POST_URI,
			WSI_TOKEN_OPTIONS_URI,
			WSI_TOKEN_PUT_URI,
			WSI_TOKEN_PATCH_URI,
			WSI_TOKEN_DELETE_URI,
		};
		static const char * const meth_names[] = {
			"GET", "POST", "OPTIONS", "PUT", "PATCH", "DELETE",
		};

		for (m = 0; m < ARRAY_SIZE(meths); m++)
			if (lws_hdr_total_length(wsi, meths[m]) >=
					script_uri_path_len) {
				uritok = meths[m];
				break;
			}

		if (uritok < 0)
			goto bail3;

		lws_snprintf(cgi_path, sizeof(cgi_path) - 1, "REQUEST_URI=%s",
			 lws_hdr_simple_ptr(wsi, uritok));
		cgi_path[sizeof(cgi_path) - 1] = '\0';
		env_array[n++] = cgi_path;

		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "REQUEST_METHOD=%s",
			      meth_names[m]);
		p++;

		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "QUERY_STRING=");
		/* dump the individual URI Arg parameters */
		m = 0;
		while (1) {
			i = lws_hdr_copy_fragment(wsi, tok, sizeof(tok),
					     WSI_TOKEN_HTTP_URI_ARGS, m);
			if (i < 0)
				break;
			t = tok;
			while (*t && *t != '=' && p < end - 4)
				*p++ = *t++;
			if (*t == '=')
				*p++ = *t++;
			i = urlencode(t, i- (t - tok), p, end - p);
			if (i > 0) {
				p += i;
				*p++ = '&';
			}
			m++;
		}
		if (m)
			p--;
		*p++ = '\0';

		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "PATH_INFO=%s",
			      lws_hdr_simple_ptr(wsi, uritok) +
			      script_uri_path_len);
		p++;
	}
	if (lws_hdr_total_length(wsi, WSI_TOKEN_HTTP_REFERER)) {
		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "HTTP_REFERER=%s",
			      lws_hdr_simple_ptr(wsi, WSI_TOKEN_HTTP_REFERER));
		p++;
	}
	if (lws_hdr_total_length(wsi, WSI_TOKEN_HOST)) {
		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "HTTP_HOST=%s",
			      lws_hdr_simple_ptr(wsi, WSI_TOKEN_HOST));
		p++;
	}
	if (lws_hdr_total_length(wsi, WSI_TOKEN_HTTP_COOKIE)) {
		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "HTTP_COOKIE=%s",
			      lws_hdr_simple_ptr(wsi, WSI_TOKEN_HTTP_COOKIE));
		p++;
	}
	if (lws_hdr_total_length(wsi, WSI_TOKEN_HTTP_USER_AGENT)) {
		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "USER_AGENT=%s",
			      lws_hdr_simple_ptr(wsi, WSI_TOKEN_HTTP_USER_AGENT));
		p++;
	}
	if (uritok == WSI_TOKEN_POST_URI) {
		if (lws_hdr_total_length(wsi, WSI_TOKEN_HTTP_CONTENT_TYPE)) {
			env_array[n++] = p;
			p += lws_snprintf(p, end - p, "CONTENT_TYPE=%s",
				      lws_hdr_simple_ptr(wsi, WSI_TOKEN_HTTP_CONTENT_TYPE));
			p++;
		}
		if (lws_hdr_total_length(wsi, WSI_TOKEN_HTTP_CONTENT_LENGTH)) {
			env_array[n++] = p;
			p += lws_snprintf(p, end - p, "CONTENT_LENGTH=%s",
				      lws_hdr_simple_ptr(wsi, WSI_TOKEN_HTTP_CONTENT_LENGTH));
			p++;
		}
	}
	env_array[n++] = p;
	p += lws_snprintf(p, end - p, "SCRIPT_PATH=%s", exec_array[0]) + 1;

	while (mp_cgienv) {
		env_array[n++] = p;
		p += lws_snprintf(p, end - p, "%s=%s", mp_cgienv->name,
			      mp_cgienv->value);
		lwsl_debug("   Applying mount-specific cgi env '%s'\n",
			   env_array[n - 1]);
		p++;
		mp_cgienv = mp_cgienv->next;
	}

	env_array[n++] = "SERVER_SOFTWARE=libwebsockets";
	env_array[n++] = "PATH=/bin:/usr/bin:/usr/local/bin:/var/www/cgi-bin";
	env_array[n] = NULL;

#if 0
	for (m = 0; m < n; m++)
		lwsl_err("    %s\n", env_array[m]);
#endif

	/*
	 * Actually having made the env, as a cgi we don't need the ah
	 * any more
	 */
	if (lws_header_table_is_in_detachable_state(wsi))
		lws_header_table_detach(wsi, 0);

	/* we are ready with the redirection pipes... run the thing */
#if !defined(LWS_HAVE_VFORK) || !defined(LWS_HAVE_EXECVPE)
	cgi->pid = fork();
#else
	cgi->pid = vfork();
#endif
	if (cgi->pid < 0) {
		lwsl_err("fork failed, errno %d", errno);
		goto bail3;
	}

#if defined(__linux__)
	prctl(PR_SET_PDEATHSIG, SIGTERM);
#endif
	setpgrp(); /* stops on-daemonized main processess getting SIGINT from TTY */

	if (cgi->pid) {
		/* we are the parent process */
		wsi->context->count_cgi_spawned++;
		lwsl_debug("%s: cgi %p spawned PID %d\n", __func__, cgi, cgi->pid);
		return 0;
	}

	/* somewhere we can at least read things and enter it */
	if (chdir("/tmp"))
		lwsl_notice("%s: Failed to chdir\n", __func__);

	/* We are the forked process, redirect and kill inherited things.
	 *
	 * Because of vfork(), we cannot do anything that changes pages in
	 * the parent environment.  Stuff that changes kernel state for the
	 * process is OK.  Stuff that happens after the execvpe() is OK.
	 */

	for (n = 0; n < 3; n++) {
		if (dup2(cgi->pipe_fds[n][!(n == 0)], n) < 0) {
			lwsl_err("%s: stdin dup2 failed\n", __func__);
			goto bail3;
		}
		close(cgi->pipe_fds[n][!(n == 0)]);
	}

#if !defined(LWS_HAVE_VFORK) || !defined(LWS_HAVE_EXECVPE)
	for (m = 0; m < n; m++) {
		p = strchr(env_array[m], '=');
		*p++ = '\0';
		setenv(env_array[m], p, 1);
	}
	execvp(exec_array[0], (char * const *)&exec_array[0]);
#else
	execvpe(exec_array[0], (char * const *)&exec_array[0], &env_array[0]);
#endif

	exit(1);

bail3:
	/* drop us from the pt cgi list */
	pt->cgi_list = cgi->cgi_list;

	while (--n >= 0)
		remove_wsi_socket_from_fds(wsi->cgi->stdwsi[n]);
bail2:
	for (n = 0; n < 3; n++)
		if (wsi->cgi->stdwsi[n])
			lws_free_wsi(cgi->stdwsi[n]);

bail1:
	for (n = 0; n < 3; n++) {
		if (cgi->pipe_fds[n][0])
			close(cgi->pipe_fds[n][0]);
		if (cgi->pipe_fds[n][1])
			close(cgi->pipe_fds[n][1]);
	}

	lws_free_set_NULL(wsi->cgi);

	lwsl_err("%s: failed\n", __func__);

	return -1;
}

/* we have to parse out these headers in the CGI output */

static const char * const significant_hdr[SIGNIFICANT_HDR_COUNT] = {
	"content-length: ",
	"location: ",
	"status: ",
	"transfer-encoding: chunked",
};

LWS_VISIBLE LWS_EXTERN int
lws_cgi_write_split_stdout_headers(struct lws *wsi)
{
	int n, m;
	unsigned char buf[LWS_PRE + 1024], *start = &buf[LWS_PRE], *p = start,
	     *end = &buf[sizeof(buf) - 1 - LWS_PRE];
	char c;

	if (!wsi->cgi)
		return -1;

	while (wsi->hdr_state != LHCS_PAYLOAD) {
		/* we have to separate header / finalize and
		 * payload chunks, since they need to be
		 * handled separately
		 */

		switch (wsi->hdr_state) {

		case LHCS_RESPONSE:
			lwsl_info("LHCS_RESPONSE: issuing response %d\n",
					wsi->cgi->response_code);
			if (lws_add_http_header_status(wsi, wsi->cgi->response_code, &p, end))
				return 1;
			if (lws_add_http_header_by_token(wsi, WSI_TOKEN_CONNECTION,
					(unsigned char *)"close", 5, &p, end))
				return 1;
			n = lws_write(wsi, start, p - start,
				      LWS_WRITE_HTTP_HEADERS);

			/* finalize cached headers before dumping them */
			if (lws_finalize_http_header(wsi,
					(unsigned char **)&wsi->cgi->headers_pos,
					(unsigned char *)wsi->cgi->headers_end)) {

				lwsl_notice("finalize failed\n");
				return -1;
			}

			wsi->hdr_state = LHCS_DUMP_HEADERS;
			wsi->reason_bf |= 8;
			lws_callback_on_writable(wsi);
			/* back to the loop for writeability again */
			return 0;

		case LHCS_DUMP_HEADERS:

			n = wsi->cgi->headers_pos - wsi->cgi->headers_dumped;
			if (n > 512)
				n = 512;

			lwsl_debug("LHCS_DUMP_HEADERS: %d\n", n);

			m = lws_write(wsi, (unsigned char *)wsi->cgi->headers_dumped,
				      n, LWS_WRITE_HTTP_HEADERS);
			if (m < 0) {
				lwsl_debug("%s: write says %d\n", __func__, m);
				return -1;
			}
			wsi->cgi->headers_dumped += n;
			if (wsi->cgi->headers_dumped == wsi->cgi->headers_pos) {
				wsi->hdr_state = LHCS_PAYLOAD;
				lws_free_set_NULL(wsi->cgi->headers_buf);
				lwsl_debug("freed cgi headers\n");
			} else {
				wsi->reason_bf |= 8;
				lws_callback_on_writable(wsi);
			}

			/* writeability becomes uncertain now we wrote
			 * something, we must return to the event loop
			 */
			return 0;
		}

		if (!wsi->cgi->headers_buf) {
			/* if we don't already have a headers buf, cook one up */
			n = 2048;
			wsi->cgi->headers_buf = malloc(n);
			if (!wsi->cgi->headers_buf) {
				lwsl_err("OOM\n");
				return -1;
			}

			lwsl_debug("allocated cgi hdrs\n");
			wsi->cgi->headers_pos = wsi->cgi->headers_buf;
			wsi->cgi->headers_dumped = wsi->cgi->headers_pos;
			wsi->cgi->headers_end = wsi->cgi->headers_buf + n - 1;

			for (n = 0; n < SIGNIFICANT_HDR_COUNT; n++) {
				wsi->cgi->match[n] = 0;
				wsi->cgi->lp = 0;
			}
		}

		n = read(lws_get_socket_fd(wsi->cgi->stdwsi[LWS_STDOUT]), &c, 1);
		if (n < 0) {
			if (errno != EAGAIN) {
				lwsl_debug("%s: read says %d\n", __func__, n);
				return -1;
			}
			else
				n = 0;

			if (wsi->cgi->headers_pos >= wsi->cgi->headers_end - 4) {
				lwsl_notice("CGI headers larger than buffer size\n");

				return -1;
			}
		}
		if (n) {
			lwsl_debug("-- 0x%02X %c %d %d\n", (unsigned char)c, c, wsi->cgi->match[1], wsi->hdr_state);
			if (!c)
				return -1;
			switch (wsi->hdr_state) {
			case LCHS_HEADER:
				hdr:
				for (n = 0; n < SIGNIFICANT_HDR_COUNT; n++) {
					/* significant headers with numeric decimal payloads */
					if (!significant_hdr[n][wsi->cgi->match[n]] &&
					    (c >= '0' && c <= '9') &&
					    wsi->cgi->lp < sizeof(wsi->cgi->l) - 1) {
						wsi->cgi->l[wsi->cgi->lp++] = c;
						wsi->cgi->l[wsi->cgi->lp] = '\0';
						switch (n) {
						case SIGNIFICANT_HDR_CONTENT_LENGTH:
							wsi->cgi->content_length = atoll(wsi->cgi->l);
							break;
						case SIGNIFICANT_HDR_STATUS:
							wsi->cgi->response_code = atol(wsi->cgi->l);
							lwsl_debug("Status set to %d\n", wsi->cgi->response_code);
							break;
						default:
							break;
						}
					}
					/* hits up to the NUL are sticky until next hdr */
					if (significant_hdr[n][wsi->cgi->match[n]]) {
						if (tolower(c) == significant_hdr[n][wsi->cgi->match[n]])
							wsi->cgi->match[n]++;
						else
							wsi->cgi->match[n] = 0;
					}
				}

				/* some cgi only send us \x0a for EOL */
				if (c == '\x0a') {
					wsi->hdr_state = LCHS_SINGLE_0A;
					*wsi->cgi->headers_pos++ = '\x0d';
				}
				*wsi->cgi->headers_pos++ = c;
				if (c == '\x0d')
					wsi->hdr_state = LCHS_LF1;

				if (wsi->hdr_state != LCHS_HEADER &&
				    !significant_hdr[SIGNIFICANT_HDR_TRANSFER_ENCODING][wsi->cgi->match[SIGNIFICANT_HDR_TRANSFER_ENCODING]]) {
					lwsl_debug("cgi produced chunked\n");
					wsi->cgi->explicitly_chunked = 1;
				}

				/* presence of Location: mandates 302 retcode */
				if (wsi->hdr_state != LCHS_HEADER &&
				    !significant_hdr[SIGNIFICANT_HDR_LOCATION][wsi->cgi->match[SIGNIFICANT_HDR_LOCATION]]) {
					lwsl_debug("CGI: Location hdr seen\n");
					wsi->cgi->response_code = 302;
				}

				break;
			case LCHS_LF1:
				*wsi->cgi->headers_pos++ = c;
				if (c == '\x0a') {
					wsi->hdr_state = LCHS_CR2;
					break;
				}
				/* we got \r[^\n]... it's unreasonable */
				lwsl_debug("%s: funny CRLF 0x%02X\n", __func__, (unsigned char)c);
				return -1;

			case LCHS_CR2:
				if (c == '\x0d') {
					/* drop the \x0d */
					wsi->hdr_state = LCHS_LF2;
					break;
				}
				wsi->hdr_state = LCHS_HEADER;
				for (n = 0; n < SIGNIFICANT_HDR_COUNT; n++)
					wsi->cgi->match[n] = 0;
				wsi->cgi->lp = 0;
				goto hdr;

			case LCHS_LF2:
			case LCHS_SINGLE_0A:
				m = wsi->hdr_state;
				if (c == '\x0a') {
					lwsl_debug("Content-Length: %lld\n", (unsigned long long)wsi->cgi->content_length);
					wsi->hdr_state = LHCS_RESPONSE;
					/* drop the \0xa ... finalize will add it if needed */
					break;
				}
				if (m == LCHS_LF2)
					/* we got \r\n\r[^\n]... it's unreasonable */
					return -1;
				/* we got \x0anext header, it's reasonable */
				*wsi->cgi->headers_pos++ = c;
				wsi->hdr_state = LCHS_HEADER;
				for (n = 0; n < SIGNIFICANT_HDR_COUNT; n++)
					wsi->cgi->match[n] = 0;
				wsi->cgi->lp = 0;
				break;
			case LHCS_PAYLOAD:
				break;
			}
		}

		/* ran out of input, ended the headers, or filled up the headers buf */
		if (!n || wsi->hdr_state == LHCS_PAYLOAD)
			return 0;
	}

	/* payload processing */

	n = read(lws_get_socket_fd(wsi->cgi->stdwsi[LWS_STDOUT]),
		 start, sizeof(buf) - LWS_PRE);

	if (n < 0 && errno != EAGAIN) {
		lwsl_debug("%s: stdout read says %d\n", __func__, n);
		return -1;
	}
	if (n > 0) {
		m = lws_write(wsi, (unsigned char *)start, n, LWS_WRITE_HTTP);
		//lwsl_notice("write %d\n", m);
		if (m < 0) {
			lwsl_debug("%s: stdout write says %d\n", __func__, m);
			return -1;
		}
		wsi->cgi->content_length_seen += m;
	}

	return 0;
}

LWS_VISIBLE LWS_EXTERN int
lws_cgi_kill(struct lws *wsi)
{
	struct lws_cgi_args args;
	int status, n;

	lwsl_debug("%s: %p\n", __func__, wsi);

	if (!wsi->cgi)
		return 0;

	if (wsi->cgi->pid > 0) {
		n = waitpid(wsi->cgi->pid, &status, WNOHANG);
		if (n > 0) {
			lwsl_debug("%s: PID %d reaped\n", __func__,
				    wsi->cgi->pid);
			goto handled;
		}
		/* kill the process group */
		n = kill(-wsi->cgi->pid, SIGTERM);
		lwsl_debug("%s: SIGTERM child PID %d says %d (errno %d)\n", __func__,
				wsi->cgi->pid, n, errno);
		if (n < 0) {
			/*
			 * hum seen errno=3 when process is listed in ps,
			 * it seems we don't always retain process grouping
			 *
			 * Direct these fallback attempt to the exact child
			 */
			n = kill(wsi->cgi->pid, SIGTERM);
			if (n < 0) {
				n = kill(wsi->cgi->pid, SIGPIPE);
				if (n < 0) {
					n = kill(wsi->cgi->pid, SIGKILL);
					if (n < 0)
						lwsl_err("%s: SIGKILL PID %d failed errno %d (maybe zombie)\n",
								__func__, wsi->cgi->pid, errno);
				}
			}
		}
		/* He could be unkillable because he's a zombie */
		n = 1;
		while (n > 0) {
			n = waitpid(-wsi->cgi->pid, &status, WNOHANG);
			if (n > 0)
				lwsl_debug("%s: reaped PID %d\n", __func__, n);
			if (n <= 0) {
				n = waitpid(wsi->cgi->pid, &status, WNOHANG);
				if (n > 0)
					lwsl_debug("%s: reaped PID %d\n", __func__, n);
			}
		}
	}

handled:
	args.stdwsi = &wsi->cgi->stdwsi[0];

	if (wsi->cgi->pid != -1 && user_callback_handle_rxflow(
			wsi->protocol->callback,
			wsi, LWS_CALLBACK_CGI_TERMINATED,
			wsi->user_space,
			(void *)&args, 0)) {
		wsi->cgi->pid = -1;
		if (!wsi->cgi->being_closed)
			lws_close_free_wsi(wsi, 0);
	}

	return 0;
}

LWS_EXTERN int
lws_cgi_kill_terminated(struct lws_context_per_thread *pt)
{
	struct lws_cgi **pcgi, *cgi = NULL;
	int status, n = 1;

	while (n > 0) {
		/* find finished guys but don't reap yet */
		n = waitpid(-1, &status, WNOHANG);
		if (n <= 0)
			continue;
		lwsl_debug("%s: observed PID %d terminated\n", __func__, n);

		pcgi = &pt->cgi_list;

		/* check all the subprocesses on the cgi list */
		while (*pcgi) {
			/* get the next one first as list may change */
			cgi = *pcgi;
			pcgi = &(*pcgi)->cgi_list;

			if (cgi->pid <= 0)
				continue;

			/* finish sending cached headers */
			if (cgi->headers_buf)
				continue;

			/* wait for stdout to be drained */
			if (cgi->content_length > cgi->content_length_seen)
				continue;

			if (cgi->content_length) {
				lwsl_debug("%s: wsi %p: expected content length seen: %lld\n",
					__func__, cgi->wsi, (unsigned long long)cgi->content_length_seen);
			}

			/* reap it */
			waitpid(n, &status, WNOHANG);
			/*
			 * he's already terminated so no need for kill()
			 * but we should do the terminated cgi callback
			 * and close him if he's not already closing
			 */
			if (n == cgi->pid) {
				lwsl_debug("%s: found PID %d on cgi list\n",
					    __func__, n);

				if (!cgi->content_length && cgi->explicitly_chunked) {
					/*
					 * well, if he sends chunked... give him 5s after the
					 * cgi terminated to send buffered
					 */
					cgi->chunked_grace++;
					continue;
				}

				/* defeat kill() */
				cgi->pid = 0;
				lws_cgi_kill(cgi->wsi);

				break;
			}
			cgi = NULL;
		}
		/* if not found on the cgi list, as he's one of ours, reap */
		if (!cgi) {
			lwsl_debug("%s: reading PID %d although no cgi match\n",
					__func__, n);
			waitpid(n, &status, WNOHANG);
		}
	}

/* disable this to confirm timeout cgi cleanup flow */
#if 1
	pcgi = &pt->cgi_list;

	/* check all the subprocesses on the cgi list */
	while (*pcgi) {
		/* get the next one first as list may change */
		cgi = *pcgi;
		pcgi = &(*pcgi)->cgi_list;

		if (cgi->pid <= 0)
			continue;

		/* we deferred killing him after reaping his PID */
		if (cgi->chunked_grace) {
			cgi->chunked_grace++;
			if (cgi->chunked_grace < 5)
				continue;
			goto finish_him;
		}

		/* finish sending cached headers */
		if (cgi->headers_buf)
			continue;

		/* wait for stdout to be drained */
		if (cgi->content_length > cgi->content_length_seen)
			continue;

		if (cgi->content_length)
			lwsl_debug("%s: wsi %p: expected content length seen: %lld\n",
				__func__, cgi->wsi, (unsigned long long)cgi->content_length_seen);

		/* reap it */
		if (waitpid(cgi->pid, &status, WNOHANG) > 0) {

			if (!cgi->content_length) {
				/*
				 * well, if he sends chunked... give him 5s after the
				 * cgi terminated to send buffered
				 */
				cgi->chunked_grace++;
				continue;
			}
finish_him:
			lwsl_debug("%s: found PID %d on cgi list\n",
				    __func__, cgi->pid);
			/* defeat kill() */
			cgi->pid = 0;
			lws_cgi_kill(cgi->wsi);

			break;
		}
	}
#endif

	/* general anti zombie defence */
//	n = waitpid(-1, &status, WNOHANG);
	//if (n > 0)
	//	lwsl_notice("%s: anti-zombie wait says %d\n", __func__, n);

	return 0;
}
#endif

#ifdef LWS_NO_EXTENSIONS
LWS_EXTERN int
lws_set_extension_option(struct lws *wsi, const char *ext_name,
			 const char *opt_name, const char *opt_val)
{
	return -1;
}
#endif

#ifdef LWS_WITH_ACCESS_LOG
int
lws_access_log(struct lws *wsi)
{
	char *p = wsi->access_log.user_agent, ass[512];
	int l;

	if (!wsi->access_log_pending)
		return 0;

	if (!wsi->access_log.header_log)
		return 0;

	if (!p)
		p = "";

	l = lws_snprintf(ass, sizeof(ass) - 1, "%s %d %lu %s\n",
		     wsi->access_log.header_log,
		     wsi->access_log.response, wsi->access_log.sent, p);

	if (wsi->vhost->log_fd != (int)LWS_INVALID_FILE) {
		if (write(wsi->vhost->log_fd, ass, l) != l)
			lwsl_err("Failed to write log\n");
	} else
		lwsl_err("%s", ass);

	if (wsi->access_log.header_log) {
		lws_free(wsi->access_log.header_log);
		wsi->access_log.header_log = NULL;
	}
	if (wsi->access_log.user_agent) {
		lws_free(wsi->access_log.user_agent);
		wsi->access_log.user_agent = NULL;
	}
	wsi->access_log_pending = 0;

	return 0;
}
#endif

void
lws_sum_stats(const struct lws_context *ctx, struct lws_conn_stats *cs)
{
	const struct lws_vhost *vh = ctx->vhost_list;

	while (vh) {

		cs->rx += vh->conn_stats.rx;
		cs->tx += vh->conn_stats.tx;
		cs->conn += vh->conn_stats.conn;
		cs->trans += vh->conn_stats.trans;
		cs->ws_upg += vh->conn_stats.ws_upg;
		cs->http2_upg += vh->conn_stats.http2_upg;
		cs->rejected += vh->conn_stats.rejected;

		vh = vh->vhost_next;
	}
}

#ifdef LWS_WITH_SERVER_STATUS

LWS_EXTERN int
lws_json_dump_vhost(const struct lws_vhost *vh, char *buf, int len)
{
	static const char * const prots[] = {
		"http://",
		"https://",
		"file://",
		"cgi://",
		">http://",
		">https://",
		"callback://"
	};
	char *orig = buf, *end = buf + len - 1, first = 1;
	int n = 0;

	if (len < 100)
		return 0;

	buf += lws_snprintf(buf, end - buf,
			"{\n \"name\":\"%s\",\n"
			" \"port\":\"%d\",\n"
			" \"use_ssl\":\"%d\",\n"
			" \"sts\":\"%d\",\n"
			" \"rx\":\"%llu\",\n"
			" \"tx\":\"%llu\",\n"
			" \"conn\":\"%lu\",\n"
			" \"trans\":\"%lu\",\n"
			" \"ws_upg\":\"%lu\",\n"
			" \"rejected\":\"%lu\",\n"
			" \"http2_upg\":\"%lu\""
			,
			vh->name, vh->listen_port,
#ifdef LWS_OPENSSL_SUPPORT
			vh->use_ssl,
#else
			0,
#endif
			!!(vh->options & LWS_SERVER_OPTION_STS),
			vh->conn_stats.rx, vh->conn_stats.tx,
			vh->conn_stats.conn, vh->conn_stats.trans,
			vh->conn_stats.ws_upg,
			vh->conn_stats.rejected,
			vh->conn_stats.http2_upg
	);

	if (vh->mount_list) {
		const struct lws_http_mount *m = vh->mount_list;

		buf += lws_snprintf(buf, end - buf, ",\n \"mounts\":[");
		while (m) {
			if (!first)
				buf += lws_snprintf(buf, end - buf, ",");
			buf += lws_snprintf(buf, end - buf,
					"\n  {\n   \"mountpoint\":\"%s\",\n"
					"  \"origin\":\"%s%s\",\n"
					"  \"cache_max_age\":\"%d\",\n"
					"  \"cache_reuse\":\"%d\",\n"
					"  \"cache_revalidate\":\"%d\",\n"
					"  \"cache_intermediaries\":\"%d\"\n"
					,
					m->mountpoint,
					prots[m->origin_protocol],
					m->origin,
					m->cache_max_age,
					m->cache_reusable,
					m->cache_revalidate,
					m->cache_intermediaries);
			if (m->def)
				buf += lws_snprintf(buf, end - buf,
						",\n  \"default\":\"%s\"",
						m->def);
			buf += lws_snprintf(buf, end - buf, "\n  }");
			first = 0;
			m = m->mount_next;
		}
		buf += lws_snprintf(buf, end - buf, "\n ]");
	}

	if (vh->protocols) {
		n = 0;
		first = 1;

		buf += lws_snprintf(buf, end - buf, ",\n \"ws-protocols\":[");
		while (n < vh->count_protocols) {
			if (!first)
				buf += lws_snprintf(buf, end - buf, ",");
			buf += lws_snprintf(buf, end - buf,
					"\n  {\n   \"%s\":{\n"
					"    \"status\":\"ok\"\n   }\n  }"
					,
					vh->protocols[n].name);
			first = 0;
			n++;
		}
		buf += lws_snprintf(buf, end - buf, "\n ]");
	}

	buf += lws_snprintf(buf, end - buf, "\n}");

	return buf - orig;
}


LWS_EXTERN LWS_VISIBLE int
lws_json_dump_context(const struct lws_context *context, char *buf, int len,
		int hide_vhosts)
{
	char *orig = buf, *end = buf + len - 1, first = 1;
	const struct lws_vhost *vh = context->vhost_list;
	const struct lws_context_per_thread *pt;
	time_t t = time(NULL);
	int n, listening = 0, cgi_count = 0;
	struct lws_conn_stats cs;
	double d = 0;
#ifdef LWS_WITH_CGI
	struct lws_cgi * const *pcgi;
#endif

#ifdef LWS_USE_LIBUV
	uv_uptime(&d);
#endif

	buf += lws_snprintf(buf, end - buf, "{ "
			    "\"version\":\"%s\",\n"
			    "\"uptime\":\"%ld\",\n",
			    lws_get_library_version(),
			    (long)d);

#ifdef LWS_HAVE_GETLOADAVG
	{
		double d[3];
		int m;

		m = getloadavg(d, 3);
		for (n = 0; n < m; n++) {
			buf += lws_snprintf(buf, end - buf,
				"\"l%d\":\"%.2f\",\n",
				n + 1, d[n]);
		}
	}
#endif

	buf += lws_snprintf(buf, end - buf, "\"contexts\":[\n");

	buf += lws_snprintf(buf, end - buf, "{ "
				"\"context_uptime\":\"%ld\",\n"
				"\"cgi_spawned\":\"%d\",\n"
				"\"pt_fd_max\":\"%d\",\n"
				"\"ah_pool_max\":\"%d\",\n"
				"\"deprecated\":\"%d\",\n"
				"\"wsi_alive\":\"%d\",\n",
				(unsigned long)(t - context->time_up),
				context->count_cgi_spawned,
				context->fd_limit_per_thread,
				context->max_http_header_pool,
				context->deprecated,
				context->count_wsi_allocated);

	buf += lws_snprintf(buf, end - buf, "\"pt\":[\n ");
	for (n = 0; n < context->count_threads; n++) {
		pt = &context->pt[n];
		if (n)
			buf += lws_snprintf(buf, end - buf, ",");
		buf += lws_snprintf(buf, end - buf,
				"\n  {\n"
				"    \"fds_count\":\"%d\",\n"
				"    \"ah_pool_inuse\":\"%d\",\n"
				"    \"ah_wait_list\":\"%d\"\n"
				"    }",
				pt->fds_count,
				pt->ah_count_in_use,
				pt->ah_wait_list_length);
	}

	buf += lws_snprintf(buf, end - buf, "]");

	buf += lws_snprintf(buf, end - buf, ", \"vhosts\":[\n ");

	first = 1;
	vh = context->vhost_list;
	listening = 0;
	cs = context->conn_stats;
	lws_sum_stats(context, &cs);
	while (vh) {

		if (!hide_vhosts) {
			if (!first)
				if(buf != end)
					*buf++ = ',';
			buf += lws_json_dump_vhost(vh, buf, end - buf);
			first = 0;
		}
		if (vh->lserv_wsi)
			listening++;
		vh = vh->vhost_next;
	}

	buf += lws_snprintf(buf, end - buf,
			"],\n\"listen_wsi\":\"%d\",\n"
			" \"rx\":\"%llu\",\n"
			" \"tx\":\"%llu\",\n"
			" \"conn\":\"%lu\",\n"
			" \"trans\":\"%lu\",\n"
			" \"ws_upg\":\"%lu\",\n"
			" \"rejected\":\"%lu\",\n"
			" \"http2_upg\":\"%lu\"",
			listening,
			cs.rx, cs.tx, cs.conn, cs.trans,
			cs.ws_upg, cs.rejected, cs.http2_upg);

#ifdef LWS_WITH_CGI
	for (n = 0; n < context->count_threads; n++) {
		pt = &context->pt[n];
		pcgi = &pt->cgi_list;

		while (*pcgi) {
			pcgi = &(*pcgi)->cgi_list;

			cgi_count++;
		}
	}
#endif
	buf += lws_snprintf(buf, end - buf, ",\n \"cgi_alive\":\"%d\"\n ",
			cgi_count);

	buf += lws_snprintf(buf, end - buf, "}");


	buf += lws_snprintf(buf, end - buf, "]}\n ");

	return buf - orig;
}

#endif

#if defined(LWS_WITH_STATS)

LWS_VISIBLE LWS_EXTERN uint64_t
lws_stats_get(struct lws_context *context, int index)
{
	if (index >= LWSSTATS_SIZE)
		return 0;

	return context->lws_stats[index];
}

LWS_VISIBLE LWS_EXTERN void
lws_stats_log_dump(struct lws_context *context)
{
	struct lws_vhost *v = context->vhost_list;
	int n;

	if (!context->updated)
		return;

	context->updated = 0;

	lwsl_notice("\n");
	lwsl_notice("LWS internal statistics dump ----->\n");
	lwsl_notice("LWSSTATS_C_CONNECTIONS:                     %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_CONNECTIONS));
	lwsl_notice("LWSSTATS_C_API_CLOSE:                       %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_API_CLOSE));
	lwsl_notice("LWSSTATS_C_API_READ:                        %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_API_READ));
	lwsl_notice("LWSSTATS_C_API_LWS_WRITE:                   %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_API_LWS_WRITE));
	lwsl_notice("LWSSTATS_C_API_WRITE:                       %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_API_WRITE));
	lwsl_notice("LWSSTATS_C_WRITE_PARTIALS:                  %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_WRITE_PARTIALS));
	lwsl_notice("LWSSTATS_C_WRITEABLE_CB_REQ:                %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_WRITEABLE_CB_REQ));
	lwsl_notice("LWSSTATS_C_WRITEABLE_CB_EFF_REQ:            %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_WRITEABLE_CB_EFF_REQ));
	lwsl_notice("LWSSTATS_C_WRITEABLE_CB:                    %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_WRITEABLE_CB));
	lwsl_notice("LWSSTATS_C_SSL_CONNECTIONS_FAILED:          %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_SSL_CONNECTIONS_FAILED));
	lwsl_notice("LWSSTATS_C_SSL_CONNECTIONS_ACCEPTED:        %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_SSL_CONNECTIONS_ACCEPTED));
	lwsl_notice("LWSSTATS_C_SSL_CONNS_HAD_RX:                %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_SSL_CONNS_HAD_RX));

	lwsl_notice("LWSSTATS_C_TIMEOUTS:                        %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_TIMEOUTS));
	lwsl_notice("LWSSTATS_C_SERVICE_ENTRY:                   %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_C_SERVICE_ENTRY));
	lwsl_notice("LWSSTATS_B_READ:                            %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_B_READ));
	lwsl_notice("LWSSTATS_B_WRITE:                           %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_B_WRITE));
	lwsl_notice("LWSSTATS_B_PARTIALS_ACCEPTED_PARTS:         %8llu\n", (unsigned long long)lws_stats_get(context, LWSSTATS_B_PARTIALS_ACCEPTED_PARTS));
	lwsl_notice("LWSSTATS_MS_SSL_CONNECTIONS_ACCEPTED_DELAY: %8llums\n", (unsigned long long)lws_stats_get(context, LWSSTATS_MS_SSL_CONNECTIONS_ACCEPTED_DELAY) / 1000);
	if (lws_stats_get(context, LWSSTATS_C_SSL_CONNECTIONS_ACCEPTED))
		lwsl_notice("  Avg accept delay:                         %8llums\n",
			(unsigned long long)(lws_stats_get(context, LWSSTATS_MS_SSL_CONNECTIONS_ACCEPTED_DELAY) /
			lws_stats_get(context, LWSSTATS_C_SSL_CONNECTIONS_ACCEPTED)) / 1000);
	lwsl_notice("LWSSTATS_MS_SSL_RX_DELAY:                   %8llums\n", (unsigned long long)lws_stats_get(context, LWSSTATS_MS_SSL_RX_DELAY) / 1000);
	if (lws_stats_get(context, LWSSTATS_C_SSL_CONNS_HAD_RX))
		lwsl_notice("  Avg accept-rx delay:                      %8llums\n",
			(unsigned long long)(lws_stats_get(context, LWSSTATS_MS_SSL_RX_DELAY) /
			lws_stats_get(context, LWSSTATS_C_SSL_CONNS_HAD_RX)) / 1000);

	lwsl_notice("LWSSTATS_MS_WRITABLE_DELAY:                 %8lluus\n",
			(unsigned long long)lws_stats_get(context, LWSSTATS_MS_WRITABLE_DELAY));
	lwsl_notice("LWSSTATS_MS_WORST_WRITABLE_DELAY:           %8lluus\n",
				(unsigned long long)lws_stats_get(context, LWSSTATS_MS_WORST_WRITABLE_DELAY));
	if (lws_stats_get(context, LWSSTATS_C_WRITEABLE_CB))
		lwsl_notice("  Avg writable delay:                       %8lluus\n",
			(unsigned long long)(lws_stats_get(context, LWSSTATS_MS_WRITABLE_DELAY) /
			lws_stats_get(context, LWSSTATS_C_WRITEABLE_CB)));
	lwsl_notice("Simultaneous SSL restriction:               %8d/%d/%d\n", context->simultaneous_ssl,
		context->simultaneous_ssl_restriction, context->ssl_gate_accepts);

	lwsl_notice("Live wsi:                                   %8d\n", context->count_wsi_allocated);

#if defined(LWS_WITH_STATS)
	context->updated = 1;
#endif

	while (v) {
		if (v->lserv_wsi) {

			struct lws_context_per_thread *pt = &context->pt[(int)v->lserv_wsi->tsi];
			struct lws_pollfd *pfd;

			pfd = &pt->fds[v->lserv_wsi->position_in_fds_table];

			lwsl_notice("  Listen port %d actual POLLIN:   %d\n",
					v->listen_port, (int)pfd->events & LWS_POLLIN);
		}

		v = v->vhost_next;
	}

	for (n = 0; n < context->count_threads; n++) {
		struct lws_context_per_thread *pt = &context->pt[n];
		struct lws *wl;
		int m = 0;

		lwsl_notice("PT %d\n", n + 1);

		lws_pt_lock(pt);

		lwsl_notice("  AH in use / max:                  %d / %d\n",
				pt->ah_count_in_use,
				context->max_http_header_pool);

		wl = pt->ah_wait_list;
		while (wl) {
			m++;
			wl = wl->u.hdr.ah_wait_list;
		}

		lwsl_notice("  AH wait list count / actual:      %d / %d\n",
				pt->ah_wait_list_length, m);

		lws_pt_unlock(pt);
	}

	lwsl_notice("\n");
}

void
lws_stats_atomic_bump(struct lws_context * context,
		struct lws_context_per_thread *pt, int index, uint64_t bump)
{
	lws_pt_lock(pt);
	context->lws_stats[index] += bump;
	if (index != LWSSTATS_C_SERVICE_ENTRY)
		context->updated = 1;
	lws_pt_unlock(pt);
}

void
lws_stats_atomic_max(struct lws_context * context,
		struct lws_context_per_thread *pt, int index, uint64_t val)
{
	lws_pt_lock(pt);
	if (val > context->lws_stats[index]) {
		context->lws_stats[index] = val;
		context->updated = 1;
	}
	lws_pt_unlock(pt);
}

#endif
