//========================================================================
// GLFW 3.0 - www.glfw.org
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
#include <string.h>
#include <limits.h>
#include <stdio.h>


// Parses the client API version string and extracts the version number
//
static GLboolean parseGLVersion(int* api, int* major, int* minor, int* rev)
{
    int i, _api = GLFW_OPENGL_API, _major, _minor = 0, _rev = 0;
    const char* version;
    const char* prefixes[] =
    {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    version = (const char*) glGetString(GL_VERSION);
    if (!version)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Failed to retrieve context version string");
        return GL_FALSE;
    }

    for (i = 0;  prefixes[i];  i++)
    {
        const size_t length = strlen(prefixes[i]);

        if (strncmp(version, prefixes[i], length) == 0)
        {
            version += length;
            _api = GLFW_OPENGL_ES_API;
            break;
        }
    }

    if (!sscanf(version, "%d.%d.%d", &_major, &_minor, &_rev))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "No version found in context version string");
        return GL_FALSE;
    }

    *api = _api;
    *major = _major;
    *minor = _minor;
    *rev = _rev;

    return GL_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

GLboolean _glfwIsValidContextConfig(_GLFWwndconfig* wndconfig)
{
    if (wndconfig->clientAPI != GLFW_OPENGL_API &&
        wndconfig->clientAPI != GLFW_OPENGL_ES_API)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid client API requested");
        return GL_FALSE;
    }

    if (wndconfig->clientAPI == GLFW_OPENGL_API)
    {
        if ((wndconfig->glMajor < 1 || wndconfig->glMinor < 0) ||
            (wndconfig->glMajor == 1 && wndconfig->glMinor > 5) ||
            (wndconfig->glMajor == 2 && wndconfig->glMinor > 1) ||
            (wndconfig->glMajor == 3 && wndconfig->glMinor > 3))
        {
            // OpenGL 1.0 is the smallest valid version
            // OpenGL 1.x series ended with version 1.5
            // OpenGL 2.x series ended with version 2.1
            // OpenGL 3.x series ended with version 3.3

            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid OpenGL version %i.%i requested",
                            wndconfig->glMajor, wndconfig->glMinor);
            return GL_FALSE;
        }
        else
        {
            // For now, let everything else through
        }

        if (wndconfig->glProfile)
        {
            if (wndconfig->glProfile != GLFW_OPENGL_CORE_PROFILE &&
                wndconfig->glProfile != GLFW_OPENGL_COMPAT_PROFILE)
            {
                _glfwInputError(GLFW_INVALID_ENUM,
                                "Invalid OpenGL profile requested");
                return GL_FALSE;
            }

            if (wndconfig->glMajor < 3 ||
                (wndconfig->glMajor == 3 && wndconfig->glMinor < 2))
            {
                // Desktop OpenGL context profiles are only defined for version 3.2
                // and above

                _glfwInputError(GLFW_INVALID_VALUE,
                                "Context profiles only exist for "
                                "OpenGL version 3.2 and above");
                return GL_FALSE;
            }
        }

        if (wndconfig->glForward && wndconfig->glMajor < 3)
        {
            // Forward-compatible contexts are only defined for OpenGL version 3.0 and above
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Forward compatibility only exist for OpenGL "
                            "version 3.0 and above");
            return GL_FALSE;
        }
    }
    else if (wndconfig->clientAPI == GLFW_OPENGL_ES_API)
    {
        if (wndconfig->glMajor < 1 || wndconfig->glMinor < 0 ||
            (wndconfig->glMajor == 1 && wndconfig->glMinor > 1) ||
            (wndconfig->glMajor == 2 && wndconfig->glMinor > 0))
        {
            // OpenGL ES 1.0 is the smallest valid version
            // OpenGL ES 1.x series ended with version 1.1
            // OpenGL ES 2.x series ended with version 2.0

            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid OpenGL ES version %i.%i requested",
                            wndconfig->glMajor, wndconfig->glMinor);
            return GL_FALSE;
        }
        else
        {
            // For now, let everything else through
        }

        if (wndconfig->glProfile)
        {
            // OpenGL ES does not support profiles
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Context profiles are not supported by OpenGL ES");
            return GL_FALSE;
        }

        if (wndconfig->glForward)
        {
            // OpenGL ES does not support forward-compatibility
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Forward compatibility is not supported by OpenGL ES");
            return GL_FALSE;
        }
    }

    if (wndconfig->glRobustness)
    {
        if (wndconfig->glRobustness != GLFW_NO_RESET_NOTIFICATION &&
            wndconfig->glRobustness != GLFW_LOSE_CONTEXT_ON_RESET)
        {
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid context robustness mode requested");
            return GL_FALSE;
        }
    }

    return GL_TRUE;
}

const _GLFWfbconfig* _glfwChooseFBConfig(const _GLFWfbconfig* desired,
                                         const _GLFWfbconfig* alternatives,
                                         unsigned int count)
{
    unsigned int i;
    unsigned int missing, leastMissing = UINT_MAX;
    unsigned int colorDiff, leastColorDiff = UINT_MAX;
    unsigned int extraDiff, leastExtraDiff = UINT_MAX;
    const _GLFWfbconfig* current;
    const _GLFWfbconfig* closest = NULL;

    for (i = 0;  i < count;  i++)
    {
        current = alternatives + i;

        if (desired->stereo > 0 && current->stereo == 0)
        {
            // Stereo is a hard constraint
            continue;
        }

        // Count number of missing buffers
        {
            missing = 0;

            if (desired->alphaBits > 0 && current->alphaBits == 0)
                missing++;

            if (desired->depthBits > 0 && current->depthBits == 0)
                missing++;

            if (desired->stencilBits > 0 && current->stencilBits == 0)
                missing++;

            if (desired->auxBuffers > 0 && current->auxBuffers < desired->auxBuffers)
                missing += desired->auxBuffers - current->auxBuffers;

            if (desired->samples > 0 && current->samples == 0)
            {
                // Technically, several multisampling buffers could be
                // involved, but that's a lower level implementation detail and
                // not important to us here, so we count them as one
                missing++;
            }
        }

        // These polynomials make many small channel size differences matter
        // less than one large channel size difference

        // Calculate color channel size difference value
        {
            colorDiff = 0;

            if (desired->redBits > 0)
            {
                colorDiff += (desired->redBits - current->redBits) *
                             (desired->redBits - current->redBits);
            }

            if (desired->greenBits > 0)
            {
                colorDiff += (desired->greenBits - current->greenBits) *
                             (desired->greenBits - current->greenBits);
            }

            if (desired->blueBits > 0)
            {
                colorDiff += (desired->blueBits - current->blueBits) *
                             (desired->blueBits - current->blueBits);
            }
        }

        // Calculate non-color channel size difference value
        {
            extraDiff = 0;

            if (desired->alphaBits > 0)
            {
                extraDiff += (desired->alphaBits - current->alphaBits) *
                             (desired->alphaBits - current->alphaBits);
            }

            if (desired->depthBits > 0)
            {
                extraDiff += (desired->depthBits - current->depthBits) *
                             (desired->depthBits - current->depthBits);
            }

            if (desired->stencilBits > 0)
            {
                extraDiff += (desired->stencilBits - current->stencilBits) *
                             (desired->stencilBits - current->stencilBits);
            }

            if (desired->accumRedBits > 0)
            {
                extraDiff += (desired->accumRedBits - current->accumRedBits) *
                             (desired->accumRedBits - current->accumRedBits);
            }

            if (desired->accumGreenBits > 0)
            {
                extraDiff += (desired->accumGreenBits - current->accumGreenBits) *
                             (desired->accumGreenBits - current->accumGreenBits);
            }

            if (desired->accumBlueBits > 0)
            {
                extraDiff += (desired->accumBlueBits - current->accumBlueBits) *
                             (desired->accumBlueBits - current->accumBlueBits);
            }

            if (desired->accumAlphaBits > 0)
            {
                extraDiff += (desired->accumAlphaBits - current->accumAlphaBits) *
                             (desired->accumAlphaBits - current->accumAlphaBits);
            }

            if (desired->samples > 0)
            {
                extraDiff += (desired->samples - current->samples) *
                             (desired->samples - current->samples);
            }

            if (desired->sRGB)
            {
                if (!current->sRGB)
                    extraDiff++;
            }
        }

        // Figure out if the current one is better than the best one found so far
        // Least number of missing buffers is the most important heuristic,
        // then color buffer size match and lastly size match for other buffers

        if (missing < leastMissing)
            closest = current;
        else if (missing == leastMissing)
        {
            if ((colorDiff < leastColorDiff) ||
                (colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
            {
                closest = current;
            }
        }

        if (current == closest)
        {
            leastMissing = missing;
            leastColorDiff = colorDiff;
            leastExtraDiff = extraDiff;
        }
    }

    return closest;
}

GLboolean _glfwRefreshContextAttribs(void)
{
    _GLFWwindow* window = _glfwPlatformGetCurrentContext();

    if (!parseGLVersion(&window->clientAPI,
                        &window->glMajor,
                        &window->glMinor,
                        &window->glRevision))
    {
        return GL_FALSE;
    }

#if defined(_GLFW_USE_OPENGL)
    if (window->glMajor > 2)
    {
        // OpenGL 3.0+ uses a different function for extension string retrieval
        // We cache it here instead of in glfwExtensionSupported mostly to alert
        // users as early as possible that their build may be broken

        window->GetStringi = (PFNGLGETSTRINGIPROC) glfwGetProcAddress("glGetStringi");
        if (!window->GetStringi)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "Entry point retrieval is broken");
            return GL_FALSE;
        }
    }

    if (window->clientAPI == GLFW_OPENGL_API)
    {
        // Read back context flags (OpenGL 3.0 and above)
        if (window->glMajor >= 3)
        {
            GLint flags;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

            if (flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
                window->glForward = GL_TRUE;

            if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                window->glDebug = GL_TRUE;
            else if (glfwExtensionSupported("GL_ARB_debug_output"))
            {
                // HACK: This is a workaround for older drivers (pre KHR_debug)
                //       not setting the debug bit in the context flags for
                //       debug contexts
                window->glDebug = GL_TRUE;
            }
        }

        // Read back OpenGL context profile (OpenGL 3.2 and above)
        if (window->glMajor > 3 ||
            (window->glMajor == 3 && window->glMinor >= 2))
        {
            GLint mask;
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);

            if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
                window->glProfile = GLFW_OPENGL_COMPAT_PROFILE;
            else if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
                window->glProfile = GLFW_OPENGL_CORE_PROFILE;
            else if (glfwExtensionSupported("GL_ARB_compatibility"))
            {
                // HACK: This is a workaround for the compatibility profile bit
                //       not being set in the context flags if an OpenGL 3.2+
                //       context was created without having requested a specific
                //       version
                window->glProfile = GLFW_OPENGL_COMPAT_PROFILE;
            }
        }

        // Read back robustness strategy
        if (glfwExtensionSupported("GL_ARB_robustness"))
        {
            // NOTE: We avoid using the context flags for detection, as they are
            //       only present from 3.0 while the extension applies from 1.1

            GLint strategy;
            glGetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, &strategy);

            if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                window->glRobustness = GLFW_LOSE_CONTEXT_ON_RESET;
            else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                window->glRobustness = GLFW_NO_RESET_NOTIFICATION;
        }
    }
    else
    {
        // Read back robustness strategy
        if (glfwExtensionSupported("GL_EXT_robustness"))
        {
            // NOTE: The values of these constants match those of the OpenGL ARB
            //       one, so we can reuse them here

            GLint strategy;
            glGetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, &strategy);

            if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                window->glRobustness = GLFW_LOSE_CONTEXT_ON_RESET;
            else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                window->glRobustness = GLFW_NO_RESET_NOTIFICATION;
        }
    }
#endif // _GLFW_USE_OPENGL

    return GL_TRUE;
}

GLboolean _glfwIsValidContext(_GLFWwndconfig* wndconfig)
{
    _GLFWwindow* window = _glfwPlatformGetCurrentContext();

    if (window->glMajor < wndconfig->glMajor ||
        (window->glMajor == wndconfig->glMajor &&
         window->glMinor < wndconfig->glMinor))
    {
        // The desired OpenGL version is greater than the actual version
        // This only happens if the machine lacks {GLX|WGL}_ARB_create_context
        // /and/ the user has requested an OpenGL version greater than 1.0

        // For API consistency, we emulate the behavior of the
        // {GLX|WGL}_ARB_create_context extension and fail here

        _glfwInputError(GLFW_VERSION_UNAVAILABLE, NULL);
        return GL_FALSE;
    }

    return GL_TRUE;
}

int _glfwStringInExtensionString(const char* string, const GLubyte* extensions)
{
    const GLubyte* start;
    GLubyte* where;
    GLubyte* terminator;

    // It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string. Don't be fooled by sub-strings,
    // etc.
    start = extensions;
    for (;;)
    {
        where = (GLubyte*) strstr((const char*) start, string);
        if (!where)
            return GL_FALSE;

        terminator = where + strlen(string);
        if (where == start || *(where - 1) == ' ')
        {
            if (*terminator == ' ' || *terminator == '\0')
                break;
        }

        start = terminator;
    }

    return GL_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI void glfwMakeContextCurrent(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;

    _GLFW_REQUIRE_INIT();

    if (_glfwPlatformGetCurrentContext() == window)
        return;

    _glfwPlatformMakeContextCurrent(window);
}

GLFWAPI GLFWwindow* glfwGetCurrentContext(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return (GLFWwindow*) _glfwPlatformGetCurrentContext();
}

GLFWAPI void glfwSwapBuffers(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    _glfwPlatformSwapBuffers(window);
}

GLFWAPI void glfwSwapInterval(int interval)
{
    _GLFW_REQUIRE_INIT();

    if (!_glfwPlatformGetCurrentContext())
    {
        _glfwInputError(GLFW_NO_CURRENT_CONTEXT, NULL);
        return;
    }

    _glfwPlatformSwapInterval(interval);
}

GLFWAPI int glfwExtensionSupported(const char* extension)
{
    const GLubyte* extensions;
    _GLFWwindow* window;

    _GLFW_REQUIRE_INIT_OR_RETURN(GL_FALSE);

    window = _glfwPlatformGetCurrentContext();
    if (!window)
    {
        _glfwInputError(GLFW_NO_CURRENT_CONTEXT, NULL);
        return GL_FALSE;
    }

    if (!extension || *extension == '\0')
    {
        _glfwInputError(GLFW_INVALID_VALUE, NULL);
        return GL_FALSE;
    }

    if (window->glMajor < 3)
    {
        // Check if extension is in the old style OpenGL extensions string

        extensions = glGetString(GL_EXTENSIONS);
        if (!extensions)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "Failed to retrieve extension string");
            return GL_FALSE;
        }

        if (_glfwStringInExtensionString(extension, extensions))
            return GL_TRUE;
    }
#if defined(_GLFW_USE_OPENGL)
    else
    {
        int i;
        GLint count;

        // Check if extension is in the modern OpenGL extensions string list

        glGetIntegerv(GL_NUM_EXTENSIONS, &count);

        for (i = 0;  i < count;  i++)
        {
            const char* en = (const char*) window->GetStringi(GL_EXTENSIONS, i);
            if (!en)
            {
                _glfwInputError(GLFW_PLATFORM_ERROR,
                                "Failed to retrieve extension string %i", i);
                return GL_FALSE;
            }

            if (strcmp(en, extension) == 0)
                return GL_TRUE;
        }
    }
#endif // _GLFW_USE_OPENGL

    // Check if extension is in the platform-specific string
    return _glfwPlatformExtensionSupported(extension);
}

GLFWAPI GLFWglproc glfwGetProcAddress(const char* procname)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (!_glfwPlatformGetCurrentContext())
    {
        _glfwInputError(GLFW_NO_CURRENT_CONTEXT, NULL);
        return NULL;
    }

    return _glfwPlatformGetProcAddress(procname);
}

