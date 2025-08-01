/*
 * filesystem.c -- filesystem functions
 *
 * Copyright (c) 2013, 2014 David Demelier <markand@malikania.fr>
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

#include "luasdl_filesystem.h"

/*
 * SDL.getBasePath()
 *
 * Returns:
 *	The base directory where the application runs or nil on failure
 *	The error message
 */
static int
l_getBasePath(lua_State *L)
{
	char *str = SDL_GetBasePath();

	if (str == NULL)
		return commonPushSDLError(L, 1);

	lua_pushstring(L, str);
	SDL_free(str);

	return 1;
}

/*
 * SDL.getPrefPath(organization, application)
 *
 * Arguments:
 *	organization the organization name
 *	application the application name
 *
 * Returns:
 *	The directory or nil on failure
 *	The error message
 */
static int
l_getPrefPath(lua_State *L)
{
	const char *organization = luaL_checkstring(L, 1);
	const char *application = luaL_checkstring(L, 2);
	char *str = SDL_GetPrefPath(organization, application);

	if (str == NULL)
		return commonPushSDLError(L, 1);

	lua_pushstring(L, str);
	SDL_free(str);

	return 1;
}

const luaL_Reg FilesystemFunctions[] = {
	{ "getBasePath",	l_getBasePath	},
	{ "getPrefPath",	l_getPrefPath	},
	{ NULL,			NULL		}
};
