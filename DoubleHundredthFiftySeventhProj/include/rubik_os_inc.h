#ifndef OS_INC_H
#define OS_INC_H

#if !defined(OS_FEATURE_GRAPHICAL)
# define OS_FEATURE_GRAPHICAL 0
#endif

#if !defined(OS_GFX_STUB)
# define OS_GFX_STUB 0
#endif

#include "rubik_os_core.h"
#if OS_FEATURE_GRAPHICAL
#if OS_WINDOWS
#  define VK_USE_PLATFORM_WIN32_KHR
#  define VK_PROTOTYPES
#elif OS_LINUX
#  define VK_USE_PLATFORM_XLIB_KHR
#  define VK_PROTOTYPES
#endif
#  include <vulkan/vulkan.h>
#  include "rubik_os_gfx.h"
#endif

#if OS_WINDOWS
# include "rubik_os_core_win32.h"
#elif OS_LINUX
# include "rubik_os_core_linux.h"
#else
# error OS core layer not implemented for this operating system.
#endif

#if OS_FEATURE_GRAPHICAL
# if OS_GFX_STUB
#  include "rubik_os_gfx_stub.h"
# elif OS_WINDOWS
#  include "rubik_os_gfx_win32.h"
# elif OS_LINUX
#  include "rubik_os_gfx_linux.h"
# else
#  error OS graphical layer not implemented for this operating system.
# endif
#endif

#endif // OS_INC_H
