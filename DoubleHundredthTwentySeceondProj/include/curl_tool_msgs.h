#ifndef HEADER_CURL_TOOL_MSGS_H
#define HEADER_CURL_TOOL_MSGS_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2019, Daniel Stenberg, <daniel@haxx.se>, et al.
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
#include "curl_tool_setup.h"

void warnf(struct GlobalConfig *config, const char *fmt, ...);
void notef(struct GlobalConfig *config, const char *fmt, ...);
void helpf(FILE *errors, const char *fmt, ...);
void errorf(struct GlobalConfig *config, const char *fmt, ...);

#endif /* HEADER_CURL_TOOL_MSGS_H */
