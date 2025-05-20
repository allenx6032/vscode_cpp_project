// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#include "rubik_metagen_os_core.c.h"

#if OS_WINDOWS
# include "rubik_metagen_os_core_win32.c.h"
#elif OS_LINUX
# include "rubik_metagen_os_core_linux.c.h"
#else
# error OS core layer not implemented for this operating system.
#endif
