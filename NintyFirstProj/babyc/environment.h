#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef BABYC_ENV_HEADER
#define BABYC_ENV_HEADER

typedef struct VarWithOffset {
    char *var_name;
    int offset;
} VarWithOffset;

typedef struct Environment {
    size_t size;
    VarWithOffset *items;
} Environment;

Environment *environment_new();

void environment_set_offset(Environment *env, char *var_name, int offset);

int environment_get_offset(Environment *env, char *var_name);

void environment_free(Environment *env);


static void warnx(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}
#endif
