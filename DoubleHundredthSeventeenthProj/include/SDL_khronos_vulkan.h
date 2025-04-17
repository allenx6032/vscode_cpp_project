#ifndef VULKAN_H_
#define VULKAN_H_ 1

/*
** Copyright (c) 2015-2020 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/

#include "SDL_khronos_vk_platform.h"
#include "SDL_khronos_vulkan_core.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include "SDL_khronos_vulkan_android.h"
#endif

#ifdef VK_USE_PLATFORM_FUCHSIA
#include <zircon/types.h>
#include "SDL_khronos_vulkan_fuchsia.h"
#endif

#ifdef VK_USE_PLATFORM_IOS_MVK
#include "SDL_khronos_vulkan_ios.h"
#endif


#ifdef VK_USE_PLATFORM_MACOS_MVK
#include "SDL_khronos_vulkan_macos.h"
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
#include "SDL_khronos_vulkan_metal.h"
#endif

#ifdef VK_USE_PLATFORM_VI_NN
#include "SDL_khronos_vulkan_vi.h"
#endif


#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#include <wayland-client.h>
#include "SDL_khronos_vulkan_wayland.h"
#endif


#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include "SDL_khronos_vulkan_win32.h"
#endif


#ifdef VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>
#include "SDL_khronos_vulkan_xcb.h"
#endif


#ifdef VK_USE_PLATFORM_XLIB_KHR
#include <X11/Xlib.h>
#include "SDL_khronos_vulkan_xlib.h"
#endif


#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
#include <directfb.h>
#include "SDL_khronos_vulkan_directfb.h"
#endif


#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "SDL_khronos_vulkan_xlib_xrandr.h"
#endif


#ifdef VK_USE_PLATFORM_GGP
#include <ggp_c/vulkan_types.h>
#include "SDL_khronos_vulkan_ggp.h"
#endif


#ifdef VK_ENABLE_BETA_EXTENSIONS
#include "SDL_khronos_vulkan_beta.h"
#endif

#endif // VULKAN_H_
