#include "rubik_os_core.c.h"
#if OS_FEATURE_GRAPHICAL
# include "rubik_os_gfx.c.h"
#endif

#if OS_WINDOWS
# include "rubik_os_core_win32.c.h"
#elif OS_LINUX
# include "rubik_os_core_linux.c.h"
#else
# error OS core layer not implemented for this operating system.
#endif

#if OS_FEATURE_GRAPHICAL
# if OS_GFX_STUB
#  include "rubik_os_gfx_stub.c.h"
# elif OS_WINDOWS
#  include "rubik_os_gfx_win32.c.h"
# elif OS_LINUX
#  include "rubik_os_gfx_linux.c.h"
# else
#  error OS graphical layer not implemented for this operating system.
# endif
#endif
