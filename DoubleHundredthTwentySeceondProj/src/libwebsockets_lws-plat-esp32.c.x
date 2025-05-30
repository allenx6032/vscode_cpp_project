/*
 * libwebsockets - small server side websockets and web server implementation
 *
 * Copyright (C) 2010-2017 Andy Green <andy@warmcat.com>
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
#include "freertos/timers.h"
#include <esp_attr.h>
#include <esp_system.h>

/*
 * included from libwebsockets.c for unix builds
 */

unsigned long long time_in_microseconds(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long)tv.tv_sec * 1000000LL) + tv.tv_usec;
}

LWS_VISIBLE int
lws_get_random(struct lws_context *context, void *buf, int len)
{
	uint8_t *pb = buf;

	while (len) {
		uint32_t r = esp_random();
		uint8_t *p = (uint8_t *)&r;
		int b = 4;

		if (len < b)
			b = len;

		len -= b;

		while (b--)
			*pb++ = p[b];
	}

	return pb - (uint8_t *)buf;
}

LWS_VISIBLE int
lws_send_pipe_choked(struct lws *wsi)
{
	fd_set writefds;
	struct timeval tv = { 0, 0 };

	/* treat the fact we got a truncated send pending as if we're choked */
	if (wsi->trunc_len)
		return 1;

	FD_ZERO(&writefds);
	FD_SET(wsi->desc.sockfd, &writefds);

	if (select(wsi->desc.sockfd + 1, NULL, &writefds, NULL, &tv) < 1)
		return 1;

	return 0;
}

LWS_VISIBLE int
lws_poll_listen_fd(struct lws_pollfd *fd)
{
	fd_set readfds;
	struct timeval tv = { 0, 0 };

	FD_ZERO(&readfds);
	FD_SET(fd->fd, &readfds);

	return select(fd->fd + 1, &readfds, NULL, NULL, &tv);
}

LWS_VISIBLE void
lws_cancel_service_pt(struct lws *wsi)
{
}

LWS_VISIBLE void
lws_cancel_service(struct lws_context *context)
{
}

LWS_VISIBLE void lwsl_emit_syslog(int level, const char *line)
{
	printf("%d: %s", level, line);
}

LWS_VISIBLE LWS_EXTERN int
_lws_plat_service_tsi(struct lws_context *context, int timeout_ms, int tsi)
{
	struct lws_context_per_thread *pt;
	int n = -1, m, c;

	/* stay dead once we are dead */

	if (!context || !context->vhost_list)
		return 1;

	pt = &context->pt[tsi];
	lws_stats_atomic_bump(context, pt, LWSSTATS_C_SERVICE_ENTRY, 1);

	if (timeout_ms < 0)
		goto faked_service;

	if (!context->service_tid_detected) {
		struct lws _lws;

		memset(&_lws, 0, sizeof(_lws));
		_lws.context = context;

		context->service_tid_detected =
			context->vhost_list->protocols[0].callback(
			&_lws, LWS_CALLBACK_GET_THREAD_ID, NULL, NULL, 0);
	}
	context->service_tid = context->service_tid_detected;

	/*
	 * is there anybody with pending stuff that needs service forcing?
	 */
	if (!lws_service_adjust_timeout(context, 1, tsi)) {
		/* -1 timeout means just do forced service */
		_lws_plat_service_tsi(context, -1, pt->tid);
		/* still somebody left who wants forced service? */
		if (!lws_service_adjust_timeout(context, 1, pt->tid))
			/* yes... come back again quickly */
			timeout_ms = 0;
	}

//	n = poll(pt->fds, pt->fds_count, timeout_ms);
	{
		fd_set readfds, writefds, errfds;
		struct timeval tv = { timeout_ms / 1000,
				      (timeout_ms % 1000) * 1000 }, *ptv = &tv;
		int max_fd = 0;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&errfds);

		for (n = 0; n < pt->fds_count; n++) {
			pt->fds[n].revents = 0;
			if (pt->fds[n].fd >= max_fd)
				max_fd = pt->fds[n].fd;
			if (pt->fds[n].events & LWS_POLLIN)
				FD_SET(pt->fds[n].fd, &readfds);
			if (pt->fds[n].events & LWS_POLLOUT)
				FD_SET(pt->fds[n].fd, &writefds);
			FD_SET(pt->fds[n].fd, &errfds);
		}

		n = select(max_fd + 1, &readfds, &writefds, &errfds, ptv);
		for (n = 0; n < pt->fds_count; n++) {
			if (FD_ISSET(pt->fds[n].fd, &readfds))
				pt->fds[n].revents |= LWS_POLLIN;
			if (FD_ISSET(pt->fds[n].fd, &writefds))
				pt->fds[n].revents |= LWS_POLLOUT;
			if (FD_ISSET(pt->fds[n].fd, &errfds))
				pt->fds[n].revents |= LWS_POLLHUP;
		}
	}


#ifdef LWS_OPENSSL_SUPPORT
	if (!pt->rx_draining_ext_list &&
	    !lws_ssl_anybody_has_buffered_read_tsi(context, tsi) && !n) {
#else
	if (!pt->rx_draining_ext_list && !n) /* poll timeout */ {
#endif
		lws_service_fd_tsi(context, NULL, tsi);
		return 0;
	}

faked_service:
	m = lws_service_flag_pending(context, tsi);
	if (m)
		c = -1; /* unknown limit */
	else
		if (n < 0) {
			if (LWS_ERRNO != LWS_EINTR)
				return -1;
			return 0;
		} else
			c = n;

	/* any socket with events to service? */
	for (n = 0; n < pt->fds_count && c; n++) {
		if (!pt->fds[n].revents)
			continue;

		c--;

		m = lws_service_fd_tsi(context, &pt->fds[n], tsi);
		if (m < 0)
			return -1;
		/* if something closed, retry this slot */
		if (m)
			n--;
	}

	return 0;
}

LWS_VISIBLE int
lws_plat_check_connection_error(struct lws *wsi)
{
	return 0;
}

LWS_VISIBLE int
lws_plat_service(struct lws_context *context, int timeout_ms)
{
	return _lws_plat_service_tsi(context, timeout_ms, 0);
}

LWS_VISIBLE int
lws_plat_set_socket_options(struct lws_vhost *vhost, int fd)
{
	int optval = 1;
	socklen_t optlen = sizeof(optval);

#if defined(__APPLE__) || \
    defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || \
    defined(__NetBSD__) || \
    defined(__OpenBSD__)
	struct protoent *tcp_proto;
#endif

	if (vhost->ka_time) {
		/* enable keepalive on this socket */
		optval = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
			       (const void *)&optval, optlen) < 0)
			return 1;

#if defined(__APPLE__) || \
    defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || \
    defined(__NetBSD__) || \
        defined(__CYGWIN__) || defined(__OpenBSD__) || defined (__sun)

		/*
		 * didn't find a way to set these per-socket, need to
		 * tune kernel systemwide values
		 */
#else
		/* set the keepalive conditions we want on it too */
		optval = vhost->ka_time;
		if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,
			       (const void *)&optval, optlen) < 0)
			return 1;

		optval = vhost->ka_interval;
		if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL,
			       (const void *)&optval, optlen) < 0)
			return 1;

		optval = vhost->ka_probes;
		if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,
			       (const void *)&optval, optlen) < 0)
			return 1;
#endif
	}

	/* Disable Nagle */
	optval = 1;
//	if (setsockopt(fd, SOL_TCP, TCP_NODELAY, (const void *)&optval, optlen) < 0)
//		return 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, optlen) < 0)
		return 1;

	/* We are nonblocking... */
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		return 1;

	return 0;
}

LWS_VISIBLE void
lws_plat_drop_app_privileges(struct lws_context_creation_info *info)
{
}

LWS_VISIBLE int
lws_plat_context_early_init(void)
{
	//signal(SIGPIPE, SIG_IGN);

//	signal(SIGABRT, sigabrt_handler);

	return 0;
}

LWS_VISIBLE void
lws_plat_context_early_destroy(struct lws_context *context)
{
}

LWS_VISIBLE void
lws_plat_context_late_destroy(struct lws_context *context)
{
#ifdef LWS_WITH_PLUGINS
	if (context->plugin_list)
		lws_plat_plugins_destroy(context);
#endif

	if (context->lws_lookup)
		lws_free(context->lws_lookup);
}

/* cast a struct sockaddr_in6 * into addr for ipv6 */

LWS_VISIBLE int
lws_interface_to_sa(int ipv6, const char *ifname, struct sockaddr_in *addr,
		    size_t addrlen)
{
#if 0
	int rc = -1;

	struct ifaddrs *ifr;
	struct ifaddrs *ifc;
#ifdef LWS_USE_IPV6
	struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
#endif

	getifaddrs(&ifr);
	for (ifc = ifr; ifc != NULL && rc; ifc = ifc->ifa_next) {
		if (!ifc->ifa_addr)
			continue;

		lwsl_info(" interface %s vs %s\n", ifc->ifa_name, ifname);

		if (strcmp(ifc->ifa_name, ifname))
			continue;

		switch (ifc->ifa_addr->sa_family) {
		case AF_INET:
#ifdef LWS_USE_IPV6
			if (ipv6) {
				/* map IPv4 to IPv6 */
				bzero((char *)&addr6->sin6_addr,
						sizeof(struct in6_addr));
				addr6->sin6_addr.s6_addr[10] = 0xff;
				addr6->sin6_addr.s6_addr[11] = 0xff;
				memcpy(&addr6->sin6_addr.s6_addr[12],
					&((struct sockaddr_in *)ifc->ifa_addr)->sin_addr,
							sizeof(struct in_addr));
			} else
#endif
				memcpy(addr,
					(struct sockaddr_in *)ifc->ifa_addr,
						    sizeof(struct sockaddr_in));
			break;
#ifdef LWS_USE_IPV6
		case AF_INET6:
			memcpy(&addr6->sin6_addr,
			  &((struct sockaddr_in6 *)ifc->ifa_addr)->sin6_addr,
						       sizeof(struct in6_addr));
			break;
#endif
		default:
			continue;
		}
		rc = 0;
	}

	freeifaddrs(ifr);

	if (rc == -1) {
		/* check if bind to IP address */
#ifdef LWS_USE_IPV6
		if (inet_pton(AF_INET6, ifname, &addr6->sin6_addr) == 1)
			rc = 0;
		else
#endif
		if (inet_pton(AF_INET, ifname, &addr->sin_addr) == 1)
			rc = 0;
	}

	return rc;
#endif

	return -1;
}

LWS_VISIBLE void
lws_plat_insert_socket_into_fds(struct lws_context *context, struct lws *wsi)
{
	struct lws_context_per_thread *pt = &context->pt[(int)wsi->tsi];

	pt->fds[pt->fds_count++].revents = 0;
}

LWS_VISIBLE void
lws_plat_delete_socket_from_fds(struct lws_context *context,
						struct lws *wsi, int m)
{
	struct lws_context_per_thread *pt = &context->pt[(int)wsi->tsi];

	pt->fds_count--;
}

LWS_VISIBLE void
lws_plat_service_periodic(struct lws_context *context)
{
}

LWS_VISIBLE int
lws_plat_change_pollfd(struct lws_context *context,
		      struct lws *wsi, struct lws_pollfd *pfd)
{
	return 0;
}

LWS_VISIBLE const char *
lws_plat_inet_ntop(int af, const void *src, char *dst, int cnt)
{
	return inet_ntop(af, src, dst, cnt);
}

LWS_VISIBLE int
lws_plat_inet_pton(int af, const char *src, void *dst)
{
	return 1; //  inet_pton(af, src, dst);
}

LWS_VISIBLE lws_fop_fd_t IRAM_ATTR
_lws_plat_file_open(const struct lws_plat_file_ops *fops, const char *filename,
		    const char *vpath, lws_fop_flags_t *flags)
{
	struct stat stat_buf;
	lws_fop_fd_t fop_fd;
	int ret = open(filename, *flags, 0664);

	if (ret < 0)
		return NULL;

	if (fstat(ret, &stat_buf) < 0)
		goto bail;

	fop_fd = malloc(sizeof(*fop_fd));
	if (!fop_fd)
		goto bail;

	fop_fd->fops = fops;
	fop_fd->fd = ret;
	fop_fd->flags = *flags;
	fop_fd->filesystem_priv = NULL; /* we don't use it */
	fop_fd->pos = 0;
	fop_fd->len = stat_buf.st_size;

	return fop_fd;

bail:
	close(ret);

	return NULL;
}

LWS_VISIBLE int IRAM_ATTR
_lws_plat_file_close(lws_fop_fd_t *fops_fd)
{
	int fd = (*fops_fd)->fd;

	free(*fops_fd);
	*fops_fd = NULL;

	return close(fd);
}

LWS_VISIBLE lws_fileofs_t IRAM_ATTR
_lws_plat_file_seek_cur(lws_fop_fd_t fops_fd, lws_fileofs_t offset)
{
	return lseek(fops_fd->fd, offset, SEEK_CUR);
}

LWS_VISIBLE int IRAM_ATTR
_lws_plat_file_read(lws_fop_fd_t fops_fd, lws_filepos_t *amount,
		    uint8_t *buf, lws_filepos_t len)
{
	long n;

	n = read(fops_fd->fd, buf, len);
	if (n == -1) {
		*amount = 0;
		return -1;
	}
	fops_fd->pos += n;
	*amount = n;

	return 0;
}

LWS_VISIBLE int IRAM_ATTR
_lws_plat_file_write(lws_fop_fd_t fops_fd, lws_filepos_t *amount,
		     uint8_t *buf, lws_filepos_t len)
{
	long n;

	n = write(fops_fd->fd, buf, len);
	if (n == -1) {
		*amount = 0;
		return -1;
	}
	fops_fd->pos += n;
	*amount = n;

	return 0;
}


LWS_VISIBLE int
lws_plat_init(struct lws_context *context,
	      struct lws_context_creation_info *info)
{
	/* master context has the global fd lookup array */
	context->lws_lookup = lws_zalloc(sizeof(struct lws *) *
					 context->max_fds);
	if (context->lws_lookup == NULL) {
		lwsl_err("OOM on lws_lookup array for %d connections\n",
			 context->max_fds);
		return 1;
	}

	lwsl_notice(" mem: platform fd map: %5lu bytes\n",
		    (unsigned long)(sizeof(struct lws *) * context->max_fds));

#ifdef LWS_WITH_PLUGINS
	if (info->plugin_dirs)
		lws_plat_plugins_init(context, info->plugin_dirs);
#endif

	return 0;
}


LWS_VISIBLE void esp32_uvtimer_cb(TimerHandle_t t)
{
	struct timer_mapping *p = pvTimerGetTimerID(t);

	p->cb(p->t);
}

void ERR_error_string_n(unsigned long e, char *buf, size_t len)
{
	strncpy(buf, "unknown", len);
}

void ERR_free_strings(void)
{
}

char *ERR_error_string(unsigned long e, char *buf)
{
	if (buf)
		strcpy(buf, "unknown");

	return "unknown";
}


/* helper functionality */

#include "romfs.h"
#include <esp_ota_ops.h>
#include <tcpip_adapter.h>
#include <esp_image_format.h>
#include <esp_task_wdt.h>
#include "soc/ledc_reg.h"
#include "driver/ledc.h"

struct lws_esp32 lws_esp32 = {
	.model = CONFIG_LWS_MODEL_NAME,
	.serial = "unknown",
	.region = WIFI_COUNTRY_US, // default to safest option
};

/*
 * Group AP / Station State
 */

enum lws_gapss {
	LWS_GAPSS_INITIAL,	/* just started up, init and move to LWS_GAPSS_SCAN */
	LWS_GAPSS_SCAN,		/*
				 * Unconnected, scanning: AP known in one of the config
				 * slots -> configure it, start timeout + LWS_GAPSS_STAT,
				 * if no AP already up in same group with lower MAC,
				 * after a random period start up our AP (LWS_GAPSS_AP)
				 */
	LWS_GAPSS_AP,		/*
				 * Trying to be the group AP... periodically do a scan
				 * LWS_GAPSS_AP_SCAN, faster and then slower
       				 */
	LWS_GAPSS_AP_SCAN,	/*
				 * doing a scan while trying to be the group AP... if
				 * we see a lower MAC being the AP for the same group
				 * AP, abandon being an AP and join that AP as a
				 * station
				 */
	LWS_GAPSS_STAT_GRP_AP,	/*
				 * We have decided to join another group member who is
				 * being the AP, as its MAC is lower than ours.  This
				 * is a stable state, but we still do periodic scans
				 * (LWS_GAPSS_STAT_GRP_AP_SCAN) and will always prefer
				 * an AP configured in a slot.
				 */
	LWS_GAPSS_STAT_GRP_AP_SCAN,
				/*
				 * We have joined a group member who is doing the AP
				 * job... we want to check every now and then if a
				 * configured AP has appeared that we should better
				 * use instead.  Otherwise stay in LWS_GAPSS_STAT_GRP_AP
				 */
	LWS_GAPSS_STAT,		/*
				 * trying to connect to another non-group AP.  If we
				 * don't get an IP within a timeout and retries,
				 * blacklist it and go back 
				 */
	LWS_GAPSS_STAT_HAPPY,
};

static const char *gapss_str[] = {
	"LWS_GAPSS_INITIAL",
        "LWS_GAPSS_SCAN",
        "LWS_GAPSS_AP",
        "LWS_GAPSS_AP_SCAN",
        "LWS_GAPSS_STAT_GRP_AP",
        "LWS_GAPSS_STAT_GRP_AP_SCAN",
        "LWS_GAPSS_STAT",
	"LWS_GAPSS_STAT_HAPPY",
};

static romfs_t lws_esp32_romfs;
static TimerHandle_t leds_timer, scan_timer, debounce_timer
#if !defined(CONFIG_LWS_IS_FACTORY_APPLICATION)
, mdns_timer
#endif
;
static enum lws_gapss gapss = LWS_GAPSS_INITIAL;
static char bdown;

#define GPIO_SW 14

struct esp32_file {
	const struct inode *i;
};

static void lws_gapss_to(enum lws_gapss to)
{
	lwsl_notice("gapss from %s to %s\n", gapss_str[gapss], gapss_str[to]);
	gapss = to;
}

uint32_t lws_esp32_get_reboot_type(void)
{
	uint32_t *p = (uint32_t *)LWS_MAGIC_REBOOT_TYPE_ADS, val = *p;
	nvs_handle nvh;
	size_t s = 0;
	int n = 0;

	ESP_ERROR_CHECK(nvs_open("lws-station", NVS_READWRITE, &nvh));
	if (nvs_get_blob(nvh, "ssl-pub.pem", NULL, &s) == ESP_OK)
		n = 1;
	if (nvs_get_blob(nvh, "ssl-pri.pem", NULL, &s) == ESP_OK)
		n |= 2;
	nvs_close(nvh);

	/*
	 * in the case the SSL certs are not there, don't require
	 * the button to be down to access all features.
	 */
	if (n != 3)
		val = LWS_MAGIC_REBOOT_TYPE_FORCED_FACTORY_BUTTON;

	return val;
}

static void render_ip(char *dest, int len, uint8_t *ip)
{
	snprintf(dest, len, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

void lws_esp32_restart_guided(uint32_t type)
{
        uint32_t *p_force_factory_magic = (uint32_t *)LWS_MAGIC_REBOOT_TYPE_ADS;

	lwsl_notice("%s: %x\n", __func__, type);
        *p_force_factory_magic = type;

	esp_restart();
}

/*
 * esp-idf goes crazy with zero length str nvs.  Use this as a workaround
 * to delete the key in that case.
 */

esp_err_t lws_nvs_set_str(nvs_handle handle, const char* key, const char* value)
{
	if (*value)
		return nvs_set_str(handle, key, value);

	return nvs_erase_key(handle, key);
}

static wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true
};

static char scan_ongoing = 0, scan_timer_exists = 0;
static int try_slot = -1;

static wifi_config_t config = {
	.ap = {
	    .channel = 6,
	    .authmode = WIFI_AUTH_OPEN,
	    .max_connection = 1,
	} }, sta_config = {
	.sta = {
		.bssid_set = 0,
	} };

static void lws_esp32_scan_timer_cb(TimerHandle_t th)
{
	int n;

	lwsl_notice("%s\n", __func__);
	scan_ongoing = 0;
	n = esp_wifi_scan_start(&scan_config, false);
	if (n != ESP_OK)
		lwsl_err("scan start failed %d\n", n);
}

#if !defined(CONFIG_LWS_IS_FACTORY_APPLICATION)

void __attribute__(( weak ))
lws_group_member_event(int e, void *p)
{
}

void __attribute__(( weak ))
lws_get_iframe_size(int *w, int *h)
{
	*w = 320;
	*h = 160;
}

void lws_group_member_event_call(int e, void *p)
{
	lws_group_member_event(e, p);
}

static int
get_txt_param(const char *txt, const char *param, char *result, int len)
{
	const char *p;

again:
	p = strstr(txt, param);
	if (!p) {
		*result = '\0';
		return 1;
	}

	p += strlen(param);
	if (*p != '=') {
		txt = p;
		goto again;
	}
	p++;
	while (*p && *p != '&' && --len)
		*result++ = *p++;

	*result = '\0';

	return 0;
}

static void lws_esp32_mdns_timer_cb(TimerHandle_t th)
{
	uint64_t now = time_in_microseconds(); 
	struct lws_group_member *p, **p1;
	const mdns_result_t *r;
	int n, m;

	if (!lws_esp32.mdns)
		return;
	n = mdns_query_end(lws_esp32.mdns);

	for (m = 0; m < n; m++) {
		char ch = 0, group[16];

		r = mdns_result_get(lws_esp32.mdns, m);

		get_txt_param(r->txt, "group", group, sizeof(group));
		if (strcmp(group, lws_esp32.group)) /* not our group */ {
			lwsl_notice("group %s vs %s  %s\n",
					group, lws_esp32.group, r->txt);
			continue;
		}

		p = lws_esp32.first;
		while (p) {
			if (strcmp(r->host, p->host))
				goto next;
			if (memcmp(&r->addr, &p->addr, sizeof(r->addr)))
				goto next;

			p->last_seen = now;
			break;
next:
			p = p->next;
		}
		if (!p) { /* did not find */
			char temp[8];
			p = malloc(sizeof(*p));
			if (!p)
				continue;
			strncpy(p->host, r->host, sizeof(p->host) - 1);
			p->host[sizeof(p->host) - 1] = '\0';

			get_txt_param(r->txt, "model", p->model, sizeof(p->model));
			get_txt_param(r->txt, "role", p->role, sizeof(p->role));
			get_txt_param(r->txt, "mac", p->mac, sizeof(p->mac));
			get_txt_param(r->txt, "width", temp, sizeof(temp));
			p->width = atoi(temp);
			get_txt_param(r->txt, "height", temp, sizeof(temp));
			p->height = atoi(temp);

			memcpy(&p->addr, &r->addr, sizeof(p->addr));
			memcpy(&p->addrv6, &r->addrv6, sizeof(p->addrv6));
			p->last_seen = now;
			p->flags = 0;
			p->next = lws_esp32.first;
			lws_esp32.first = p;
			lws_esp32.extant_group_members++;

			lws_group_member_event_call(LWS_SYSTEM_GROUP_MEMBER_ADD, p);
		} else {
			if (memcmp(&p->addr, &r->addr, sizeof(p->addr))) {
				memcpy(&p->addr, &r->addr, sizeof(p->addr));
				ch = 1;
			}
			if (memcmp(&p->addrv6, &r->addrv6, sizeof(p->addrv6))) {
				memcpy(&p->addrv6, &r->addrv6, sizeof(p->addrv6));
				ch = 1;
			}
			if (ch)
				lws_group_member_event_call(LWS_SYSTEM_GROUP_MEMBER_CHANGE, p);
		}
	}

	mdns_result_free(lws_esp32.mdns);

	/* garbage-collect group members not seen for too long */
	p1 = &lws_esp32.first;
	while (*p1) {
		p = *p1;
		if (!(p->flags & LWS_GROUP_FLAG_SELF) &&
				now - p->last_seen > 60000000) {
			lws_esp32.extant_group_members--;
			*p1 = p->next;

			lws_group_member_event_call(LWS_SYSTEM_GROUP_MEMBER_REMOVE, p);
			free(p);
			continue;
		}
		p1 = &(*p1)->next;
	}

	mdns_query(lws_esp32.mdns, "_lwsgrmem", "_tcp", 0);
	xTimerStart(mdns_timer, 0);
}
#endif

void __attribute__(( weak ))
lws_esp32_button(int down)
{
}

void IRAM_ATTR
gpio_irq(void *arg)
{
	bdown ^= 1;
	gpio_set_intr_type(GPIO_SW, GPIO_INTR_DISABLE);
	xTimerStart(debounce_timer, 0);

	lws_esp32_button(bdown);
}

static void lws_esp32_debounce_timer_cb(TimerHandle_t th)
{
	if (bdown)
		gpio_set_intr_type(GPIO_SW, GPIO_INTR_POSEDGE);
	else
		gpio_set_intr_type(GPIO_SW, GPIO_INTR_NEGEDGE);
}


static int
start_scan()
{
	/* if no APs configured, no point... */

	if (!lws_esp32.ssid[0][0] &&
	    !lws_esp32.ssid[1][0] &&
	    !lws_esp32.ssid[2][0] &&
	    !lws_esp32.ssid[3][0])
		return 0;

	if (scan_timer_exists && !scan_ongoing) {
		// lwsl_notice("Starting scan timer...\n");
		scan_ongoing = 1;
		xTimerStart(scan_timer, 0);
	}

	return 0;
}



static void
end_scan()
{
	wifi_ap_record_t ap_records[10];
	uint16_t count_ap_records;
	int n, m;

	count_ap_records = ARRAY_SIZE(ap_records);
	if (esp_wifi_scan_get_ap_records(&count_ap_records, ap_records) != ESP_OK) {
		lwsl_err("%s: failed\n", __func__);
		return;
	}

	if (!count_ap_records)
		goto passthru;

	if (gapss != LWS_GAPSS_SCAN) {
		lwsl_notice("ignoring scan as gapss %s\n", gapss_str[gapss]);
		goto passthru;
	}

	/* no point if no APs set up */
	if (!lws_esp32.ssid[0][0] &&
	    !lws_esp32.ssid[1][0] &&
	    !lws_esp32.ssid[2][0] &&
	    !lws_esp32.ssid[3][0])
		goto passthru;

	lwsl_notice("checking %d scan records\n", count_ap_records);

	for (n = 0; n < 4; n++) {

		if (!lws_esp32.ssid[(n + try_slot + 1) & 3][0])
			continue;

		lwsl_notice("looking for %s\n", lws_esp32.ssid[(n + try_slot + 1) & 3]);

		/* this ssid appears in scan results? */

		for (m = 0; m < count_ap_records; m++) {
			// lwsl_notice("  %s\n", ap_records[m].ssid);
			if (strcmp((char *)ap_records[m].ssid, lws_esp32.ssid[(n + try_slot + 1) & 3]) == 0)
				goto hit;
		}

		continue;

hit:
		m = (n + try_slot + 1) & 3;
		try_slot = m;
		lwsl_notice("Attempting connection with slot %d: %s:\n", m,
				lws_esp32.ssid[m]);
		/* set the ssid we last tried to connect to */
		strncpy(lws_esp32.active_ssid, lws_esp32.ssid[m],
				sizeof(lws_esp32.active_ssid) - 1);
		lws_esp32.active_ssid[sizeof(lws_esp32.active_ssid) - 1] = '\0';

		strncpy((char *)sta_config.sta.ssid, lws_esp32.ssid[m], sizeof(sta_config.sta.ssid) - 1);
		strncpy((char *)sta_config.sta.password, lws_esp32.password[m], sizeof(sta_config.sta.password) - 1);

		tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, (const char *)&config.ap.ssid[7]);
		lws_gapss_to(LWS_GAPSS_STAT);

		esp_wifi_set_config(WIFI_IF_STA, &sta_config);
		esp_wifi_connect();
		break;
	}

	if (n == 4)
		start_scan();

passthru:
	if (lws_esp32.scan_consumer)
		lws_esp32.scan_consumer(count_ap_records, ap_records, lws_esp32.scan_consumer_arg);

}

static void
lws_set_genled(int n)
{
	lws_esp32.genled_t = time_in_microseconds();
	lws_esp32.genled = n;
}

int
lws_esp32_leds_network_indication(void)
{
	uint64_t us, r;
	int n, fadein = 100, speed = 1199, div = 1, base = 0;

	r = time_in_microseconds();
	us = r - lws_esp32.genled_t;

	switch (lws_esp32.genled) {
	case LWSESP32_GENLED__INIT:
		lws_esp32.genled = LWSESP32_GENLED__LOST_NETWORK;
		/* fallthru */
	case LWSESP32_GENLED__LOST_NETWORK:
		fadein = us / 10000; /* 100 steps in 1s */
		if (fadein > 100) {
			fadein = 100;
			lws_esp32.genled = LWSESP32_GENLED__NO_NETWORK;
		}
		/* fallthru */
	case LWSESP32_GENLED__NO_NETWORK:
		break;
	case LWSESP32_GENLED__CONN_AP:
		base = 4096;
		speed = 933;
		div = 2;
		break;
	case LWSESP32_GENLED__GOT_IP:
		fadein = us / 10000; /* 100 steps in 1s */
		if (fadein > 100) {
			fadein = 100;
			lws_esp32.genled = LWSESP32_GENLED__OK;
		}
		fadein = 100 - fadein; /* we are fading out */
		/* fallthru */
	case LWSESP32_GENLED__OK:
		if (lws_esp32.genled == LWSESP32_GENLED__OK)
			return 0;

		base = 4096;
		speed = 766;
		div = 3;
		break;
	}

	n = base + (lws_esp32_sine_interp(r / speed) / div);
	return (n * fadein) / 100;
}

esp_err_t lws_esp32_event_passthru(void *ctx, system_event_t *event)
{
#if !defined(CONFIG_LWS_IS_FACTORY_APPLICATION)
	struct lws_group_member *mem;
	int n;
#endif
	char slot[8];
	nvs_handle nvh;
	uint32_t use;

	switch((int)event->event_id) {
	case SYSTEM_EVENT_STA_START:
		//esp_wifi_connect();
//		break;
		/* fallthru */
	case SYSTEM_EVENT_STA_DISCONNECTED:
		lwsl_notice("SYSTEM_EVENT_STA_DISCONNECTED\n");
		lws_esp32.conn_ap = 0;
		lws_esp32.inet = 0;
		lws_esp32.sta_ip[0] = '\0';
		lws_esp32.sta_mask[0] = '\0';
		lws_esp32.sta_gw[0] = '\0';
		lws_gapss_to(LWS_GAPSS_SCAN);
		if (lws_esp32.mdns)
			mdns_service_remove_all(lws_esp32.mdns);
		mdns_free(lws_esp32.mdns);
		lws_esp32.mdns = NULL;
		lws_set_genled(LWSESP32_GENLED__LOST_NETWORK);
		start_scan();
		esp_wifi_connect();
		break;

	case SYSTEM_EVENT_STA_CONNECTED:
		lws_esp32.conn_ap = 1;
		lws_set_genled(LWSESP32_GENLED__CONN_AP);
		break;

	case SYSTEM_EVENT_STA_GOT_IP:
		lwsl_notice("SYSTEM_EVENT_STA_GOT_IP\n");

		lws_esp32.inet = 1;
		lws_set_genled(LWSESP32_GENLED__GOT_IP);

		render_ip(lws_esp32.sta_ip, sizeof(lws_esp32.sta_ip) - 1,
				(uint8_t *)&event->event_info.got_ip.ip_info.ip);
		render_ip(lws_esp32.sta_mask, sizeof(lws_esp32.sta_mask) - 1,
				(uint8_t *)&event->event_info.got_ip.ip_info.netmask);
		render_ip(lws_esp32.sta_gw, sizeof(lws_esp32.sta_gw) - 1,
				(uint8_t *)&event->event_info.got_ip.ip_info.gw);

		if (!nvs_open("lws-station", NVS_READWRITE, &nvh)) {
			lws_snprintf(slot, sizeof(slot) - 1, "%duse", try_slot);
			use = 0;
			nvs_get_u32(nvh, slot, &use);
			nvs_set_u32(nvh, slot, use + 1);
			nvs_commit(nvh);
			nvs_close(nvh);
		}

		lws_gapss_to(LWS_GAPSS_STAT_HAPPY);

#if !defined(CONFIG_LWS_IS_FACTORY_APPLICATION)
		n = mdns_init(TCPIP_ADAPTER_IF_STA, &lws_esp32.mdns);
		if (!n) {
			static char *txta[6];
			int w, h;

			mdns_set_hostname(lws_esp32.mdns, lws_esp32.hostname);
			mdns_set_instance(lws_esp32.mdns, lws_esp32.group);
			mdns_service_add(lws_esp32.mdns, "_lwsgrmem", "_tcp", 443);
			if (txta[0])
				free(txta[0]);
			txta[0] = malloc(32 * ARRAY_SIZE(txta));
			if (!txta[0]) {
				lwsl_notice("mdns OOM\n");
				break;
			}
			txta[1] = &txta[0][32];
			txta[2] = &txta[1][32];
			txta[3] = &txta[2][32];
			txta[4] = &txta[3][32];
			txta[5] = &txta[4][32];

			lws_get_iframe_size(&w, &h);

			lws_snprintf(txta[0], 31, "model=%s", lws_esp32.model);
			lws_snprintf(txta[1], 31, "group=%s", lws_esp32.group);
			lws_snprintf(txta[2], 31, "role=%s", lws_esp32.role);
			lws_snprintf(txta[3], 31, "mac=%s", lws_esp32.mac);
			lws_snprintf(txta[4], 31, "width=%d", w);
			lws_snprintf(txta[5], 31, "height=%d", h);

			mem = lws_esp32.first;
			while (mem) {
				if (mem->flags & 1)
					break;
				mem = mem->next;
			}

			if (!mem) {
				struct lws_group_member *mem = malloc(sizeof(*mem));
				if (mem) {
					mem->last_seen = ~(uint64_t)0;
					strcpy(mem->model, lws_esp32.model);
					strcpy(mem->role, lws_esp32.role);
					strcpy(mem->host, lws_esp32.hostname);
					strcpy(mem->mac, lws_esp32.mac);
					mem->flags = LWS_GROUP_FLAG_SELF;
					lws_get_iframe_size(&mem->width, &mem->height);
					memcpy(&mem->addr, &event->event_info.got_ip.ip_info.ip,
							sizeof(mem->addr));
					memcpy(&mem->addrv6, &event->event_info.got_ip6.ip6_info.ip,
							sizeof(mem->addrv6));
					mem->next = lws_esp32.first;
					lws_esp32.first = mem;
					lws_esp32.extant_group_members++;

					lws_group_member_event_call(LWS_SYSTEM_GROUP_MEMBER_ADD, mem);
				}
			} else { /* update our IP */
					memcpy(&mem->addr, &event->event_info.got_ip.ip_info.ip,
							sizeof(mem->addr));
					memcpy(&mem->addrv6, &event->event_info.got_ip6.ip6_info.ip,
							sizeof(mem->addrv6));
					lws_group_member_event_call(LWS_SYSTEM_GROUP_MEMBER_CHANGE, mem);
			}


			if (mdns_service_txt_set(lws_esp32.mdns, "_lwsgrmem", "_tcp", ARRAY_SIZE(txta),
						 (const char **)txta))
				lwsl_notice("txt set failed\n");
		} else
			lwsl_err("unable to init mdns on STA: %d\n", n);

		mdns_query(lws_esp32.mdns, "_lwsgrmem", "_tcp", 0);
		xTimerStart(mdns_timer, 0);
#endif

		lwsl_notice(" --- Got IP %s\n", lws_esp32.sta_ip);
		break;

	case SYSTEM_EVENT_SCAN_DONE:
		lwsl_notice("SYSTEM_EVENT_SCAN_DONE\n");
		end_scan();
		break;

	default:
		break;
	}

	return ESP_OK;
}

static lws_fop_fd_t IRAM_ATTR
esp32_lws_fops_open(const struct lws_plat_file_ops *fops, const char *filename,
                const char *vfs_path, lws_fop_flags_t *flags)
{
	struct esp32_file *f = malloc(sizeof(*f));
	lws_fop_fd_t fop_fd;
	size_t len, csum;

	lwsl_notice("%s: %s\n", __func__, filename);

	if (!f)
		return NULL;
	f->i = romfs_get_info(lws_esp32_romfs, filename, &len, &csum);
	if (!f->i)
		goto bail;

        fop_fd = malloc(sizeof(*fop_fd));
        if (!fop_fd)
                goto bail;

        fop_fd->fops = fops;
        fop_fd->filesystem_priv = f;
	fop_fd->mod_time = csum;
	*flags |= LWS_FOP_FLAG_MOD_TIME_VALID;
	fop_fd->flags = *flags;
	
	fop_fd->len = len;
	fop_fd->pos = 0;

	return fop_fd;

bail:
	free(f);

	return NULL;
}

static int IRAM_ATTR
esp32_lws_fops_close(lws_fop_fd_t *fop_fd)
{
	free((*fop_fd)->filesystem_priv);
	free(*fop_fd);

	*fop_fd = NULL;

	return 0;
}
static lws_fileofs_t IRAM_ATTR
esp32_lws_fops_seek_cur(lws_fop_fd_t fop_fd, lws_fileofs_t offset_from_cur_pos)
{
	fop_fd->pos += offset_from_cur_pos;
	
	if (fop_fd->pos > fop_fd->len)
		fop_fd->pos = fop_fd->len;

       return 0;
}

static int IRAM_ATTR
esp32_lws_fops_read(lws_fop_fd_t fop_fd, lws_filepos_t *amount, uint8_t *buf,
                   lws_filepos_t len)
{
       struct esp32_file *f = fop_fd->filesystem_priv;
#if 0
       if ((long)buf & 3) {
               lwsl_err("misaligned buf\n");

               return -1;
       }
#endif
       if (fop_fd->pos >= fop_fd->len)
               return 0;

       if (len > fop_fd->len - fop_fd->pos)
               len = fop_fd->len - fop_fd->pos;

       spi_flash_read((uint32_t)(char *)f->i + fop_fd->pos, buf, len);

       *amount = len;
       fop_fd->pos += len;

       return 0;
}

static const struct lws_plat_file_ops fops = {
	.next = &fops_zip,
	.LWS_FOP_OPEN = esp32_lws_fops_open,
	.LWS_FOP_CLOSE = esp32_lws_fops_close,
	.LWS_FOP_READ = esp32_lws_fops_read,
	.LWS_FOP_SEEK_CUR = esp32_lws_fops_seek_cur,
};

int
lws_esp32_wlan_nvs_get(int retry)
{
	nvs_handle nvh;
	char r[2], lws_esp32_force_ap = 0, slot[12];
	size_t s;
	uint8_t mac[6];
	int n;

	esp_efuse_mac_get_default(mac);
	mac[5] |= 1; /* match the AP MAC */
	snprintf(lws_esp32.serial, sizeof(lws_esp32.serial) - 1, "%02X%02X%02X", mac[3], mac[4], mac[5]);
	snprintf(lws_esp32.mac, sizeof(lws_esp32.mac) - 1, "%02X%02X%02X%02X%02X%02X", mac[0],
			mac[1], mac[2], mac[3], mac[4], mac[5]);

	ESP_ERROR_CHECK(nvs_open("lws-station", NVS_READWRITE, &nvh));

	config.sta.ssid[0] = '\0';
	config.sta.password[0] = '\0';

	for (n = 0; n < 4; n++) {
		lws_snprintf(slot, sizeof(slot) - 1, "%dssid", n);
		s = sizeof(lws_esp32.ssid[0]) - 1;
		lws_esp32.ssid[n][0] = '\0';
		nvs_get_str(nvh, slot, lws_esp32.ssid[n], &s);

		lws_snprintf(slot, sizeof(slot) - 1, "%dpassword", n);
		s = sizeof(lws_esp32.password[0]) - 1;
		lws_esp32.password[n][0] = '\0';
		nvs_get_str(nvh, slot, lws_esp32.password[n], &s);
	}

	s = sizeof(lws_esp32.serial) - 1;
	if (nvs_get_str(nvh, "serial", lws_esp32.serial, &s) != ESP_OK)
		lws_esp32_force_ap = 1;
	else
		snprintf((char *)config.ap.ssid, sizeof(config.ap.ssid) - 1,
			 "config-%s-%s", lws_esp32.model, lws_esp32.serial);
	s = sizeof(lws_esp32.opts) - 1;
	if (nvs_get_str(nvh, "opts", lws_esp32.opts, &s) != ESP_OK)
		lws_esp32_force_ap = 1;

	s = sizeof(r);
	if (nvs_get_str(nvh, "region", r, &s) != ESP_OK)
		lws_esp32_force_ap = 1;
	else
		lws_esp32.region = atoi(r);
	lws_esp32.access_pw[0] = '\0';
	nvs_get_str(nvh, "access_pw", lws_esp32.access_pw, &s);

	lws_esp32.group[0] = '\0';
	s = sizeof(lws_esp32.group);
	nvs_get_str(nvh, "group", lws_esp32.group, &s);

	lws_esp32.role[0] = '\0';
	s = sizeof(lws_esp32.role);
	nvs_get_str(nvh, "role", lws_esp32.role, &s);

	/* if group and role defined: group-role */
	if (lws_esp32.group[0] && lws_esp32.role[0])
		lws_snprintf(lws_esp32.hostname, sizeof(lws_esp32.hostname) - 1,
				"%s-%s", lws_esp32.group, lws_esp32.role);
	else /* otherwise model-serial */
		lws_snprintf(lws_esp32.hostname, sizeof(lws_esp32.hostname) - 1,
				"%s-%s", lws_esp32.model, lws_esp32.serial);

	nvs_close(nvh);

	lws_gapss_to(LWS_GAPSS_SCAN);
	start_scan();

	return lws_esp32_force_ap;
}


void
lws_esp32_wlan_config(void)
{
	ledc_timer_config_t ledc_timer = {
	        .bit_num = LEDC_TIMER_13_BIT,
	        .freq_hz = 5000,
	        .speed_mode = LEDC_HIGH_SPEED_MODE,
	        .timer_num = LEDC_TIMER_0
	};
	int n;

	ledc_timer_config(&ledc_timer);

	lws_set_genled(LWSESP32_GENLED__INIT);

	/* user code needs to provide lws_esp32_leds_timer_cb */

        leds_timer = xTimerCreate("lws_leds", pdMS_TO_TICKS(25), 1, NULL,
                          (TimerCallbackFunction_t)lws_esp32_leds_timer_cb);
        scan_timer = xTimerCreate("lws_scan", pdMS_TO_TICKS(10000), 0, NULL,
                          (TimerCallbackFunction_t)lws_esp32_scan_timer_cb);
        debounce_timer = xTimerCreate("lws_db", pdMS_TO_TICKS(100), 0, NULL,
                          (TimerCallbackFunction_t)lws_esp32_debounce_timer_cb);

#if !defined(CONFIG_LWS_IS_FACTORY_APPLICATION)
        mdns_timer = xTimerCreate("lws_mdns", pdMS_TO_TICKS(5000), 0, NULL,
                          (TimerCallbackFunction_t)lws_esp32_mdns_timer_cb);
#endif
	scan_timer_exists = 1;
        xTimerStart(leds_timer, 0);

	*(volatile uint32_t *)PERIPHS_IO_MUX_MTMS_U = FUNC_MTMS_GPIO14;

	gpio_output_set(0, 0, 0, (1 << GPIO_SW));

	n = gpio_install_isr_service(0);
	if (!n) {
		gpio_config_t c;

		c.intr_type = GPIO_INTR_NEGEDGE;
		c.mode = GPIO_MODE_INPUT;
		c.pin_bit_mask = 1 << GPIO_SW;
		c.pull_down_en = 0;
		c.pull_up_en = 0;
		gpio_config(&c);

		if (gpio_isr_handler_add(GPIO_SW, gpio_irq, NULL))
			lwsl_notice("isr handler add for 14 failed\n");
	} else
		lwsl_notice("failed to install gpio isr service: %d\n", n);

	lws_esp32_wlan_nvs_get(0);
	tcpip_adapter_init();
}

void
lws_esp32_wlan_start_ap(void)
{
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK( esp_wifi_init(&cfg));
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_wifi_set_country(lws_esp32.region);

	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &config) );
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK( esp_wifi_start());

	esp_wifi_scan_start(&scan_config, false);

	if (sta_config.sta.ssid[0]) {
		tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, (const char *)&config.ap.ssid[7]);
		esp_wifi_set_auto_connect(1);
		ESP_ERROR_CHECK( esp_wifi_connect());
		ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config));
		ESP_ERROR_CHECK( esp_wifi_connect());
	}
}

void
lws_esp32_wlan_start_station(void)
{
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK( esp_wifi_init(&cfg));
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_wifi_set_country(lws_esp32.region);

	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config));

	ESP_ERROR_CHECK( esp_wifi_start());

	tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, (const char *)&config.ap.ssid[7]);
	esp_wifi_set_auto_connect(1);
	ESP_ERROR_CHECK( esp_wifi_connect());

	lws_esp32_scan_timer_cb(NULL);
}

const esp_partition_t *
lws_esp_ota_get_boot_partition(void)
{
	const esp_partition_t *part = esp_ota_get_boot_partition(), *factory_part, *ota;
	esp_image_header_t eih, ota_eih;
	uint32_t *p_force_factory_magic = (uint32_t *)LWS_MAGIC_REBOOT_TYPE_ADS;

	/* confirm what we are told is the boot part is sane */
	spi_flash_read(part->address , &eih, sizeof(eih));
	factory_part = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
			ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
 	ota = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
			ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
	spi_flash_read(ota->address , &ota_eih, sizeof(ota_eih));

	if (eih.spi_mode == 0xff ||
	    *p_force_factory_magic == LWS_MAGIC_REBOOT_TYPE_FORCED_FACTORY ||
	    *p_force_factory_magic == LWS_MAGIC_REBOOT_TYPE_FORCED_FACTORY_BUTTON
	   ) {
		/*
		 * we believed we were going to boot OTA, but we fell
		 * back to FACTORY in the bootloader when we saw it
		 * had been erased.  esp_ota_get_boot_partition() still
		 * says the OTA partition then even if we are in the
		 * factory partition right now.
		 */
		part = factory_part;
	} 
	
#ifdef CONFIG_LWS_IS_FACTORY_APPLICATION
	else
		if (ota_eih.spi_mode != 0xff &&
		    part->address != factory_part->address) {
			uint8_t buf[4096];
			uint32_t n;
			/*
			 * we are a FACTORY image running in an OTA slot...
			 * it means we were just written and need to copy
			 * ourselves into the FACTORY slot.
			 */
			lwsl_notice("Copying FACTORY update into place 0x%x len 0x%x\n",
				    factory_part->address, factory_part->size);
			esp_task_wdt_feed();
			if (spi_flash_erase_range(factory_part->address, factory_part->size) != ESP_OK) {
	               	        lwsl_err("spi: Failed to erase\n");
	               	        goto retry;
	               	}

			for (n = 0; n < factory_part->size; n += sizeof(buf)) {
				esp_task_wdt_feed();
				spi_flash_read(part->address + n , buf, sizeof(buf));
	                	if (spi_flash_write(factory_part->address + n, buf, sizeof(buf)) != ESP_OK) {
	                	        lwsl_err("spi: Failed to write\n");
	                	        goto retry;
	                	}
			}

			/* destroy our OTA image header */
			spi_flash_erase_range(ota->address, 4096);

			/*
			 * with no viable OTA image, we will come back up in factory
			 * where the user can reload the OTA image
			 */
			lwsl_notice("  FACTORY copy successful, rebooting\n");
retry:
			esp_restart();
		}
#endif

	return part;
}


void
lws_esp32_set_creation_defaults(struct lws_context_creation_info *info)
{
	const esp_partition_t *part;

	memset(info, 0, sizeof(*info));

	lws_set_log_level(63, lwsl_emit_syslog);

	part = lws_esp_ota_get_boot_partition();
	(void)part;

	info->port = 443;
	info->fd_limit_per_thread = 30;
	info->max_http_header_pool = 3;
	info->max_http_header_data = 1024;
	info->pt_serv_buf_size = 4096;
	info->keepalive_timeout = 30;
	info->timeout_secs = 30;
	info->simultaneous_ssl_restriction = 3;
	info->options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS |
		       LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

	info->ssl_cert_filepath = "ssl-pub.pem";
	info->ssl_private_key_filepath = "ssl-pri.pem";
}

int
lws_esp32_get_image_info(const esp_partition_t *part, struct lws_esp32_image *i,
			 char *json, int json_len)
{
	esp_image_segment_header_t eis;
	esp_image_header_t eih;
	uint32_t hdr;

	spi_flash_read(part->address , &eih, sizeof(eih));
	hdr = part->address + sizeof(eih);

	if (eih.magic != ESP_IMAGE_HEADER_MAGIC)
		return 1;

	eis.data_len = 0;
	while (eih.segment_count-- && eis.data_len != 0xffffffff) {
		spi_flash_read(hdr, &eis, sizeof(eis));
		hdr += sizeof(eis) + eis.data_len;
	}
	hdr += (~hdr & 15) + 1;

	i->romfs = hdr + 4;
	spi_flash_read(hdr, &i->romfs_len, sizeof(i->romfs_len));
	i->json = i->romfs + i->romfs_len + 4;
	spi_flash_read(i->json - 4, &i->json_len, sizeof(i->json_len));

	if (i->json_len < json_len - 1)
		json_len = i->json_len;
	spi_flash_read(i->json, json, json_len);
	json[json_len] = '\0';

	return 0;
}

struct lws_context *
lws_esp32_init(struct lws_context_creation_info *info)
{
	const esp_partition_t *part = lws_esp_ota_get_boot_partition();
	struct lws_context *context;
	struct lws_esp32_image i;
	struct lws_vhost *vhost;
	nvs_handle nvh;
	char buf[512];
	size_t s;
	int n;

	ESP_ERROR_CHECK(nvs_open("lws-station", NVS_READWRITE, &nvh));
	n = 0;
	s = 1;
	if (nvs_get_blob(nvh, "ssl-pub.pem", NULL, &s) == ESP_OK)
		n = 1;
	s = 1;
	if (nvs_get_blob(nvh, "ssl-pri.pem", NULL, &s) == ESP_OK)
		n |= 2;
	nvs_close(nvh);

	if (n != 3) {
		/* we are not configured for SSL yet... fall back to port 80 / http */
		info->port = 80;
		info->options &= ~LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		lwsl_notice("No SSL certs... using port 80\n");
	}

	context = lws_create_context(info);
	if (context == NULL) {
		lwsl_err("Failed to create context\n");
		return NULL;
	}

	lws_esp32_get_image_info(part, &i, buf, sizeof(buf) - 1);
	
	lws_esp32_romfs = (romfs_t)i.romfs;
	if (!romfs_mount_check(lws_esp32_romfs)) {
		lwsl_err("Failed to mount ROMFS at %p 0x%x\n", lws_esp32_romfs, i.romfs);
		return NULL;
	}

	lwsl_notice("ROMFS length %uKiB\n", i.romfs_len >> 10);

	puts(buf);

	/* set the lws vfs to use our romfs */

	lws_set_fops(context, &fops);

	vhost = lws_create_vhost(context, info);
	if (!vhost)
		lwsl_err("Failed to create vhost\n");
	else
		lws_init_vhost_client_ssl(info, vhost); 

	lws_protocol_init(context);

	return context;
}

static const uint16_t sineq16[] = {
        0x0000, 0x0191, 0x031e, 0x04a4, 0x061e, 0x0789, 0x08e2, 0x0a24,
        0x0b4e, 0x0c5c, 0x0d4b, 0x0e1a, 0x0ec6, 0x0f4d, 0x0faf, 0x0fea,
};

static uint16_t sine_lu(int n)
{
        switch ((n >> 4) & 3) {
        case 1:
                return 4096 + sineq16[n & 15];
        case 2:
                return 4096 + sineq16[15 - (n & 15)];
        case 3:
                return 4096 - sineq16[n & 15];
        default:
                return  4096 - sineq16[15 - (n & 15)];
        }
}

/* useful for sine led fade patterns */

uint16_t lws_esp32_sine_interp(int n)
{
        /*
         * 2: quadrant
         * 4: table entry in quadrant
         * 4: interp (LSB)
         *
         * total 10 bits / 1024 steps per cycle
	 *
	 * +   0: 0
	 * + 256: 4096
	 * + 512: 8192
	 * + 768: 4096
	 * +1023: 0
         */

        return (sine_lu(n >> 4) * (15 - (n & 15)) +
                sine_lu((n >> 4) + 1) * (n & 15)) / 15;
}

