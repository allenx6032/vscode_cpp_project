/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2022, Daniel Stenberg, <daniel.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#include "curl_tool_setup.h"
#include "curl_tool_help.h"

/*
 * DO NOT edit tool_listhelp.c manually.
 * This source file is generated with the following command:

  cd $srcroot/docs/cmdline-opts
  ./gen.pl listhelp *.d > $srcroot/src/tool_listhelp.c
 */

const struct helptxt helptext[] = {
  {"    --abstract-unix-socket <path>",
   "Connect via abstract Unix domain socket",
   CURLHELP_CONNECTION},
  {"    --alt-svc <file name>",
   "Enable alt-svc with this cache file",
   CURLHELP_HTTP},
  {"    --anyauth",
   "Pick any authentication method",
   CURLHELP_HTTP | CURLHELP_PROXY | CURLHELP_AUTH},
  {"-a, --append",
   "Append to target file when uploading",
   CURLHELP_FTP | CURLHELP_SFTP},
  {"    --aws-sigv4 <provider1[:provider2[:region[:service]]]>",
   "Use AWS V4 signature authentication",
   CURLHELP_AUTH | CURLHELP_HTTP},
  {"    --basic",
   "Use HTTP Basic Authentication",
   CURLHELP_AUTH},
  {"    --cacert <file>",
   "CA certificate to verify peer against",
   CURLHELP_TLS},
  {"    --capath <dir>",
   "CA directory to verify peer against",
   CURLHELP_TLS},
  {"-E, --cert <certificate[:password]>",
   "Client certificate file and password",
   CURLHELP_TLS},
  {"    --cert-status",
   "Verify the status of the server cert via OCSP-staple",
   CURLHELP_TLS},
  {"    --cert-type <type>",
   "Certificate type (DER/PEM/ENG/P12)",
   CURLHELP_TLS},
  {"    --ciphers <list of ciphers>",
   "SSL ciphers to use",
   CURLHELP_TLS},
  {"    --compressed",
   "Request compressed response",
   CURLHELP_HTTP},
  {"    --compressed-ssh",
   "Enable SSH compression",
   CURLHELP_SCP | CURLHELP_SSH},
  {"-K, --config <file>",
   "Read config from a file",
   CURLHELP_CURL},
  {"    --connect-timeout <fractional seconds>",
   "Maximum time allowed for connection",
   CURLHELP_CONNECTION},
  {"    --connect-to <HOST1:PORT1:HOST2:PORT2>",
   "Connect to host",
   CURLHELP_CONNECTION},
  {"-C, --continue-at <offset>",
   "Resumed transfer offset",
   CURLHELP_CONNECTION},
  {"-b, --cookie <data|filename>",
   "Send cookies from string/file",
   CURLHELP_HTTP},
  {"-c, --cookie-jar <filename>",
   "Write cookies to <filename> after operation",
   CURLHELP_HTTP},
  {"    --create-dirs",
   "Create necessary local directory hierarchy",
   CURLHELP_CURL},
  {"    --create-file-mode <mode>",
   "File mode for created files",
   CURLHELP_SFTP | CURLHELP_SCP | CURLHELP_FILE | CURLHELP_UPLOAD},
  {"    --crlf",
   "Convert LF to CRLF in upload",
   CURLHELP_FTP | CURLHELP_SMTP},
  {"    --crlfile <file>",
   "Use this CRL list",
   CURLHELP_TLS},
  {"    --curves <algorithm list>",
   "(EC) TLS key exchange algorithm(s) to request",
   CURLHELP_TLS},
  {"-d, --data <data>",
   "HTTP POST data",
   CURLHELP_IMPORTANT | CURLHELP_HTTP | CURLHELP_POST | CURLHELP_UPLOAD},
  {"    --data-ascii <data>",
   "HTTP POST ASCII data",
   CURLHELP_HTTP | CURLHELP_POST | CURLHELP_UPLOAD},
  {"    --data-binary <data>",
   "HTTP POST binary data",
   CURLHELP_HTTP | CURLHELP_POST | CURLHELP_UPLOAD},
  {"    --data-raw <data>",
   "HTTP POST data, '@' allowed",
   CURLHELP_HTTP | CURLHELP_POST | CURLHELP_UPLOAD},
  {"    --data-urlencode <data>",
   "HTTP POST data URL encoded",
   CURLHELP_HTTP | CURLHELP_POST | CURLHELP_UPLOAD},
  {"    --delegation <LEVEL>",
   "GSS-API delegation permission",
   CURLHELP_AUTH},
  {"    --digest",
   "Use HTTP Digest Authentication",
   CURLHELP_PROXY | CURLHELP_AUTH | CURLHELP_HTTP},
  {"-q, --disable",
   "Disable .curlrc",
   CURLHELP_CURL},
  {"    --disable-eprt",
   "Inhibit using EPRT or LPRT",
   CURLHELP_FTP},
  {"    --disable-epsv",
   "Inhibit using EPSV",
   CURLHELP_FTP},
  {"    --disallow-username-in-url",
   "Disallow username in URL",
   CURLHELP_CURL | CURLHELP_HTTP},
  {"    --dns-interface <interface>",
   "Interface to use for DNS requests",
   CURLHELP_DNS},
  {"    --dns-ipv4-addr <address>",
   "IPv4 address to use for DNS requests",
   CURLHELP_DNS},
  {"    --dns-ipv6-addr <address>",
   "IPv6 address to use for DNS requests",
   CURLHELP_DNS},
  {"    --dns-servers <addresses>",
   "DNS server addrs to use",
   CURLHELP_DNS},
  {"    --doh-cert-status",
   "Verify the status of the DoH server cert via OCSP-staple",
   CURLHELP_DNS | CURLHELP_TLS},
  {"    --doh-insecure",
   "Allow insecure DoH server connections",
   CURLHELP_DNS | CURLHELP_TLS},
  {"    --doh-url <URL>",
   "Resolve host names over DoH",
   CURLHELP_DNS},
  {"-D, --dump-header <filename>",
   "Write the received headers to <filename>",
   CURLHELP_HTTP | CURLHELP_FTP},
  {"    --egd-file <file>",
   "EGD socket path for random data",
   CURLHELP_TLS},
  {"    --engine <name>",
   "Crypto engine to use",
   CURLHELP_TLS},
  {"    --etag-compare <file>",
   "Pass an ETag from a file as a custom header",
   CURLHELP_HTTP},
  {"    --etag-save <file>",
   "Parse ETag from a request and save it to a file",
   CURLHELP_HTTP},
  {"    --expect100-timeout <seconds>",
   "How long to wait for 100-continue",
   CURLHELP_HTTP},
  {"-f, --fail",
   "Fail fast with no output on HTTP errors",
   CURLHELP_IMPORTANT | CURLHELP_HTTP},
  {"    --fail-early",
   "Fail on first transfer error, do not continue",
   CURLHELP_CURL},
  {"    --fail-with-body",
   "Fail on HTTP errors but save the body",
   CURLHELP_HTTP | CURLHELP_OUTPUT},
  {"    --false-start",
   "Enable TLS False Start",
   CURLHELP_TLS},
  {"-F, --form <name=content>",
   "Specify multipart MIME data",
   CURLHELP_HTTP | CURLHELP_UPLOAD},
  {"    --form-escape",
   "Escape multipart form field/file names using backslash",
   CURLHELP_HTTP | CURLHELP_UPLOAD},
  {"    --form-string <name=string>",
   "Specify multipart MIME data",
   CURLHELP_HTTP | CURLHELP_UPLOAD},
  {"    --ftp-account <data>",
   "Account data string",
   CURLHELP_FTP | CURLHELP_AUTH},
  {"    --ftp-alternative-to-user <command>",
   "String to replace USER [name]",
   CURLHELP_FTP},
  {"    --ftp-create-dirs",
   "Create the remote dirs if not present",
   CURLHELP_FTP | CURLHELP_SFTP | CURLHELP_CURL},
  {"    --ftp-method <method>",
   "Control CWD usage",
   CURLHELP_FTP},
  {"    --ftp-pasv",
   "Use PASV/EPSV instead of PORT",
   CURLHELP_FTP},
  {"-P, --ftp-port <address>",
   "Use PORT instead of PASV",
   CURLHELP_FTP},
  {"    --ftp-pret",
   "Send PRET before PASV",
   CURLHELP_FTP},
  {"    --ftp-skip-pasv-ip",
   "Skip the IP address for PASV",
   CURLHELP_FTP},
  {"    --ftp-ssl-ccc",
   "Send CCC after authenticating",
   CURLHELP_FTP | CURLHELP_TLS},
  {"    --ftp-ssl-ccc-mode <active/passive>",
   "Set CCC mode",
   CURLHELP_FTP | CURLHELP_TLS},
  {"    --ftp-ssl-control",
   "Require SSL/TLS for FTP login, clear for transfer",
   CURLHELP_FTP | CURLHELP_TLS},
  {"-G, --get",
   "Put the post data in the URL and use GET",
   CURLHELP_HTTP | CURLHELP_UPLOAD},
  {"-g, --globoff",
   "Disable URL sequences and ranges using {} and []",
   CURLHELP_CURL},
  {"    --happy-eyeballs-timeout-ms <milliseconds>",
   "Time for IPv6 before trying IPv4",
   CURLHELP_CONNECTION},
  {"    --haproxy-protocol",
   "Send HAProxy PROXY protocol v1 header",
   CURLHELP_HTTP | CURLHELP_PROXY},
  {"-I, --head",
   "Show document info only",
   CURLHELP_HTTP | CURLHELP_FTP | CURLHELP_FILE},
  {"-H, --header <header/@file>",
   "Pass custom header(s) to server",
   CURLHELP_HTTP},
  {"-h, --help <category>",
   "Get help for commands",
   CURLHELP_IMPORTANT | CURLHELP_CURL},
  {"    --hostpubmd5 <md5>",
   "Acceptable MD5 hash of the host public key",
   CURLHELP_SFTP | CURLHELP_SCP},
  {"    --hostpubsha256 <sha256>",
   "Acceptable SHA256 hash of the host public key",
   CURLHELP_SFTP | CURLHELP_SCP},
  {"    --hsts <file name>",
   "Enable HSTS with this cache file",
   CURLHELP_HTTP},
  {"    --http0.9",
   "Allow HTTP 0.9 responses",
   CURLHELP_HTTP},
  {"-0, --http1.0",
   "Use HTTP 1.0",
   CURLHELP_HTTP},
  {"    --http1.1",
   "Use HTTP 1.1",
   CURLHELP_HTTP},
  {"    --http2",
   "Use HTTP 2",
   CURLHELP_HTTP},
  {"    --http2-prior-knowledge",
   "Use HTTP 2 without HTTP/1.1 Upgrade",
   CURLHELP_HTTP},
  {"    --http3",
   "Use HTTP v3",
   CURLHELP_HTTP},
  {"    --ignore-content-length",
   "Ignore the size of the remote resource",
   CURLHELP_HTTP | CURLHELP_FTP},
  {"-i, --include",
   "Include protocol response headers in the output",
   CURLHELP_IMPORTANT | CURLHELP_VERBOSE},
  {"-k, --insecure",
   "Allow insecure server connections",
   CURLHELP_TLS | CURLHELP_SFTP | CURLHELP_SCP},
  {"    --interface <name>",
   "Use network INTERFACE (or address)",
   CURLHELP_CONNECTION},
  {"-4, --ipv4",
   "Resolve names to IPv4 addresses",
   CURLHELP_CONNECTION | CURLHELP_DNS},
  {"-6, --ipv6",
   "Resolve names to IPv6 addresses",
   CURLHELP_CONNECTION | CURLHELP_DNS},
  {"    --json <data>",
   "HTTP POST JSON",
   CURLHELP_HTTP | CURLHELP_POST | CURLHELP_UPLOAD},
  {"-j, --junk-session-cookies",
   "Ignore session cookies read from file",
   CURLHELP_HTTP},
  {"    --keepalive-time <seconds>",
   "Interval time for keepalive probes",
   CURLHELP_CONNECTION},
  {"    --key <key>",
   "Private key file name",
   CURLHELP_TLS | CURLHELP_SSH},
  {"    --key-type <type>",
   "Private key file type (DER/PEM/ENG)",
   CURLHELP_TLS},
  {"    --krb <level>",
   "Enable Kerberos with security <level>",
   CURLHELP_FTP},
  {"    --libcurl <file>",
   "Dump libcurl equivalent code of this command line",
   CURLHELP_CURL},
  {"    --limit-rate <speed>",
   "Limit transfer speed to RATE",
   CURLHELP_CONNECTION},
  {"-l, --list-only",
   "List only mode",
   CURLHELP_FTP | CURLHELP_POP3},
  {"    --local-port <num/range>",
   "Force use of RANGE for local port numbers",
   CURLHELP_CONNECTION},
  {"-L, --location",
   "Follow redirects",
   CURLHELP_HTTP},
  {"    --location-trusted",
   "Like --location, and send auth to other hosts",
   CURLHELP_HTTP | CURLHELP_AUTH},
  {"    --login-options <options>",
   "Server login options",
   CURLHELP_IMAP | CURLHELP_POP3 | CURLHELP_SMTP | CURLHELP_AUTH},
  {"    --mail-auth <address>",
   "Originator address of the original email",
   CURLHELP_SMTP},
  {"    --mail-from <address>",
   "Mail from this address",
   CURLHELP_SMTP},
  {"    --mail-rcpt <address>",
   "Mail to this address",
   CURLHELP_SMTP},
  {"    --mail-rcpt-allowfails",
   "Allow RCPT TO command to fail for some recipients",
   CURLHELP_SMTP},
  {"-M, --manual",
   "Display the full manual",
   CURLHELP_CURL},
  {"    --max-filesize <bytes>",
   "Maximum file size to download",
   CURLHELP_CONNECTION},
  {"    --max-redirs <num>",
   "Maximum number of redirects allowed",
   CURLHELP_HTTP},
  {"-m, --max-time <fractional seconds>",
   "Maximum time allowed for transfer",
   CURLHELP_CONNECTION},
  {"    --metalink",
   "Process given URLs as metalink XML file",
   CURLHELP_MISC},
  {"    --negotiate",
   "Use HTTP Negotiate (SPNEGO) authentication",
   CURLHELP_AUTH | CURLHELP_HTTP},
  {"-n, --netrc",
   "Must read .netrc for user name and password",
   CURLHELP_CURL},
  {"    --netrc-file <filename>",
   "Specify FILE for netrc",
   CURLHELP_CURL},
  {"    --netrc-optional",
   "Use either .netrc or URL",
   CURLHELP_CURL},
  {"-:, --next",
   "Make next URL use its separate set of options",
   CURLHELP_CURL},
  {"    --no-alpn",
   "Disable the ALPN TLS extension",
   CURLHELP_TLS | CURLHELP_HTTP},
  {"-N, --no-buffer",
   "Disable buffering of the output stream",
   CURLHELP_CURL},
  {"    --no-clobber",
   "Do not overwrite files that already exist",
   CURLHELP_CURL | CURLHELP_OUTPUT},
  {"    --no-keepalive",
   "Disable TCP keepalive on the connection",
   CURLHELP_CONNECTION},
  {"    --no-npn",
   "Disable the NPN TLS extension",
   CURLHELP_TLS | CURLHELP_HTTP},
  {"    --no-progress-meter",
   "Do not show the progress meter",
   CURLHELP_VERBOSE},
  {"    --no-sessionid",
   "Disable SSL session-ID reusing",
   CURLHELP_TLS},
  {"    --noproxy <no-proxy-list>",
   "List of hosts which do not use proxy",
   CURLHELP_PROXY},
  {"    --ntlm",
   "Use HTTP NTLM authentication",
   CURLHELP_AUTH | CURLHELP_HTTP},
  {"    --ntlm-wb",
   "Use HTTP NTLM authentication with winbind",
   CURLHELP_AUTH | CURLHELP_HTTP},
  {"    --oauth2-bearer <token>",
   "OAuth 2 Bearer Token",
   CURLHELP_AUTH},
  {"-o, --output <file>",
   "Write to file instead of stdout",
   CURLHELP_IMPORTANT | CURLHELP_CURL},
  {"    --output-dir <dir>",
   "Directory to save files in",
   CURLHELP_CURL},
  {"-Z, --parallel",
   "Perform transfers in parallel",
   CURLHELP_CONNECTION | CURLHELP_CURL},
  {"    --parallel-immediate",
   "Do not wait for multiplexing (with --parallel)",
   CURLHELP_CONNECTION | CURLHELP_CURL},
  {"    --parallel-max <num>",
   "Maximum concurrency for parallel transfers",
   CURLHELP_CONNECTION | CURLHELP_CURL},
  {"    --pass <phrase>",
   "Pass phrase for the private key",
   CURLHELP_SSH | CURLHELP_TLS | CURLHELP_AUTH},
  {"    --path-as-is",
   "Do not squash .. sequences in URL path",
   CURLHELP_CURL},
  {"    --pinnedpubkey <hashes>",
   "FILE/HASHES Public key to verify peer against",
   CURLHELP_TLS},
  {"    --post301",
   "Do not switch to GET after following a 301",
   CURLHELP_HTTP | CURLHELP_POST},
  {"    --post302",
   "Do not switch to GET after following a 302",
   CURLHELP_HTTP | CURLHELP_POST},
  {"    --post303",
   "Do not switch to GET after following a 303",
   CURLHELP_HTTP | CURLHELP_POST},
  {"    --preproxy [protocol://]host[:port]",
   "Use this proxy first",
   CURLHELP_PROXY},
  {"-#, --progress-bar",
   "Display transfer progress as a bar",
   CURLHELP_VERBOSE},
  {"    --proto <protocols>",
   "Enable/disable PROTOCOLS",
   CURLHELP_CONNECTION | CURLHELP_CURL},
  {"    --proto-default <protocol>",
   "Use PROTOCOL for any URL missing a scheme",
   CURLHELP_CONNECTION | CURLHELP_CURL},
  {"    --proto-redir <protocols>",
   "Enable/disable PROTOCOLS on redirect",
   CURLHELP_CONNECTION | CURLHELP_CURL},
  {"-x, --proxy [protocol://]host[:port]",
   "Use this proxy",
   CURLHELP_PROXY},
  {"    --proxy-anyauth",
   "Pick any proxy authentication method",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --proxy-basic",
   "Use Basic authentication on the proxy",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --proxy-cacert <file>",
   "CA certificate to verify peer against for proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-capath <dir>",
   "CA directory to verify peer against for proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-cert <cert[:passwd]>",
   "Set client certificate for proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-cert-type <type>",
   "Client certificate type for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-ciphers <list>",
   "SSL ciphers to use for proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-crlfile <file>",
   "Set a CRL list for proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-digest",
   "Use Digest authentication on the proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-header <header/@file>",
   "Pass custom header(s) to proxy",
   CURLHELP_PROXY},
  {"    --proxy-insecure",
   "Do HTTPS proxy connections without verifying the proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-key <key>",
   "Private key for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-key-type <type>",
   "Private key file type for proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-negotiate",
   "Use HTTP Negotiate (SPNEGO) authentication on the proxy",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --proxy-ntlm",
   "Use NTLM authentication on the proxy",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --proxy-pass <phrase>",
   "Pass phrase for the private key for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS | CURLHELP_AUTH},
  {"    --proxy-pinnedpubkey <hashes>",
   "FILE/HASHES public key to verify proxy with",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-service-name <name>",
   "SPNEGO proxy service name",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-ssl-allow-beast",
   "Allow security flaw for interop for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-ssl-auto-client-cert",
   "Use auto client certificate for proxy (Schannel)",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-tls13-ciphers <ciphersuite list>",
   "TLS 1.3 proxy cipher suites",
   CURLHELP_PROXY | CURLHELP_TLS},
  {"    --proxy-tlsauthtype <type>",
   "TLS authentication type for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS | CURLHELP_AUTH},
  {"    --proxy-tlspassword <string>",
   "TLS password for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS | CURLHELP_AUTH},
  {"    --proxy-tlsuser <name>",
   "TLS username for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS | CURLHELP_AUTH},
  {"    --proxy-tlsv1",
   "Use TLSv1 for HTTPS proxy",
   CURLHELP_PROXY | CURLHELP_TLS | CURLHELP_AUTH},
  {"-U, --proxy-user <user:password>",
   "Proxy user and password",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --proxy1.0 <host[:port]>",
   "Use HTTP/1.0 proxy on given port",
   CURLHELP_PROXY},
  {"-p, --proxytunnel",
   "Operate through an HTTP proxy tunnel (using CONNECT)",
   CURLHELP_PROXY},
  {"    --pubkey <key>",
   "SSH Public key file name",
   CURLHELP_SFTP | CURLHELP_SCP | CURLHELP_AUTH},
  {"-Q, --quote <command>",
   "Send command(s) to server before transfer",
   CURLHELP_FTP | CURLHELP_SFTP},
  {"    --random-file <file>",
   "File for reading random data from",
   CURLHELP_MISC},
  {"-r, --range <range>",
   "Retrieve only the bytes within RANGE",
   CURLHELP_HTTP | CURLHELP_FTP | CURLHELP_SFTP | CURLHELP_FILE},
  {"    --rate <max request rate>",
   "Request rate for serial transfers",
   CURLHELP_CONNECTION},
  {"    --raw",
   "Do HTTP \"raw\"; no transfer decoding",
   CURLHELP_HTTP},
  {"-e, --referer <URL>",
   "Referrer URL",
   CURLHELP_HTTP},
  {"-J, --remote-header-name",
   "Use the header-provided filename",
   CURLHELP_OUTPUT},
  {"-O, --remote-name",
   "Write output to a file named as the remote file",
   CURLHELP_IMPORTANT | CURLHELP_OUTPUT},
  {"    --remote-name-all",
   "Use the remote file name for all URLs",
   CURLHELP_OUTPUT},
  {"-R, --remote-time",
   "Set the remote file's time on the local output",
   CURLHELP_OUTPUT},
  {"    --remove-on-error",
   "Remove output file on errors",
   CURLHELP_CURL},
  {"-X, --request <method>",
   "Specify request method to use",
   CURLHELP_CONNECTION},
  {"    --request-target <path>",
   "Specify the target for this request",
   CURLHELP_HTTP},
  {"    --resolve <[+]host:port:addr[,addr]...>",
   "Resolve the host+port to this address",
   CURLHELP_CONNECTION | CURLHELP_DNS},
  {"    --retry <num>",
   "Retry request if transient problems occur",
   CURLHELP_CURL},
  {"    --retry-all-errors",
   "Retry all errors (use with --retry)",
   CURLHELP_CURL},
  {"    --retry-connrefused",
   "Retry on connection refused (use with --retry)",
   CURLHELP_CURL},
  {"    --retry-delay <seconds>",
   "Wait time between retries",
   CURLHELP_CURL},
  {"    --retry-max-time <seconds>",
   "Retry only within this period",
   CURLHELP_CURL},
  {"    --sasl-authzid <identity>",
   "Identity for SASL PLAIN authentication",
   CURLHELP_AUTH},
  {"    --sasl-ir",
   "Enable initial response in SASL authentication",
   CURLHELP_AUTH},
  {"    --service-name <name>",
   "SPNEGO service name",
   CURLHELP_MISC},
  {"-S, --show-error",
   "Show error even when -s is used",
   CURLHELP_CURL},
  {"-s, --silent",
   "Silent mode",
   CURLHELP_IMPORTANT | CURLHELP_VERBOSE},
  {"    --socks4 <host[:port]>",
   "SOCKS4 proxy on given host + port",
   CURLHELP_PROXY},
  {"    --socks4a <host[:port]>",
   "SOCKS4a proxy on given host + port",
   CURLHELP_PROXY},
  {"    --socks5 <host[:port]>",
   "SOCKS5 proxy on given host + port",
   CURLHELP_PROXY},
  {"    --socks5-basic",
   "Enable username/password auth for SOCKS5 proxies",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --socks5-gssapi",
   "Enable GSS-API auth for SOCKS5 proxies",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --socks5-gssapi-nec",
   "Compatibility with NEC SOCKS5 server",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --socks5-gssapi-service <name>",
   "SOCKS5 proxy service name for GSS-API",
   CURLHELP_PROXY | CURLHELP_AUTH},
  {"    --socks5-hostname <host[:port]>",
   "SOCKS5 proxy, pass host name to proxy",
   CURLHELP_PROXY},
  {"-Y, --speed-limit <speed>",
   "Stop transfers slower than this",
   CURLHELP_CONNECTION},
  {"-y, --speed-time <seconds>",
   "Trigger 'speed-limit' abort after this time",
   CURLHELP_CONNECTION},
  {"    --ssl",
   "Try SSL/TLS",
   CURLHELP_TLS},
  {"    --ssl-allow-beast",
   "Allow security flaw to improve interop",
   CURLHELP_TLS},
  {"    --ssl-auto-client-cert",
   "Use auto client certificate (Schannel)",
   CURLHELP_TLS},
  {"    --ssl-no-revoke",
   "Disable cert revocation checks (Schannel)",
   CURLHELP_TLS},
  {"    --ssl-reqd",
   "Require SSL/TLS",
   CURLHELP_TLS},
  {"    --ssl-revoke-best-effort",
   "Ignore missing/offline cert CRL dist points",
   CURLHELP_TLS},
  {"-2, --sslv2",
   "Use SSLv2",
   CURLHELP_TLS},
  {"-3, --sslv3",
   "Use SSLv3",
   CURLHELP_TLS},
  {"    --stderr <file>",
   "Where to redirect stderr",
   CURLHELP_VERBOSE},
  {"    --styled-output",
   "Enable styled output for HTTP headers",
   CURLHELP_VERBOSE},
  {"    --suppress-connect-headers",
   "Suppress proxy CONNECT response headers",
   CURLHELP_PROXY},
  {"    --tcp-fastopen",
   "Use TCP Fast Open",
   CURLHELP_CONNECTION},
  {"    --tcp-nodelay",
   "Use the TCP_NODELAY option",
   CURLHELP_CONNECTION},
  {"-t, --telnet-option <opt=val>",
   "Set telnet option",
   CURLHELP_TELNET},
  {"    --tftp-blksize <value>",
   "Set TFTP BLKSIZE option",
   CURLHELP_TFTP},
  {"    --tftp-no-options",
   "Do not send any TFTP options",
   CURLHELP_TFTP},
  {"-z, --time-cond <time>",
   "Transfer based on a time condition",
   CURLHELP_HTTP | CURLHELP_FTP},
  {"    --tls-max <VERSION>",
   "Set maximum allowed TLS version",
   CURLHELP_TLS},
  {"    --tls13-ciphers <ciphersuite list>",
   "TLS 1.3 cipher suites to use",
   CURLHELP_TLS},
  {"    --tlsauthtype <type>",
   "TLS authentication type",
   CURLHELP_TLS | CURLHELP_AUTH},
  {"    --tlspassword <string>",
   "TLS password",
   CURLHELP_TLS | CURLHELP_AUTH},
  {"    --tlsuser <name>",
   "TLS user name",
   CURLHELP_TLS | CURLHELP_AUTH},
  {"-1, --tlsv1",
   "Use TLSv1.0 or greater",
   CURLHELP_TLS},
  {"    --tlsv1.0",
   "Use TLSv1.0 or greater",
   CURLHELP_TLS},
  {"    --tlsv1.1",
   "Use TLSv1.1 or greater",
   CURLHELP_TLS},
  {"    --tlsv1.2",
   "Use TLSv1.2 or greater",
   CURLHELP_TLS},
  {"    --tlsv1.3",
   "Use TLSv1.3 or greater",
   CURLHELP_TLS},
  {"    --tr-encoding",
   "Request compressed transfer encoding",
   CURLHELP_HTTP},
  {"    --trace <file>",
   "Write a debug trace to FILE",
   CURLHELP_VERBOSE},
  {"    --trace-ascii <file>",
   "Like --trace, but without hex output",
   CURLHELP_VERBOSE},
  {"    --trace-time",
   "Add time stamps to trace/verbose output",
   CURLHELP_VERBOSE},
  {"    --unix-socket <path>",
   "Connect through this Unix domain socket",
   CURLHELP_CONNECTION},
  {"-T, --upload-file <file>",
   "Transfer local FILE to destination",
   CURLHELP_IMPORTANT | CURLHELP_UPLOAD},
  {"    --url <url>",
   "URL to work with",
   CURLHELP_CURL},
  {"-B, --use-ascii",
   "Use ASCII/text transfer",
   CURLHELP_MISC},
  {"-u, --user <user:password>",
   "Server user and password",
   CURLHELP_IMPORTANT | CURLHELP_AUTH},
  {"-A, --user-agent <name>",
   "Send User-Agent <name> to server",
   CURLHELP_IMPORTANT | CURLHELP_HTTP},
  {"-v, --verbose",
   "Make the operation more talkative",
   CURLHELP_IMPORTANT | CURLHELP_VERBOSE},
  {"-V, --version",
   "Show version number and quit",
   CURLHELP_IMPORTANT | CURLHELP_CURL},
  {"-w, --write-out <format>",
   "Use output FORMAT after completion",
   CURLHELP_VERBOSE},
  {"    --xattr",
   "Store metadata in extended file attributes",
   CURLHELP_MISC},
  { NULL, NULL, CURLHELP_HIDDEN }
};
