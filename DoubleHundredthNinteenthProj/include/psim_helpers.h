#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <SDL.h>

#define clean_errno()         (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...)     fprintf(stderr, "[ERROR] [%s:%d] [ERRNO: %s] [SDL: %s] [ASSERT: " M "]\n", __FILE__, __LINE__, clean_errno(), SDL_GetError(), ##__VA_ARGS__)
#define assertf(A, M, ...)    if(!(A)) { log_error(M, ##__VA_ARGS__); exit(EXIT_FAILURE); }

#ifdef DEBUG
#define DBG_CHECK(A, M, ...)  assertf(A, M, ##__VA_ARGS__)
#else
#define DBG_CHECK(A, M, ...)
#endif

#define RUN_CHECK(A, M, ...)  assertf(A, M, ##__VA_ARGS__)
#define UNREACHABLE()         RUN_CHECK(false, "unreachable code executed")
#define TODO()                RUN_CHECK(false, "not implemented")
