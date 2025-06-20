/*
 * window.h -- window management
 *
 * Copyright (c) 2013, 2014 David Demelier <markand@malikania.fr>
 * Copyright (c) 2016 Webster Sheets <webster@web-eworks.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <luasdl_common.h>

#define WindowName	Window.name

extern const luaL_Reg WindowFunctions[];

extern const CommonObject Window;

extern const CommonEnum WindowFlags[];

#if SDL_VERSION_ATLEAST(2, 0, 4)

extern const CommonEnum HitTestResults[];

#endif

#endif /* !_WINDOW_H_ */
