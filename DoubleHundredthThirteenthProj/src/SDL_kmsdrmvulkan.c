/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
 * @author Manuel Alfayate Corchere <redwindwanderer@gmail.com>.
 * Based on Jacob Lifshay's SDL_x11vulkan.c.
 */

#include "SDL_internal.h"

#if SDL_VIDEO_VULKAN && SDL_VIDEO_DRIVER_KMSDRM

#include "SDL_kmsdrmvideo.h"
#include "SDL_kmsdrmdyn.h"
#include "SDL_assert.h"

#include "SDL_loadso.h"
#include "SDL_kmsdrmvulkan.h"
#include "SDL_syswm.h"
#include "sys/ioctl.h"

#if defined(__OpenBSD__)
#define DEFAULT_VULKAN  "libvulkan.so"
#else
#define DEFAULT_VULKAN  "libvulkan.so.1"
#endif

int KMSDRM_Vulkan_LoadLibrary(_THIS, const char *path)
{
    VkExtensionProperties *extensions = NULL;
    Uint32 i, extensionCount = 0;
    SDL_bool hasSurfaceExtension = SDL_FALSE;
    SDL_bool hasDisplayExtension = SDL_FALSE;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = NULL;

    if(_this->vulkan_config.loader_handle)
        return SDL_SetError("Vulkan already loaded");

    /* Load the Vulkan library */
    if(!path)
        path = SDL_getenv("SDL_VULKAN_LIBRARY");
    if(!path)
        path = DEFAULT_VULKAN;

    _this->vulkan_config.loader_handle = SDL_LoadObject(path);

    if(!_this->vulkan_config.loader_handle)
        return -1;

    SDL_strlcpy(_this->vulkan_config.loader_path, path,
                SDL_arraysize(_this->vulkan_config.loader_path));

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)SDL_LoadFunction(
        _this->vulkan_config.loader_handle, "vkGetInstanceProcAddr");

    if(!vkGetInstanceProcAddr)
        goto fail;

    _this->vulkan_config.vkGetInstanceProcAddr = (void *)vkGetInstanceProcAddr;
    _this->vulkan_config.vkEnumerateInstanceExtensionProperties =
        (void *)((PFN_vkGetInstanceProcAddr)_this->vulkan_config.vkGetInstanceProcAddr)(
            VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties");

    if(!_this->vulkan_config.vkEnumerateInstanceExtensionProperties)
        goto fail;

    extensions = SDL_Vulkan_CreateInstanceExtensionsList(
        (PFN_vkEnumerateInstanceExtensionProperties)
            _this->vulkan_config.vkEnumerateInstanceExtensionProperties,
        &extensionCount);

    if(!extensions)
        goto fail;

    for(i = 0; i < extensionCount; i++)
    {
        if(SDL_strcmp(VK_KHR_SURFACE_EXTENSION_NAME, extensions[i].extensionName) == 0)
            hasSurfaceExtension = SDL_TRUE;
        else if(SDL_strcmp(VK_KHR_DISPLAY_EXTENSION_NAME, extensions[i].extensionName) == 0)
            hasDisplayExtension = SDL_TRUE;
    }

    SDL_free(extensions);

    if(!hasSurfaceExtension)
    {
        SDL_SetError("Installed Vulkan doesn't implement the "
                     VK_KHR_SURFACE_EXTENSION_NAME " extension");
        goto fail;
    }
    else if(!hasDisplayExtension)
    {
        SDL_SetError("Installed Vulkan doesn't implement the "
                     VK_KHR_DISPLAY_EXTENSION_NAME "extension");
        goto fail;
    }

    return 0;

fail:
    SDL_UnloadObject(_this->vulkan_config.loader_handle);
    _this->vulkan_config.loader_handle = NULL;
    return -1;
}

void KMSDRM_Vulkan_UnloadLibrary(_THIS)
{
    if(_this->vulkan_config.loader_handle)
    {
        SDL_UnloadObject(_this->vulkan_config.loader_handle);
        _this->vulkan_config.loader_handle = NULL;
    }
}

/*********************************************************************/
/* Here we can put whatever Vulkan extensions we want to be enabled  */
/* at instance creation, which is done in the programs, not in SDL.  */
/* So: programs call SDL_Vulkan_GetInstanceExtensions() and here     */
/* we put the extensions specific to this backend so the programs    */
/* get a list with the extension we want, so they can include that   */
/* list in the ppEnabledExtensionNames and EnabledExtensionCount     */
/* members of the VkInstanceCreateInfo struct passed to              */
/* vkCreateInstance().                                               */
/*********************************************************************/
SDL_bool KMSDRM_Vulkan_GetInstanceExtensions(_THIS,
                                          SDL_Window *window,
                                          unsigned *count,
                                          const char **names)
{
    static const char *const extensionsForKMSDRM[] = {
        VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_DISPLAY_EXTENSION_NAME
    };
    if(!_this->vulkan_config.loader_handle)
    {
        SDL_SetError("Vulkan is not loaded");
        return SDL_FALSE;
    }
    return SDL_Vulkan_GetInstanceExtensions_Helper(
            count, names, SDL_arraysize(extensionsForKMSDRM),
            extensionsForKMSDRM);
}

void KMSDRM_Vulkan_GetDrawableSize(_THIS, SDL_Window *window, int *w, int *h)
{
    if (w) {
        *w = window->w;
    }

    if (h) {
        *h = window->h;
    }
}

/***********************************************************************/
/* First thing to know is that we don't call vkCreateInstance() here.  */
/* Instead, programs using SDL and Vulkan create their Vulkan instance */
/* and we get it here, ready to use.                                   */
/* Extensions specific for this platform are activated in              */
/* KMSDRM_Vulkan_GetInstanceExtensions(), like we do with              */
/* VK_KHR_DISPLAY_EXTENSION_NAME, which is what we need for x-less VK. */                
/***********************************************************************/
SDL_bool KMSDRM_Vulkan_CreateSurface(_THIS,
                                  SDL_Window *window,
                                  VkInstance instance,
                                  VkSurfaceKHR *surface)
{
    VkPhysicalDevice gpu;
    uint32_t gpu_count;
    uint32_t display_count;
    uint32_t mode_count;
    uint32_t plane_count;

    VkPhysicalDevice *physical_devices = NULL;
    VkDisplayPropertiesKHR *displays_props = NULL;
    VkDisplayModePropertiesKHR *modes_props = NULL;
    VkDisplayPlanePropertiesKHR *planes_props = NULL;

    VkDisplayModeCreateInfoKHR display_mode_create_info;
    VkDisplaySurfaceCreateInfoKHR display_plane_surface_create_info;

    VkExtent2D image_size;
    VkDisplayModeKHR display_mode;
    VkDisplayModePropertiesKHR display_mode_props = {0};

    VkResult result;
    SDL_bool ret = SDL_FALSE;

    /* We don't receive a display index in KMSDRM_CreateDevice(), only
       a device index, which determines the GPU to use, but not the output.
       So we simply use the first connected output (ie, the first connected
       video output) for now.
       In other words, change this index to select a different output. Easy! */
    int display_index = 0;

    int i;

    SDL_VideoData *viddata = ((SDL_VideoData *)_this->driverdata);

    /* Get the function pointers for the functions we will use. */
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        (PFN_vkGetInstanceProcAddr)_this->vulkan_config.vkGetInstanceProcAddr;

    PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR =
        (PFN_vkCreateDisplayPlaneSurfaceKHR)vkGetInstanceProcAddr(
            instance, "vkCreateDisplayPlaneSurfaceKHR");

    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices =
        (PFN_vkEnumeratePhysicalDevices)vkGetInstanceProcAddr(
            instance, "vkEnumeratePhysicalDevices");

    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR =
        (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");

    PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR =
        (PFN_vkGetDisplayModePropertiesKHR)vkGetInstanceProcAddr(
            instance, "vkGetDisplayModePropertiesKHR");

    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR =
        (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");

    /*PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR =
        (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)vkGetInstanceProcAddr(
            instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
    
    PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR =
        (PFN_vkGetDisplayPlaneCapabilitiesKHR)vkGetInstanceProcAddr(
            instance, "vkGetDisplayPlaneCapabilitiesKHR");
    */

    PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR =
        (PFN_vkCreateDisplayModeKHR)vkGetInstanceProcAddr(
            instance, "vkCreateDisplayModeKHR");

    if(!_this->vulkan_config.loader_handle)
    {
        SDL_SetError("Vulkan is not loaded");
        goto clean;
    }

    /*************************************/
    /* Block for vulkan surface creation */
    /*************************************/

    /****************************************************************/
    /* If we got vkCreateDisplayPlaneSurfaceKHR() pointer, it means */
    /* that the VK_KHR_Display extension is active on the instance. */
    /* That's the central extension we need for x-less VK!          */
    /****************************************************************/
    if(!vkCreateDisplayPlaneSurfaceKHR)
    {
        SDL_SetError(VK_KHR_DISPLAY_EXTENSION_NAME
                     " extension is not enabled in the Vulkan instance.");
        goto clean;
    }

    /* Get the physical device count. */
    vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);

    if (gpu_count == 0) {
        SDL_SetError("Vulkan can't find physical devices (gpus).");
        goto clean;
    }

    /* Get the physical devices. */
    physical_devices = malloc(sizeof(VkPhysicalDevice) * gpu_count);
    vkEnumeratePhysicalDevices(instance, &gpu_count, physical_devices);

    /* A GPU (or physical_device, in vkcube terms) is a GPU. A machine with more
       than one video output doen't need to have more than one GPU, like the Pi4
       which has 1 GPU and 2 video outputs.
       We grab the GPU/physical_device with the index we got in KMSDR_CreateDevice(). */
    gpu = physical_devices[viddata->devindex]; 

    /* A display is a video output. 1 GPU can have N displays.
       Vulkan only counts the connected displays.
       Get the display count of the GPU. */
    vkGetPhysicalDeviceDisplayPropertiesKHR(gpu, &display_count, NULL);
    if (display_count == 0) {
        SDL_SetError("Vulkan can't find any displays.");
        goto clean;
    }

    /* Get the props of the displays of the physical device. */
    displays_props = (VkDisplayPropertiesKHR *) malloc(display_count * sizeof(*displays_props));
    vkGetPhysicalDeviceDisplayPropertiesKHR(gpu,
                                           &display_count,
                                           displays_props);

    /* Get the videomode count for the first display. */
    vkGetDisplayModePropertiesKHR(gpu,
                                 displays_props[display_index].display,
                                 &mode_count, NULL);

    if (mode_count == 0) {
        SDL_SetError("Vulkan can't find any video modes for display %i (%s)\n", 0,
                               displays_props[display_index].displayName);
        goto clean;
    }

    /* Get the props of the videomodes for the first display. */
    modes_props = (VkDisplayModePropertiesKHR *) malloc(mode_count * sizeof(*modes_props));
    vkGetDisplayModePropertiesKHR(gpu,
                                 displays_props[display_index].display,
                                 &mode_count, modes_props);

    /* Get the planes count of the physical device. */
    vkGetPhysicalDeviceDisplayPlanePropertiesKHR(gpu, &plane_count, NULL);
    if (plane_count == 0) {
        SDL_SetError("Vulkan can't find any planes.");
        goto clean;
    }

    /* Get the props of the planes for the physical device. */
    planes_props = malloc(sizeof(VkDisplayPlanePropertiesKHR) * plane_count);
    vkGetPhysicalDeviceDisplayPlanePropertiesKHR(gpu, &plane_count, planes_props);

    /* Get a video mode equal or smaller than the window size. REMEMBER:
       We have to get a small enough videomode for the window size,
       because videomode determines how big the scanout region is and we can't
       scanout a region bigger than the window (we would be reading past the
       buffer, and Vulkan would give us a confusing VK_ERROR_SURFACE_LOST_KHR). */
    for (i = 0; i < mode_count; i++) {
        if (modes_props[i].parameters.visibleRegion.width <= window->w &&
            modes_props[i].parameters.visibleRegion.height <= window->h)
        {
            display_mode_props = modes_props[i];
            break;
        }
    }

    if (display_mode_props.parameters.visibleRegion.width == 0
        || display_mode_props.parameters.visibleRegion.height == 0)
    {
        SDL_SetError("Vulkan can't find a proper display mode for the window size.");
        goto clean;
    }

    /* We have the props of the display mode, but we need an actual display mode. */
    display_mode_create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR;
    display_mode_create_info.parameters = display_mode_props.parameters;
    result = vkCreateDisplayModeKHR(gpu,
			      displays_props[display_index].display,
			      &display_mode_create_info,
			      NULL, &display_mode);
    if (result != VK_SUCCESS) {
        SDL_SetError("Vulkan can't create the display mode.");
        goto clean;
    }

    /* Let's finally create the Vulkan surface! */

    image_size.width = window->w;
    image_size.height = window->h;
    
    display_plane_surface_create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
    display_plane_surface_create_info.displayMode = display_mode;
    /* For now, simply use the first plane. */
    display_plane_surface_create_info.planeIndex = 0;
    display_plane_surface_create_info.imageExtent = image_size;
    result = vkCreateDisplayPlaneSurfaceKHR(instance,
                                     &display_plane_surface_create_info,
                                     NULL,
                                     surface);
    if(result != VK_SUCCESS)
    {
        SDL_SetError("vkCreateKMSDRMSurfaceKHR failed: %s",
            SDL_Vulkan_GetResultString(result));
        goto clean;
    }

    ret = SDL_TRUE;

clean:
    if (physical_devices)
        free (physical_devices);
    if (displays_props)
        free (displays_props);
    if (planes_props)
        free (planes_props);
    if (modes_props)
        free (modes_props);

    return ret;
}

#endif

/* vim: set ts=4 sw=4 expandtab: */
