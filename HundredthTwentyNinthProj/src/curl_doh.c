/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 2018 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
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

#ifndef CURL_DISABLE_DOH

#include "curl_urldata.h"
#include "curl_addrinfo.h"
#include "curl_doh.h"

#include "curl_sendf.h"
#include "curl_multiif.h"
#include "curl_url.h"
#include "curl_share.h"
#include "curl_base64.h"
#include "curl_connect.h"
#include "curl_strdup.h"
#include "curl_dynbuf.h"
/* The last 3 #include files should be in this order */
#include "curl_printf.h"
#include "curl_memory.h"
#include "curl_memdebug.h"

#define DNS_CLASS_IN 0x01

#ifndef CURL_DISABLE_VERBOSE_STRINGS
static const char * const errors[]={
  "",
  "Bad label",
  "Out of range",
  "Label loop",
  "Too small",
  "Out of memory",
  "RDATA length",
  "Malformat",
  "Bad RCODE",
  "Unexpected TYPE",
  "Unexpected CLASS",
  "No content",
  "Bad ID"
};

static const char *doh_strerror(DOHcode code)
{
  if((code >= DOH_OK) && (code <= DOH_DNS_BAD_ID))
    return errors[code];
  return "bad error code";
}
#endif

#ifdef DEBUGBUILD
#define UNITTEST
#else
#define UNITTEST static
#endif

/* @unittest 1655
 */
UNITTEST DOHcode doh_encode(const char *host,
                            DNStype dnstype,
                            unsigned char *dnsp, /* buffer */
                            size_t len,  /* buffer size */
                            size_t *olen) /* output length */
{
  const size_t hostlen = strlen(host);
  unsigned char *orig = dnsp;
  const char *hostp = host;

  /* The expected output length is 16 bytes more than the length of
   * the QNAME-encoding of the host name.
   *
   * A valid DNS name may not contain a zero-length label, except at
   * the end.  For this reason, a name beginning with a dot, or
   * containing a sequence of two or more consecutive dots, is invalid
   * and cannot be encoded as a QNAME.
   *
   * If the host name ends with a trailing dot, the corresponding
   * QNAME-encoding is one byte longer than the host name. If (as is
   * also valid) the hostname is shortened by the omission of the
   * trailing dot, then its QNAME-encoding will be two bytes longer
   * than the host name.
   *
   * Each [ label, dot ] pair is encoded as [ length, label ],
   * preserving overall length.  A final [ label ] without a dot is
   * also encoded as [ length, label ], increasing overall length
   * by one. The encoding is completed by appending a zero byte,
   * representing the zero-length root label, again increasing
   * the overall length by one.
   */

  size_t expected_len;
  DEBUGASSERT(hostlen);
  expected_len = 12 + 1 + hostlen + 4;
  if(host[hostlen-1]!='.')
    expected_len++;

  if(expected_len > (256 + 16)) /* RFCs 1034, 1035 */
    return DOH_DNS_NAME_TOO_LONG;

  if(len < expected_len)
    return DOH_TOO_SMALL_BUFFER;

  *dnsp++ = 0; /* 16 bit id */
  *dnsp++ = 0;
  *dnsp++ = 0x01; /* |QR|   Opcode  |AA|TC|RD| Set the RD bit */
  *dnsp++ = '\0'; /* |RA|   Z    |   RCODE   |                */
  *dnsp++ = '\0';
  *dnsp++ = 1;    /* QDCOUNT (number of entries in the question section) */
  *dnsp++ = '\0';
  *dnsp++ = '\0'; /* ANCOUNT */
  *dnsp++ = '\0';
  *dnsp++ = '\0'; /* NSCOUNT */
  *dnsp++ = '\0';
  *dnsp++ = '\0'; /* ARCOUNT */

  /* encode each label and store it in the QNAME */
  while(*hostp) {
    size_t labellen;
    char *dot = strchr(hostp, '.');
    if(dot)
      labellen = dot - hostp;
    else
      labellen = strlen(hostp);
    if((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      return DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned char)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    if(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535]  */
  *dnsp++ = (unsigned char)(255 & (dnstype>>8)); /* upper 8 bit TYPE */
  *dnsp++ = (unsigned char)(255 & dnstype);      /* lower 8 bit TYPE */

  *dnsp++ = '\0'; /* upper 8 bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  return DOH_OK;
}

static size_t
doh_write_cb(const void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct dynbuf *mem = (struct dynbuf *)userp;

  if(Curl_dyn_addn(mem, contents, realsize))
    return 0;

  return realsize;
}

/* called from multi.c when this DOH transfer is complete */
static int Curl_doh_done(struct Curl_easy *doh, CURLcode result)
{
  struct Curl_easy *data = doh->set.dohfor;
  /* so one of the DOH request done for the 'data' transfer is now complete! */
  data->req.doh.pending--;
  infof(data, "a DOH request is completed, %u to go\n", data->req.doh.pending);
  if(result)
    infof(data, "DOH request %s\n", curl_easy_strerror(result));

  if(!data->req.doh.pending) {
    /* DOH completed */
    curl_slist_free_all(data->req.doh.headers);
    data->req.doh.headers = NULL;
    Curl_expire(data, 0, EXPIRE_RUN_NOW);
  }
  return 0;
}

#define ERROR_CHECK_SETOPT(x,y) \
do {                                      \
  result = curl_easy_setopt(doh, x, y);   \
  if(result)                              \
    goto error;                           \
} while(0)

static CURLcode dohprobe(struct Curl_easy *data,
                         struct dnsprobe *p, DNStype dnstype,
                         const char *host,
                         const char *url, CURLM *multi,
                         struct curl_slist *headers)
{
  struct Curl_easy *doh = NULL;
  char *nurl = NULL;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  DOHcode d = doh_encode(host, dnstype, p->dohbuffer, sizeof(p->dohbuffer),
                         &p->dohlen);
  if(d) {
    failf(data, "Failed to encode DOH packet [%d]\n", d);
    return CURLE_OUT_OF_MEMORY;
  }

  p->dnstype = dnstype;
  Curl_dyn_init(&p->serverdoh, DYN_DOH_RESPONSE);

  /* Note: this is code for sending the DoH request with GET but there's still
     no logic that actually enables this. We should either add that ability or
     yank out the GET code. Discuss! */
  if(data->set.doh_get) {
    char *b64;
    size_t b64len;
    result = Curl_base64url_encode(data, (char *)p->dohbuffer, p->dohlen,
                                   &b64, &b64len);
    if(result)
      goto error;
    nurl = aprintf("%s?dns=%s", url, b64);
    free(b64);
    if(!nurl) {
      result = CURLE_OUT_OF_MEMORY;
      goto error;
    }
    url = nurl;
  }

  timeout_ms = Curl_timeleft(data, NULL, TRUE);
  if(timeout_ms <= 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    goto error;
  }
  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  if(!result) {
    /* pass in the struct pointer via a local variable to please coverity and
       the gcc typecheck helpers */
    struct dynbuf *resp = &p->serverdoh;
    ERROR_CHECK_SETOPT(CURLOPT_URL, url);
    ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_write_cb);
    ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, resp);
    if(!data->set.doh_get) {
      ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, p->dohbuffer);
      ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (long)p->dohlen);
    }
    ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, headers);
#ifdef USE_NGHTTP2
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
#endif
#ifndef CURLDEBUG
    /* enforce HTTPS if not debug */
    ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#else
    /* in debug mode, also allow http */
    ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP|CURLPROTO_HTTPS);
#endif
    ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
    if(data->set.verbose)
      ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
    if(data->set.no_signal)
      ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661 */
    if(data->set.ssl.falsestart)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_FALSESTART, 1L);
    if(data->set.ssl.primary.verifyhost)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST, 2L);
#ifndef CURL_DISABLE_PROXY
    if(data->set.proxy_ssl.primary.verifyhost)
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_SSL_VERIFYHOST, 2L);
    if(data->set.proxy_ssl.primary.verifypeer)
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_SSL_VERIFYPEER, 1L);
    if(data->set.str[STRING_SSL_CAFILE_PROXY]) {
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_CAINFO,
        data->set.str[STRING_SSL_CAFILE_PROXY]);
    }
    if(data->set.str[STRING_SSL_CRLFILE_PROXY]) {
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_CRLFILE,
        data->set.str[STRING_SSL_CRLFILE_PROXY]);
    }
    if(data->set.proxy_ssl.no_revoke)
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);
    else if(data->set.proxy_ssl.revoke_best_effort)
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_SSL_OPTIONS,
                         CURLSSLOPT_REVOKE_BEST_EFFORT);
    if(data->set.str[STRING_SSL_CAPATH_PROXY]) {
      ERROR_CHECK_SETOPT(CURLOPT_PROXY_CAPATH,
        data->set.str[STRING_SSL_CAPATH_PROXY]);
    }
#endif
    if(data->set.ssl.primary.verifypeer)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER, 1L);
    if(data->set.ssl.primary.verifystatus)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS, 1L);
    if(data->set.str[STRING_SSL_CAFILE_ORIG]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
        data->set.str[STRING_SSL_CAFILE_ORIG]);
    }
    if(data->set.str[STRING_SSL_CAPATH_ORIG]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
        data->set.str[STRING_SSL_CAPATH_ORIG]);
    }
    if(data->set.str[STRING_SSL_CRLFILE_ORIG]) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
        data->set.str[STRING_SSL_CRLFILE_ORIG]);
    }
    if(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    if(data->set.str[STRING_SSL_RANDOM_FILE]) {
      ERROR_CHECK_SETOPT(CURLOPT_RANDOM_FILE,
        data->set.str[STRING_SSL_RANDOM_FILE]);
    }
    if(data->set.str[STRING_SSL_EGDSOCKET]) {
      ERROR_CHECK_SETOPT(CURLOPT_EGDSOCKET,
        data->set.str[STRING_SSL_EGDSOCKET]);
    }
    if(data->set.ssl.no_revoke)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);
    else if(data->set.ssl.revoke_best_effort)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_OPTIONS, CURLSSLOPT_REVOKE_BEST_EFFORT);
    if(data->set.ssl.fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl.fsslctx);
    if(data->set.ssl.fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl.fsslctxp);

    doh->set.fmultidone = Curl_doh_done;
    doh->set.dohfor = data; /* identify for which transfer this is done */
    p->easy = doh;

    /* add this transfer to the multi handle */
    if(curl_multi_add_handle(multi, doh))
      goto error;
  }
  else
    goto error;
  free(nurl);
  return CURLE_OK;

  error:
  free(nurl);
  Curl_close(&doh);
  return result;
}

/*
 * Curl_doh() resolves a name using DOH. It resolves a name and returns a
 * 'Curl_addrinfo *' with the address information.
 */

struct Curl_addrinfo *Curl_doh(struct connectdata *conn,
                               const char *hostname,
                               int port,
                               int *waitp)
{
  struct Curl_easy *data = conn->data;
  CURLcode result = CURLE_OK;
  int slot;
  *waitp = TRUE; /* this never returns synchronously */
  (void)conn;
  (void)hostname;
  (void)port;

  /* start clean, consider allocating this struct on demand */
  memset(&data->req.doh, 0, sizeof(struct dohdata));

  conn->bits.doh = TRUE;
  data->req.doh.host = hostname;
  data->req.doh.port = port;
  data->req.doh.headers =
    curl_slist_append(NULL,
                      "Content-Type: application/dns-message");
  if(!data->req.doh.headers)
    goto error;

  if(conn->ip_version != CURL_IPRESOLVE_V6) {
    /* create IPv4 DOH request */
    result = dohprobe(data, &data->req.doh.probe[DOH_PROBE_SLOT_IPADDR_V4],
                      DNS_TYPE_A, hostname, data->set.str[STRING_DOH],
                      data->multi, data->req.doh.headers);
    if(result)
      goto error;
    data->req.doh.pending++;
  }

  if(conn->ip_version != CURL_IPRESOLVE_V4) {
    /* create IPv6 DOH request */
    result = dohprobe(data, &data->req.doh.probe[DOH_PROBE_SLOT_IPADDR_V6],
                      DNS_TYPE_AAAA, hostname, data->set.str[STRING_DOH],
                      data->multi, data->req.doh.headers);
    if(result)
      goto error;
    data->req.doh.pending++;
  }
  return NULL;

  error:
  curl_slist_free_all(data->req.doh.headers);
  data->req.doh.headers = NULL;
  for(slot = 0; slot < DOH_PROBE_SLOTS; slot++) {
    Curl_close(&data->req.doh.probe[slot].easy);
  }
  return NULL;
}

static DOHcode skipqname(const unsigned char *doh, size_t dohlen,
                         unsigned int *indexp)
{
  unsigned char length;
  do {
    if(dohlen < (*indexp + 1))
      return DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    if((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      if(dohlen < (*indexp + 2))
        return DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      break;
    }
    if(length & 0xc0)
      return DOH_DNS_BAD_LABEL;
    if(dohlen < (*indexp + 1 + length))
      return DOH_DNS_OUT_OF_RANGE;
    *indexp += 1 + length;
  } while(length);
  return DOH_OK;
}

static unsigned short get16bit(const unsigned char *doh, int index)
{
  return (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

static unsigned int get32bit(const unsigned char *doh, int index)
{
   /* make clang and gcc optimize this to bswap by incrementing
      the pointer first. */
   doh += index;

   /* avoid undefined behaviour by casting to unsigned before shifting
      24 bits, possibly into the sign bit. codegen is same, but
      ub sanitizer won't be upset */
  return ( (unsigned)doh[0] << 24) | (doh[1] << 16) |(doh[2] << 8) | doh[3];
}

static DOHcode store_a(const unsigned char *doh, int index, struct dohentry *d)
{
  /* silently ignore addresses over the limit */
  if(d->numaddr < DOH_MAX_ADDR) {
    struct dohaddr *a = &d->addr[d->numaddr];
    a->type = DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
  return DOH_OK;
}

static DOHcode store_aaaa(const unsigned char *doh,
                          int index,
                          struct dohentry *d)
{
  /* silently ignore addresses over the limit */
  if(d->numaddr < DOH_MAX_ADDR) {
    struct dohaddr *a = &d->addr[d->numaddr];
    a->type = DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
  return DOH_OK;
}

static DOHcode store_cname(const unsigned char *doh,
                           size_t dohlen,
                           unsigned int index,
                           struct dohentry *d)
{
  struct dynbuf *c;
  unsigned int loop = 128; /* a valid DNS name can never loop this much */
  unsigned char length;

  if(d->numcname == DOH_MAX_CNAME)
    return DOH_OK; /* skip! */

  c = &d->cname[d->numcname++];
  do {
    if(index >= dohlen)
      return DOH_DNS_OUT_OF_RANGE;
    length = doh[index];
    if((length & 0xc0) == 0xc0) {
      int newpos;
      /* name pointer, get the new offset (14 bits) */
      if((index + 1) >= dohlen)
        return DOH_DNS_OUT_OF_RANGE;

      /* move to the new index */
      newpos = (length & 0x3f) << 8 | doh[index + 1];
      index = newpos;
      continue;
    }
    else if(length & 0xc0)
      return DOH_DNS_BAD_LABEL; /* bad input */
    else
      index++;

    if(length) {
      if(Curl_dyn_len(c)) {
        if(Curl_dyn_add(c, "."))
          return DOH_OUT_OF_MEM;
      }
      if((index + length) > dohlen)
        return DOH_DNS_BAD_LABEL;

      if(Curl_dyn_addn(c, &doh[index], length))
        return DOH_OUT_OF_MEM;
      index += length;
    }
  } while(length && --loop);

  if(!loop)
    return DOH_DNS_LABEL_LOOP;
  return DOH_OK;
}

static DOHcode rdata(const unsigned char *doh,
                     size_t dohlen,
                     unsigned short rdlength,
                     unsigned short type,
                     int index,
                     struct dohentry *d)
{
  /* RDATA
     - A (TYPE 1):  4 bytes
     - AAAA (TYPE 28): 16 bytes
     - NS (TYPE 2): N bytes */
  DOHcode rc;

  switch(type) {
  case DNS_TYPE_A:
    if(rdlength != 4)
      return DOH_DNS_RDATA_LEN;
    rc = store_a(doh, index, d);
    if(rc)
      return rc;
    break;
  case DNS_TYPE_AAAA:
    if(rdlength != 16)
      return DOH_DNS_RDATA_LEN;
    rc = store_aaaa(doh, index, d);
    if(rc)
      return rc;
    break;
  case DNS_TYPE_CNAME:
    rc = store_cname(doh, dohlen, index, d);
    if(rc)
      return rc;
    break;
  case DNS_TYPE_DNAME:
    /* explicit for clarity; just skip; rely on synthesized CNAME  */
    break;
  default:
    /* unsupported type, just skip it */
    break;
  }
  return DOH_OK;
}

UNITTEST void de_init(struct dohentry *de)
{
  int i;
  memset(de, 0, sizeof(*de));
  de->ttl = INT_MAX;
  for(i = 0; i < DOH_MAX_CNAME; i++)
    Curl_dyn_init(&de->cname[i], DYN_DOH_CNAME);
}


UNITTEST DOHcode doh_decode(const unsigned char *doh,
                            size_t dohlen,
                            DNStype dnstype,
                            struct dohentry *d)
{
  unsigned char rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned int index = 12;
  DOHcode rc;

  if(dohlen < 12)
    return DOH_TOO_SMALL_BUFFER; /* too small */
  if(!doh || doh[0] || doh[1])
    return DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  if(rcode)
    return DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = get16bit(doh, 4);
  while(qdcount) {
    rc = skipqname(doh, dohlen, &index);
    if(rc)
      return rc; /* bad qname */
    if(dohlen < (index + 4))
      return DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = get16bit(doh, 6);
  while(ancount) {
    unsigned short class;
    unsigned int ttl;

    rc = skipqname(doh, dohlen, &index);
    if(rc)
      return rc; /* bad qname */

    if(dohlen < (index + 2))
      return DOH_DNS_OUT_OF_RANGE;

    type = get16bit(doh, index);
    if((type != DNS_TYPE_CNAME)    /* may be synthesized from DNAME */
       && (type != DNS_TYPE_DNAME) /* if present, accept and ignore */
       && (type != dnstype))
      /* Not the same type as was asked for nor CNAME nor DNAME */
      return DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    if(dohlen < (index + 2))
      return DOH_DNS_OUT_OF_RANGE;
    class = get16bit(doh, index);
    if(DNS_CLASS_IN != class)
      return DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    if(dohlen < (index + 4))
      return DOH_DNS_OUT_OF_RANGE;

    ttl = get32bit(doh, index);
    if(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    if(dohlen < (index + 2))
      return DOH_DNS_OUT_OF_RANGE;

    rdlength = get16bit(doh, index);
    index += 2;
    if(dohlen < (index + rdlength))
      return DOH_DNS_OUT_OF_RANGE;

    rc = rdata(doh, dohlen, rdlength, type, index, d);
    if(rc)
      return rc; /* bad rdata */
    index += rdlength;
    ancount--;
  }

  nscount = get16bit(doh, 8);
  while(nscount) {
    rc = skipqname(doh, dohlen, &index);
    if(rc)
      return rc; /* bad qname */

    if(dohlen < (index + 8))
      return DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, class and ttl */

    if(dohlen < (index + 2))
      return DOH_DNS_OUT_OF_RANGE;

    rdlength = get16bit(doh, index);
    index += 2;
    if(dohlen < (index + rdlength))
      return DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = get16bit(doh, 10);
  while(arcount) {
    rc = skipqname(doh, dohlen, &index);
    if(rc)
      return rc; /* bad qname */

    if(dohlen < (index + 8))
      return DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, class and ttl */

    if(dohlen < (index + 2))
      return DOH_DNS_OUT_OF_RANGE;

    rdlength = get16bit(doh, index);
    index += 2;
    if(dohlen < (index + rdlength))
      return DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  if(index != dohlen)
    return DOH_DNS_MALFORMAT; /* something is wrong */

  if((type != DNS_TYPE_NS) && !d->numcname && !d->numaddr)
    /* nothing stored! */
    return DOH_NO_CONTENT;

  return DOH_OK; /* ok */
}

#ifndef CURL_DISABLE_VERBOSE_STRINGS
static void showdoh(struct Curl_easy *data,
                    const struct dohentry *d)
{
  int i;
  infof(data, "TTL: %u seconds\n", d->ttl);
  for(i = 0; i < d->numaddr; i++) {
    const struct dohaddr *a = &d->addr[i];
    if(a->type == DNS_TYPE_A) {
      infof(data, "DOH A: %u.%u.%u.%u\n",
            a->ip.v4[0], a->ip.v4[1],
            a->ip.v4[2], a->ip.v4[3]);
    }
    else if(a->type == DNS_TYPE_AAAA) {
      int j;
      char buffer[128];
      char *ptr;
      size_t len;
      msnprintf(buffer, 128, "DOH AAAA: ");
      ptr = &buffer[10];
      len = 118;
      for(j = 0; j < 16; j += 2) {
        size_t l;
        msnprintf(ptr, len, "%s%02x%02x", j?":":"", d->addr[i].ip.v6[j],
                  d->addr[i].ip.v6[j + 1]);
        l = strlen(ptr);
        len -= l;
        ptr += l;
      }
      infof(data, "%s\n", buffer);
    }
  }
  for(i = 0; i < d->numcname; i++) {
    infof(data, "CNAME: %s\n", Curl_dyn_ptr(&d->cname[i]));
  }
}
#else
#define showdoh(x,y)
#endif

/*
 * doh2ai()
 *
 * This function returns a pointer to the first element of a newly allocated
 * Curl_addrinfo struct linked list filled with the data from a set of DOH
 * lookups.  Curl_addrinfo is meant to work like the addrinfo struct does for
 * a IPv6 stack, but usable also for IPv4, all hosts and environments.
 *
 * The memory allocated by this function *MUST* be free'd later on calling
 * Curl_freeaddrinfo().  For each successful call to this function there
 * must be an associated call later to Curl_freeaddrinfo().
 */

static struct Curl_addrinfo *
doh2ai(const struct dohentry *de, const char *hostname, int port)
{
  struct Curl_addrinfo *ai;
  struct Curl_addrinfo *prevai = NULL;
  struct Curl_addrinfo *firstai = NULL;
  struct sockaddr_in *addr;
#ifdef ENABLE_IPV6
  struct sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  int i;
  size_t hostlen = strlen(hostname) + 1; /* include zero terminator */

  if(!de)
    /* no input == no output! */
    return NULL;

  for(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    if(de->addr[i].type == DNS_TYPE_AAAA) {
#ifndef ENABLE_IPV6
      /* we can't handle IPv6 addresses */
      continue;
#else
      ss_size = sizeof(struct sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    else {
      ss_size = sizeof(struct sockaddr_in);
      addrtype = AF_INET;
    }

    ai = calloc(1, sizeof(struct Curl_addrinfo) + ss_size + hostlen);
    if(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      break;
    }
    ai->ai_addr = (void *)((char *)ai + sizeof(struct Curl_addrinfo));
    ai->ai_canonname = (void *)((char *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    if(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    if(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    switch(ai->ai_family) {
    case AF_INET:
      addr = (void *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(sizeof(struct in_addr) == sizeof(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, sizeof(struct in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      break;

#ifdef ENABLE_IPV6
    case AF_INET6:
      addr6 = (void *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(sizeof(struct in6_addr) == sizeof(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, sizeof(struct in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      break;
#endif
    }

    prevai = ai;
  }

  if(result) {
    Curl_freeaddrinfo(firstai);
    firstai = NULL;
  }

  return firstai;
}

#ifndef CURL_DISABLE_VERBOSE_STRINGS
static const char *type2name(DNStype dnstype)
{
  return (dnstype == DNS_TYPE_A)?"A":"AAAA";
}
#endif

UNITTEST void de_cleanup(struct dohentry *d)
{
  int i = 0;
  for(i = 0; i < d->numcname; i++) {
    Curl_dyn_free(&d->cname[i]);
  }
}

CURLcode Curl_doh_is_resolved(struct connectdata *conn,
                              struct Curl_dns_entry **dnsp)
{
  CURLcode result;
  struct Curl_easy *data = conn->data;
  *dnsp = NULL; /* defaults to no response */

  if(!data->req.doh.probe[DOH_PROBE_SLOT_IPADDR_V4].easy &&
     !data->req.doh.probe[DOH_PROBE_SLOT_IPADDR_V6].easy) {
    failf(data, "Could not DOH-resolve: %s", conn->async.hostname);
    return conn->bits.proxy?CURLE_COULDNT_RESOLVE_PROXY:
      CURLE_COULDNT_RESOLVE_HOST;
  }
  else if(!data->req.doh.pending) {
    DOHcode rc[DOH_PROBE_SLOTS] = {
      DOH_OK, DOH_OK
    };
    struct dohentry de;
    int slot;
    /* remove DOH handles from multi handle and close them */
    for(slot = 0; slot < DOH_PROBE_SLOTS; slot++) {
      curl_multi_remove_handle(data->multi, data->req.doh.probe[slot].easy);
      Curl_close(&data->req.doh.probe[slot].easy);
    }
    /* parse the responses, create the struct and return it! */
    de_init(&de);
    for(slot = 0; slot < DOH_PROBE_SLOTS; slot++) {
      struct dnsprobe *p = &data->req.doh.probe[slot];
      if(!p->dnstype)
        continue;
      rc[slot] = doh_decode(Curl_dyn_uptr(&p->serverdoh),
                            Curl_dyn_len(&p->serverdoh),
                            p->dnstype,
                            &de);
      Curl_dyn_free(&p->serverdoh);
      if(rc[slot]) {
        infof(data, "DOH: %s type %s for %s\n", doh_strerror(rc[slot]),
              type2name(p->dnstype), data->req.doh.host);
      }
    } /* next slot */

    result = CURLE_COULDNT_RESOLVE_HOST; /* until we know better */
    if(!rc[DOH_PROBE_SLOT_IPADDR_V4] || !rc[DOH_PROBE_SLOT_IPADDR_V6]) {
      /* we have an address, of one kind or other */
      struct Curl_dns_entry *dns;
      struct Curl_addrinfo *ai;

      infof(data, "DOH Host name: %s\n", data->req.doh.host);
      showdoh(data, &de);

      ai = doh2ai(&de, data->req.doh.host, data->req.doh.port);
      if(!ai) {
        de_cleanup(&de);
        return CURLE_OUT_OF_MEMORY;
      }

      if(data->share)
        Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);

      /* we got a response, store it in the cache */
      dns = Curl_cache_addr(data, ai, data->req.doh.host, data->req.doh.port);

      if(data->share)
        Curl_share_unlock(data, CURL_LOCK_DATA_DNS);

      if(!dns) {
        /* returned failure, bail out nicely */
        Curl_freeaddrinfo(ai);
      }
      else {
        conn->async.dns = dns;
        *dnsp = dns;
        result = CURLE_OK;      /* address resolution OK */
      }
    } /* address processing done */

    /* Now process any build-specific attributes retrieved from DNS */

    /* All done */
    de_cleanup(&de);
    return result;

  } /* !data->req.doh.pending */

  /* else wait for pending DOH transactions to complete */
  return CURLE_OK;
}

#endif /* CURL_DISABLE_DOH */
