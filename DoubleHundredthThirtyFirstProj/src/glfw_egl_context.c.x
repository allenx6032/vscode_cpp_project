//========================================================================
// GLFW 3.0 EGL - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "glfw_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// Thread local storage attribute macro
//
#if defined(_MSC_VER)
 #define _GLFW_TLS __declspec(thread)
#elif defined(__GNUC__)
 #define _GLFW_TLS __thread
#else
 #define _GLFW_TLS
#endif


// The per-thread current context/window pointer
//
static _GLFW_TLS _GLFWwindow* _glfwCurrentWindow = NULL;


// Return a description of the specified EGL error
//
static const char* getErrorString(EGLint error)
{
    switch (error)
    {
        case EGL_SUCCESS:
            return "Success";
        case EGL_NOT_INITIALIZED:
            return "EGL is not or could not be initialized";
        case EGL_BAD_ACCESS:
            return "EGL cannot access a requested resource";
        case EGL_BAD_ALLOC:
            return "EGL failed to allocate resources for the requested operation";
        case EGL_BAD_ATTRIBUTE:
            return "An unrecognized attribute or attribute value was passed "
                   "in the attribute list";
        case EGL_BAD_CONTEXT:
            return "An EGLContext argument does not name a valid EGL "
                   "rendering context";
        case EGL_BAD_CONFIG:
            return "An EGLConfig argument does not name a valid EGL frame "
                   "buffer configuration";
        case EGL_BAD_CURRENT_SURFACE:
            return "The current surface of the calling thread is a window, pixel "
                   "buffer or pixmap that is no longer valid";
        case EGL_BAD_DISPLAY:
            return "An EGLDisplay argument does not name a valid EGL display "
                   "connection";
        case EGL_BAD_SURFACE:
            return "An EGLSurface argument does not name a valid surface "
                   "configured for GL rendering";
        case EGL_BAD_MATCH:
            return "Arguments are inconsistent";
        case EGL_BAD_PARAMETER:
            return "One or more argument values are invalid";
        case EGL_BAD_NATIVE_PIXMAP:
            return "A NativePixmapType argument does not refer to a valid "
                   "native pixmap";
        case EGL_BAD_NATIVE_WINDOW:
            return "A NativeWindowType argument does not refer to a valid "
                   "native window";
        case EGL_CONTEXT_LOST:
            return "The application must destroy all contexts and reinitialise";
    }

    return "UNKNOWN EGL ERROR";
}

// Returns the specified attribute of the specified EGLConfig
//
static int getConfigAttrib(EGLConfig config, int attrib)
{
    int value;
    eglGetConfigAttrib(_glfw.egl.display, config, attrib, &value);
    return value;
}

// Return a list of available and usable framebuffer configs
//
static GLboolean chooseFBConfigs(const _GLFWwndconfig* wndconfig,
                                 const _GLFWfbconfig* desired,
                                 EGLConfig* result)
{
    EGLConfig* nativeConfigs;
    _GLFWfbconfig* usableConfigs;
    const _GLFWfbconfig* closest;
    int i, nativeCount, usableCount;

    eglGetConfigs(_glfw.egl.display, NULL, 0, &nativeCount);
    if (!nativeCount)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE, "EGL: No EGLConfigs returned");
        return GL_FALSE;
    }

    nativeConfigs = calloc(nativeCount, sizeof(EGLConfig));
    eglGetConfigs(_glfw.egl.display, nativeConfigs, nativeCount, &nativeCount);

    usableConfigs = calloc(nativeCount, sizeof(_GLFWfbconfig));
    usableCount = 0;

    for (i = 0;  i < nativeCount;  i++)
    {
        const EGLConfig n = nativeConfigs[i];
        _GLFWfbconfig* u = usableConfigs + usableCount;

#if defined(_GLFW_X11)
        if (!getConfigAttrib(n, EGL_NATIVE_VISUAL_ID))
        {
            // Only consider EGLConfigs with associated visuals
            continue;
        }
#endif // _GLFW_X11

        if (!(getConfigAttrib(n, EGL_COLOR_BUFFER_TYPE) & EGL_RGB_BUFFER))
        {
            // Only consider RGB(A) EGLConfigs
            continue;
        }

        if (!(getConfigAttrib(n, EGL_SURFACE_TYPE) & EGL_WINDOW_BIT))
        {
            // Only consider window EGLConfigs
            continue;
        }

        if (wndconfig->clientAPI == GLFW_OPENGL_ES_API)
        {
            if (wndconfig->glMajor == 1)
            {
                if (!(getConfigAttrib(n, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES_BIT))
                    continue;
            }
            else
            {
                if (!(getConfigAttrib(n, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES2_BIT))
                    continue;
            }
        }
        else if (wndconfig->clientAPI == GLFW_OPENGL_API)
        {
            if (!(getConfigAttrib(n, EGL_RENDERABLE_TYPE) & EGL_OPENGL_BIT))
                continue;
        }

        u->redBits = getConfigAttrib(n, EGL_RED_SIZE);
        u->greenBits = getConfigAttrib(n, EGL_GREEN_SIZE);
        u->blueBits = getConfigAttrib(n, EGL_BLUE_SIZE);

        u->alphaBits = getConfigAttrib(n, EGL_ALPHA_SIZE);
        u->depthBits = getConfigAttrib(n, EGL_DEPTH_SIZE);
        u->stencilBits = getConfigAttrib(n, EGL_STENCIL_SIZE);

        u->samples = getConfigAttrib(n, EGL_SAMPLES);

        u->egl = n;
        usableCount++;
    }

    closest = _glfwChooseFBConfig(desired, usableConfigs, usableCount);
    if (closest)
        *result = closest->egl;

    free(nativeConfigs);
    free(usableConfigs);

    return closest ? GL_TRUE : GL_FALSE;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Initialize EGL
//
int _glfwInitContextAPI(void)
{
    _glfw.egl.display = eglGetDisplay((EGLNativeDisplayType)_GLFW_EGL_NATIVE_DISPLAY);
    if (_glfw.egl.display == EGL_NO_DISPLAY)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "EGL: Failed to get EGL display: %s",
                        getErrorString(eglGetError()));
        return GL_FALSE;
    }

    if (!eglInitialize(_glfw.egl.display,
                       &_glfw.egl.versionMajor,
                       &_glfw.egl.versionMinor))
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "EGL: Failed to initialize EGL: %s",
                        getErrorString(eglGetError()));
        return GL_FALSE;
    }

    if (_glfwPlatformExtensionSupported("EGL_KHR_create_context"))
        _glfw.egl.KHR_create_context = GL_TRUE;

    return GL_TRUE;
}

// Terminate EGL
//
void _glfwTerminateContextAPI(void)
{
    eglTerminate(_glfw.egl.display);
}

#define setEGLattrib(attribName, attribValue) \
{ \
    attribs[index++] = attribName; \
    attribs[index++] = attribValue; \
    assert((size_t) index < sizeof(attribs) / sizeof(attribs[0])); \
}

// Prepare for creation of the OpenGL context
//
int _glfwCreateContext(_GLFWwindow* window,
                       const _GLFWwndconfig* wndconfig,
                       const _GLFWfbconfig* fbconfig)
{
    int attribs[40];
    EGLint count = 0;
    EGLConfig config;
    EGLContext share = NULL;

    if (wndconfig->share)
        share = wndconfig->share->egl.context;

    if (!chooseFBConfigs(wndconfig, fbconfig, &config))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "EGL: Failed to find a suitable EGLConfig");
        return GL_FALSE;
    }

#if defined(_GLFW_X11)
    // Retrieve the visual corresponding to the chosen EGL config
    {
        int mask;
        EGLint redBits, greenBits, blueBits, alphaBits, visualID = 0;
        XVisualInfo info;

        eglGetConfigAttrib(_glfw.egl.display, config,
                           EGL_NATIVE_VISUAL_ID, &visualID);

        info.screen = _glfw.x11.screen;
        mask = VisualScreenMask;

        if (visualID)
        {
            // The X window visual must match the EGL config
            info.visualid = visualID;
            mask |= VisualIDMask;
        }
        else
        {
            // some EGL drivers don't implement the EGL_NATIVE_VISUAL_ID
            // attribute, so attempt to find the closest match.

            eglGetConfigAttrib(_glfw.egl.display, config,
                               EGL_RED_SIZE, &redBits);
            eglGetConfigAttrib(_glfw.egl.display, config,
                               EGL_GREEN_SIZE, &greenBits);
            eglGetConfigAttrib(_glfw.egl.display, config,
                               EGL_BLUE_SIZE, &blueBits);
            eglGetConfigAttrib(_glfw.egl.display, config,
                               EGL_ALPHA_SIZE, &alphaBits);

            info.depth = redBits + greenBits + blueBits + alphaBits;
            mask |= VisualDepthMask;
        }

        window->egl.visual = XGetVisualInfo(_glfw.x11.display,
                                            mask, &info, &count);

        if (window->egl.visual == NULL)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "EGL: Failed to retrieve visual for EGLConfig");
            return GL_FALSE;
        }
    }
#endif // _GLFW_X11

    if (wndconfig->clientAPI == GLFW_OPENGL_ES_API)
    {
        if (!eglBindAPI(EGL_OPENGL_ES_API))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "EGL: Failed to bind OpenGL ES: %s",
                            getErrorString(eglGetError()));
            return GL_FALSE;
        }
    }
    else
    {
        if (!eglBindAPI(EGL_OPENGL_API))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "EGL: Failed to bind OpenGL: %s",
                            getErrorString(eglGetError()));
            return GL_FALSE;
        }
    }

    if (_glfw.egl.KHR_create_context)
    {
        int index = 0, mask = 0, flags = 0, strategy = 0;

        if (wndconfig->clientAPI == GLFW_OPENGL_API)
        {
            if (wndconfig->glProfile == GLFW_OPENGL_CORE_PROFILE)
                mask |= EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
            else if (wndconfig->glProfile == GLFW_OPENGL_COMPAT_PROFILE)
                mask |= EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;

            if (wndconfig->glForward)
                flags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;

            if (wndconfig->glDebug)
                flags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
        }

        if (wndconfig->glRobustness != GLFW_NO_ROBUSTNESS)
        {
            if (wndconfig->glRobustness == GLFW_NO_RESET_NOTIFICATION)
                strategy = EGL_NO_RESET_NOTIFICATION_KHR;
            else if (wndconfig->glRobustness == GLFW_LOSE_CONTEXT_ON_RESET)
                strategy = EGL_LOSE_CONTEXT_ON_RESET_KHR;

            flags |= EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR;
        }

        if (wndconfig->glMajor != 1 || wndconfig->glMinor != 0)
        {
            setEGLattrib(EGL_CONTEXT_MAJOR_VERSION_KHR, wndconfig->glMajor);
            setEGLattrib(EGL_CONTEXT_MINOR_VERSION_KHR, wndconfig->glMinor);
        }

        if (mask)
            setEGLattrib(EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, mask);

        if (flags)
            setEGLattrib(EGL_CONTEXT_FLAGS_KHR, flags);

        if (strategy)
            setEGLattrib(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR, strategy);

        setEGLattrib(EGL_NONE, EGL_NONE);
    }
    else
    {
        int index = 0;

        if (wndconfig->clientAPI == GLFW_OPENGL_ES_API)
            setEGLattrib(EGL_CONTEXT_CLIENT_VERSION, wndconfig->glMajor);

        setEGLattrib(EGL_NONE, EGL_NONE);
    }

    window->egl.context = eglCreateContext(_glfw.egl.display,
                                           config, share, attribs);

    if (window->egl.context == EGL_NO_CONTEXT)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "EGL: Failed to create context: %s",
                        getErrorString(eglGetError()));
        return GL_FALSE;
    }

    window->egl.config = config;

    return GL_TRUE;
}

#undef setEGLattrib

// Destroy the OpenGL context
//
void _glfwDestroyContext(_GLFWwindow* window)
{
#if defined(_GLFW_X11)
    if (window->egl.visual)
    {
       XFree(window->egl.visual);
       window->egl.visual = NULL;
    }
#endif // _GLFW_X11

    if (window->egl.surface)
    {
        eglDestroySurface(_glfw.egl.display, window->egl.surface);
        window->egl.surface = EGL_NO_SURFACE;
    }

    if (window->egl.context)
    {
        eglDestroyContext(_glfw.egl.display, window->egl.context);
        window->egl.context = EGL_NO_CONTEXT;
    }
}

// Analyzes the specified context for possible recreation
//
int _glfwAnalyzeContext(const _GLFWwindow* window,
                        const _GLFWwndconfig* wndconfig,
                        const _GLFWfbconfig* fbconfig)
{
#if defined(_GLFW_WIN32)
    return _GLFW_RECREATION_NOT_NEEDED;
#else
    return 0;
#endif
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

void _glfwPlatformMakeContextCurrent(_GLFWwindow* window)
{
    if (window)
    {
        if (window->egl.surface == EGL_NO_SURFACE)
        {
            window->egl.surface = eglCreateWindowSurface(_glfw.egl.display,
                                                         window->egl.config,
                                                         (EGLNativeWindowType)_GLFW_EGL_NATIVE_WINDOW,
                                                         NULL);
            if (window->egl.surface == EGL_NO_SURFACE)
            {
                _glfwInputError(GLFW_PLATFORM_ERROR,
                                "EGL: Failed to create window surface: %s",
                                getErrorString(eglGetError()));
            }
        }

        eglMakeCurrent(_glfw.egl.display,
                       window->egl.surface,
                       window->egl.surface,
                       window->egl.context);
    }
    else
    {
        eglMakeCurrent(_glfw.egl.display,
                       EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    _glfwCurrentWindow = window;
}

_GLFWwindow* _glfwPlatformGetCurrentContext(void)
{
    return _glfwCurrentWindow;
}

void _glfwPlatformSwapBuffers(_GLFWwindow* window)
{
    eglSwapBuffers(_glfw.egl.display, window->egl.surface);
}

void _glfwPlatformSwapInterval(int interval)
{
    eglSwapInterval(_glfw.egl.display, interval);
}

int _glfwPlatformExtensionSupported(const char* extension)
{
    const char* extensions;

    extensions = eglQueryString(_glfw.egl.display, EGL_EXTENSIONS);
    if (extensions != NULL)
    {
        if (_glfwStringInExtensionString(extension, (unsigned char*) extensions))
            return GL_TRUE;
    }

    return GL_FALSE;
}

GLFWglproc _glfwPlatformGetProcAddress(const char* procname)
{
    return eglGetProcAddress(procname);
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW native API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI EGLDisplay glfwGetEGLDisplay(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return _glfw.egl.display;
}

GLFWAPI EGLContext glfwGetEGLContext(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return window->egl.context;
}

GLFWAPI EGLSurface glfwGetEGLSurface(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return window->egl.surface;
}

