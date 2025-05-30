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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

#include "drystal_util.h"
#include "drystal_macro.h"
#include "drystal_log.h"

bool stderr_use_colors(void)
{
#ifdef EMSCRIPTEN
	return false;
#else
	return on_tty(STDERR_FILENO);
#endif
}

bool on_tty(int fd)
{
	static int cached_on_tty[256];

	if (fd >= 256)
		return isatty(fd) > 0;

	if (cached_on_tty[fd] == 0)
		cached_on_tty[fd] = isatty(fd) > 0 ? 2 : 1;

	return cached_on_tty[fd] == 2;
}

char *xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (!p) {
		log_oom_and_exit();
	}

	return p;
}

void *xcalloc(size_t nmemb, size_t size)
{
	void *p;

	p = calloc(nmemb, size);
	if (!p) {
		log_oom_and_exit();
	}

	return p;
}

void *xmalloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (!p) {
		log_oom_and_exit();
	}

	return p;
}

int mkdir_p(const char *path)
{
	char *p;
	char t[strlen(path) + 1];

	assert(path);

	strcpy(t, path);
	for (p = strchr(t, '/'); p && *p; p = strchr(p + 1, '/')) {
		int r;

		*p = 0;

#ifdef _WIN32
		r = mkdir(t);
#else
		r = mkdir(t, 0777);
#endif
		if (r < 0 && errno != EEXIST)
			return -errno;

		*p = '/';
	}

	return 0;
}

char *strjoin(const char *s, ...)
{
	va_list ap;
	size_t l = 0;
	char *join, *p;

	va_start(ap, s);
	l = strlen(s);
	for (;;) {
		const char *t;

		t = va_arg(ap, const char *);
		if (!t) {
			break;
		}

		l += strlen(t);
	}
	va_end(ap);

	join = new(char, l + 1);

	va_start(ap, s);
	#ifdef _WIN32
	p = strcpy(join, s);
	#else
	p = stpcpy(join, s);
	#endif
	for (;;) {
		const char *t;

		t = va_arg(ap, const char *);
		if (!t) {
			break;
		}
		#ifdef _WIN32
		p = strcpy(p, t);
		#else
		p = stpcpy(p, t);
		#endif
	}
	va_end(ap);

	return join;
}

void msleep(unsigned long milisec)
{
	struct timespec ts;
	ts.tv_sec = milisec / MSEC_PER_SEC;
	ts.tv_nsec = (milisec % MSEC_PER_SEC) * USEC_PER_SEC;

	/* we sleep until the time requested has passed */
	while (nanosleep(&ts, &ts) == -1);
}

bool endswith(const char *s, const char *postfix)
{
	size_t s_length;
	size_t p_length;

	assert(s);
	assert(postfix);

	s_length = strlen(s);
	p_length = strlen(postfix);

	if (p_length == 0)
		return true;

	if (s_length < p_length)
		return false;

	if (memcmp(s + s_length - p_length, postfix, p_length) != 0)
		return false;

	return true;
}

bool is_directory(const char *directory)
{
	struct stat s;

	assert(directory);

	if (stat(directory, &s) < 0) {
		return false;
	}

	if (S_ISDIR(s.st_mode)) {
		return true;
	}

	return false;
}

bool files_are_same(const char *filea, const char *fileb)
{
	struct stat a, b;

	if (!strcmp(filea, fileb))
		return true;

	if (stat(filea, &a) < 0)
		return false;

	if (stat(fileb, &b) < 0)
		return false;

	return a.st_dev == b.st_dev &&
		a.st_ino == b.st_ino;
}

void *xrealloc(void **p, size_t *nmemb, size_t need, size_t size, unsigned min_nmemb)
{
	size_t new_totalsize;
	size_t new_nmemb;
	void *q;

	assert(p);
	assert(nmemb);
	assert(size > 0);

	if (*nmemb >= need)
		return *p;

	new_nmemb = MAX(need * 2, min_nmemb);
	new_totalsize = new_nmemb * size;

	if (new_totalsize < size * need)
		log_oom_and_exit();

	q = realloc(*p, new_totalsize);
	if (!q) {
		log_oom_and_exit();
	}

	*p = q;
	*nmemb = new_nmemb;
	return q;
}

