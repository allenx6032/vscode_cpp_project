/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "faur_out.v.h"
#include <faur.v.h>

#ifdef __GLIBC__
    #include <execinfo.h>
#endif

typedef enum {
    F_COLOR__BLACK = 30,
    F_COLOR__RED = 31,
    F_COLOR__GREEN = 32,
    F_COLOR__YELLOW = 33,
    F_COLOR__BLUE = 34,
    F_COLOR__MAGENTA = 35,
    F_COLOR__CYAN = 36,
    F_COLOR__WHITE = 37
} FColorCode;

static const char* g_sources[F_OUT__SOURCE_NUM] = {
    [F_OUT__SOURCE_FAUR] = "Faur",
    [F_OUT__SOURCE_APP] = "Game",
};

static const struct {
    const char* name;
    FColorCode color;
} g_types[F_OUT__TYPE_NUM] = {
    [F_OUT__TYPE_INFO] = {"Inf", F_COLOR__GREEN},
    [F_OUT__TYPE_WARNING] = {"Wrn", F_COLOR__YELLOW},
    [F_OUT__TYPE_ERROR] = {"Err", F_COLOR__RED},
    [F_OUT__TYPE_STATE] = {"Stt", F_COLOR__BLUE},
    [F_OUT__TYPE_FATAL] = {"Ftl", F_COLOR__RED},
};

static void outWorkerPrint(FOutSource Source, FOutType Type, FILE* Stream, const char* Text)
{
    char headerTag[64];

    if(f_str_fmt(headerTag,
                 sizeof(headerTag),
                 false,
#if F_CONFIG_OUT_COLOR_TEXT
                 "\033[1;%dm[%s][%s][%08x]\033[0m ",
                 g_types[Type].color,
#else
                 "[%s][%s][%08x] ",
#endif
                 g_sources[Source],
                 g_types[Type].name,
                 (unsigned)f_fps_ticksGet())) {

        f_platform_api__filePrint(Stream, headerTag);
    }

    f_platform_api__filePrint(Stream, Text);
    f_platform_api__filePrint(Stream, "\n");
}

static void outWorker(FOutSource Source, FOutType Type, FILE* Stream, const char* Format, va_list Args)
{
    static char buffer[512];

    if(f_str_fmtv(buffer, sizeof(buffer), true, Format, Args)) {
        outWorkerPrint(Source, Type, Stream, buffer);

        #if F_CONFIG_TRAIT_CONSOLE
            f_console__write(Source, Type, buffer);
        #endif
    }
}

void f_out__info(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_FAUR,
              F_OUT__TYPE_INFO,
              F_CONFIG_OUT_STDOUT,
              Format,
              args);

    va_end(args);
}

void f_out__warning(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_FAUR,
              F_OUT__TYPE_WARNING,
              F_CONFIG_OUT_STDERR,
              Format,
              args);

    va_end(args);
}

void f_out__error(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_FAUR,
              F_OUT__TYPE_ERROR,
              F_CONFIG_OUT_STDERR,
              Format,
              args);

    va_end(args);

    #if F_CONFIG_DEBUG
        f_out__backtrace(F_OUT__SOURCE_FAUR);
    #endif
}

void f_out__errorv(const char* Format, va_list Args)
{
    outWorker(F_OUT__SOURCE_FAUR,
              F_OUT__TYPE_ERROR,
              F_CONFIG_OUT_STDERR,
              Format,
              Args);

    #if F_CONFIG_DEBUG
        f_out__backtrace(F_OUT__SOURCE_FAUR);
    #endif
}

void f_out__state(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_FAUR,
              F_OUT__TYPE_STATE,
              F_CONFIG_OUT_STDOUT,
              Format,
              args);

    va_end(args);
}

void f_out_info(const char* Format, ...)
{
    F__CHECK(Format != NULL);

    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_APP,
              F_OUT__TYPE_INFO,
              F_CONFIG_OUT_STDOUT,
              Format,
              args);

    va_end(args);
}

void f_out_warning(const char* Format, ...)
{
    F__CHECK(Format != NULL);

    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_APP,
              F_OUT__TYPE_WARNING,
              F_CONFIG_OUT_STDERR,
              Format,
              args);

    va_end(args);
}

void f_out_error(const char* Format, ...)
{
    F__CHECK(Format != NULL);

    va_list args;
    va_start(args, Format);

    outWorker(F_OUT__SOURCE_APP,
              F_OUT__TYPE_ERROR,
              F_CONFIG_OUT_STDERR,
              Format,
              args);

    va_end(args);

    #if F_CONFIG_DEBUG
        f_out__backtrace(F_OUT__SOURCE_APP);
    #endif
}

void f_out_errorv(const char* Format, va_list Args)
{
    F__CHECK(Format != NULL);

    outWorker(F_OUT__SOURCE_APP,
              F_OUT__TYPE_ERROR,
              F_CONFIG_OUT_STDERR,
              Format,
              Args);

    #if F_CONFIG_DEBUG
        f_out__backtrace(F_OUT__SOURCE_APP);
    #endif
}

void f_out__backtrace(FOutSource Source)
{
    #ifdef __GLIBC__
        void* addresses[16];
        int numAddresses = backtrace(addresses, F_ARRAY_LEN(addresses));
        char** functionNames = backtrace_symbols(addresses, numAddresses);

        for(int i = 0; i < numAddresses; i++) {
            outWorkerPrint(Source,
                           F_OUT__TYPE_INFO,
                           F_CONFIG_OUT_STDOUT,
                           functionNames[i]);
        }

        free(functionNames);
    #else
        F_UNUSED(Source);
    #endif
}
