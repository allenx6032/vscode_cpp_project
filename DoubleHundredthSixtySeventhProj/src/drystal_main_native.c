/**
 * This file is part of Drystal.
 *
 * Drystal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Drystal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Drystal.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include <errno.h>
#include <stdio.h>
#ifdef BUILD_LIVECODING
#include <assert.h>
#include <libgen.h>// dirname()
#include <stdlib.h>

#include "drystal_livecoding.h"
#include "drystal_macro.h"
#endif

#include "drystal_engine.h"
#include "drystal_dlua.h"
#include "drystal_log.h"
#include "drystal_util.h"
#include "drystal_build.h"

log_category("main");

#ifdef BUILD_LIVECODING
static void reload(_unused_ void *arg, const char* filename)
{
	engine_add_file_to_reloadqueue(filename);
}

static int start_livecoding(const char *filename)
{
	char *filename_dup;
	char *watched_directory;
	int r;

	assert(filename);

	filename_dup = xstrdup(filename);

	watched_directory = dirname(filename_dup);
	r = livecoding_init(reload, NULL);
	if (r < 0) {
		log_error("Cannot initialize livecoding: %s", strerror(-r));
		free(filename_dup);
		return r;
	}

	r = livecoding_watch_directory_recursively(watched_directory);
	if (r < 0) {
		log_error("Cannot watch the directory '%s' for livecoding: %s", watched_directory, strerror(-r));
		free(filename_dup);
		livecoding_quit();
		return r;
	}

	if (!files_are_same(watched_directory, ".")) {
		r = livecoding_watch_directory_recursively(".");
		if (r < 0) {
			log_error("Cannot watch the directory '.' for livecoding: %s", strerror(-r));
			livecoding_quit();
			return r;
		}
	}

	free(filename_dup);

	return 0;
}
#endif

static void help(void)
{
	printf("drystal [OPTIONS] <directory>[/main.lua]\n\n"
	       "OPTIONS\n"
	       "    -h --help       Show this help message and exit\n"
	       "    -v --version    Show Drystal version and available features\n"
#ifdef BUILD_LIVECODING
	       "    -l --livecoding Enable the livecoding which will reload the lua code when modifications on the files are performed\n"
#endif
	      );
}

static void print_version(void)
{
	puts(DRYSTAL_VERSION);
	puts(DRYSTAL_FEATURES);
}

int main(int argc, char* argv[])
{
	const char* default_filename = "main.lua";
	char* filename = NULL;
	int r;
#ifdef BUILD_LIVECODING
	bool livecoding = false;
#endif
	bool is_arg[argc];

	for (int i = 1; i < argc; i++) {
		is_arg[i] = false;
		if (streq(argv[i], "--help") || streq(argv[i], "-h")) {
			help();
			return 0;
		} else if (streq(argv[i], "--version") || streq(argv[i], "-v")) {
			print_version();
			return 0;
		} else if (streq(argv[i], "--livecoding") || streq(argv[i], "-l")) {
#ifdef BUILD_LIVECODING
			livecoding = true;
#else
			fprintf(stderr, "Cannot start livecoding: disabled at compilation time.\n");
			return EXIT_FAILURE;
#endif
		} else if (!filename) {
			filename = xstrdup(argv[i]);
		} else {
			is_arg[i] = true;
		}
	}

	if (!filename) {
		filename = xstrdup(default_filename);
	} else if (is_directory(filename)) {
		char* newfilename;
		if (endswith(filename, "/")) {
			newfilename = strjoin(filename, default_filename, NULL);
		} else {
			newfilename = strjoin(filename, "/", default_filename, NULL);
		}
		free(filename);
		filename = newfilename;
	}

	r = engine_init(filename, 60);
	if (r < 0) {
		return EXIT_FAILURE;
	}

	for (int i = 1; i < argc; i++) {
		if (is_arg[i]) {
			dlua_add_arg(argv[i]);
		}
	}

#ifdef BUILD_LIVECODING
	if (livecoding) {
		int r = start_livecoding(filename);
		if (r < 0) {
			return EXIT_FAILURE;
		}
	}
#endif

	engine_load();
	engine_loop();

#ifdef BUILD_LIVECODING
	livecoding_quit();
#endif

	engine_free();
	free(filename);
	return 0;
}

