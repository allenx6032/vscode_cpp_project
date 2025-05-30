/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

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
#include "SDL_config.h"

#if SDL_VIDEO_RENDER_OGL_ES && !SDL_RENDER_DISABLED

#include "SDL_hints.h"
#include "SDL_opengles.h"
#include "SDL_sysrender.h"

#if defined(SDL_VIDEO_DRIVER_PANDORA)

/* Empty function stub to get OpenGL ES 1.x support without  */
/* OpenGL ES extension GL_OES_draw_texture supported         */
GL_API void GL_APIENTRY
glDrawTexiOES(GLint x, GLint y, GLint z, GLint width, GLint height)
{
    return;
}

#endif /* PANDORA */

/* OpenGL ES 1.1 renderer implementation, based on the OpenGL renderer */

/* Used to re-create the window with OpenGL ES capability */
extern int SDL_RecreateWindow(SDL_Window * window, Uint32 flags);

static const float inv255f = 1.0f / 255.0f;

static SDL_Renderer *GLES_CreateRenderer(SDL_Window * window, Uint32 flags);
static void GLES_WindowEvent(SDL_Renderer * renderer,
                             const SDL_WindowEvent *event);
static int GLES_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture);
static int GLES_UpdateTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                              const SDL_Rect * rect, const void *pixels,
                              int pitch);
static int GLES_LockTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                            const SDL_Rect * rect, void **pixels, int *pitch);
static void GLES_UnlockTexture(SDL_Renderer * renderer,
                               SDL_Texture * texture);
static int GLES_SetRenderTarget(SDL_Renderer * renderer,
                                 SDL_Texture * texture);
static int GLES_UpdateViewport(SDL_Renderer * renderer);
static int GLES_UpdateClipRect(SDL_Renderer * renderer);
static int GLES_RenderClear(SDL_Renderer * renderer);
static int GLES_RenderDrawPoints(SDL_Renderer * renderer,
                                 const SDL_FPoint * points, int count);
static int GLES_RenderDrawLines(SDL_Renderer * renderer,
                                const SDL_FPoint * points, int count);
static int GLES_RenderFillRects(SDL_Renderer * renderer,
                                const SDL_FRect * rects, int count);
static int GLES_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                           const SDL_Rect * srcrect,
                           const SDL_FRect * dstrect);
static int GLES_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture,
                         const SDL_Rect * srcrect, const SDL_FRect * dstrect,
                         const double angle, const SDL_FPoint *center, const SDL_RendererFlip flip);
static int GLES_RenderReadPixels(SDL_Renderer * renderer, const SDL_Rect * rect,
                    Uint32 pixel_format, void * pixels, int pitch);
static void GLES_RenderPresent(SDL_Renderer * renderer);
static void GLES_DestroyTexture(SDL_Renderer * renderer,
                                SDL_Texture * texture);
static void GLES_DestroyRenderer(SDL_Renderer * renderer);
static int GLES_BindTexture (SDL_Renderer * renderer, SDL_Texture *texture, float *texw, float *texh);
static int GLES_UnbindTexture (SDL_Renderer * renderer, SDL_Texture *texture);

typedef struct GLES_FBOList GLES_FBOList;

struct GLES_FBOList
{
   Uint32 w, h;
   GLuint FBO;
   GLES_FBOList *next;
};


SDL_RenderDriver GLES_RenderDriver = {
    GLES_CreateRenderer,
    {
     "opengles",
     (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ),
     1,
     {SDL_PIXELFORMAT_ABGR8888},
     0,
     0}
};

typedef struct
{
    SDL_GLContext context;
    struct {
        Uint32 color;
        int blendMode;
        SDL_bool tex_coords;
    } current;

#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;
#define SDL_PROC_OES SDL_PROC
#include "SDL_glesfuncs.h"
#undef SDL_PROC
#undef SDL_PROC_OES
    SDL_bool GL_OES_framebuffer_object_supported;
    GLES_FBOList *framebuffers;
    GLuint window_framebuffer;

    SDL_bool useDrawTexture;
    SDL_bool GL_OES_draw_texture_supported;
    SDL_bool GL_OES_blend_func_separate_supported;
} GLES_RenderData;

typedef struct
{
    GLuint texture;
    GLenum type;
    GLfloat texw;
    GLfloat texh;
    GLenum format;
    GLenum formattype;
    void *pixels;
    int pitch;
    GLES_FBOList *fbo;
} GLES_TextureData;

static int
GLES_SetError(const char *prefix, GLenum result)
{
    const char *error;

    switch (result) {
    case GL_NO_ERROR:
        error = "GL_NO_ERROR";
        break;
    case GL_INVALID_ENUM:
        error = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        error = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        error = "GL_INVALID_OPERATION";
        break;
    case GL_STACK_OVERFLOW:
        error = "GL_STACK_OVERFLOW";
        break;
    case GL_STACK_UNDERFLOW:
        error = "GL_STACK_UNDERFLOW";
        break;
    case GL_OUT_OF_MEMORY:
        error = "GL_OUT_OF_MEMORY";
        break;
    default:
        error = "UNKNOWN";
        break;
    }
    return SDL_SetError("%s: %s", prefix, error);
}

static int GLES_LoadFunctions(GLES_RenderData * data)
{
#if SDL_VIDEO_DRIVER_UIKIT
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_ANDROID
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_PANDORA
#define __SDL_NOGETPROCADDR__
#endif

#ifdef __SDL_NOGETPROCADDR__
#define SDL_PROC(ret,func,params) data->func=func;
#define SDL_PROC_OES(ret,func,params) data->func=func;
#else
#define SDL_PROC(ret,func,params) \
    do { \
        data->func = SDL_GL_GetProcAddress(#func); \
        if ( ! data->func ) { \
            return SDL_SetError("Couldn't load GLES function %s: %s\n", #func, SDL_GetError()); \
        } \
    } while ( 0 );
#define SDL_PROC_OES(ret,func,params) \
    do { \
        data->func = SDL_GL_GetProcAddress(#func); \
    } while ( 0 );    
#endif /* _SDL_NOGETPROCADDR_ */

#include "SDL_glesfuncs.h"
#undef SDL_PROC
#undef SDL_PROC_OES
    return 0;
}

static SDL_GLContext SDL_CurrentContext = NULL;

GLES_FBOList *
GLES_GetFBO(GLES_RenderData *data, Uint32 w, Uint32 h)
{
   GLES_FBOList *result = data->framebuffers;
   while ((result) && ((result->w != w) || (result->h != h)) )
   {
       result = result->next;
   }
   if (result == NULL)
   {
       result = SDL_malloc(sizeof(GLES_FBOList));
       result->w = w;
       result->h = h;
       data->glGenFramebuffersOES(1, &result->FBO);
       result->next = data->framebuffers;
       data->framebuffers = result;
   }
   return result;
}


static int
GLES_ActivateRenderer(SDL_Renderer * renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    if (SDL_CurrentContext != data->context) {
        if (SDL_GL_MakeCurrent(renderer->window, data->context) < 0) {
            return -1;
        }
        SDL_CurrentContext = data->context;

        GLES_UpdateViewport(renderer);
    }
    return 0;
}

/* This is called if we need to invalidate all of the SDL OpenGL state */
static void
GLES_ResetState(SDL_Renderer *renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    if (SDL_CurrentContext == data->context) {
        GLES_UpdateViewport(renderer);
    } else {
        GLES_ActivateRenderer(renderer);
    }

    data->current.color = 0;
    data->current.blendMode = -1;
    data->current.tex_coords = SDL_FALSE;

    data->glDisable(GL_DEPTH_TEST);
    data->glDisable(GL_CULL_FACE);

    data->glMatrixMode(GL_MODELVIEW);
    data->glLoadIdentity();

    data->glEnableClientState(GL_VERTEX_ARRAY);
    data->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

SDL_Renderer *
GLES_CreateRenderer(SDL_Window * window, Uint32 flags)
{

    SDL_Renderer *renderer;
    GLES_RenderData *data;
    GLint value;
    Uint32 windowFlags;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    windowFlags = SDL_GetWindowFlags(window);
    if (!(windowFlags & SDL_WINDOW_OPENGL)) {
        if (SDL_RecreateWindow(window, windowFlags | SDL_WINDOW_OPENGL) < 0) {
            /* Uh oh, better try to put it back... */
            SDL_RecreateWindow(window, windowFlags);
            return NULL;
        }
    }

    renderer = (SDL_Renderer *) SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }

    data = (GLES_RenderData *) SDL_calloc(1, sizeof(*data));
    if (!data) {
        GLES_DestroyRenderer(renderer);
        SDL_OutOfMemory();
        return NULL;
    }

    renderer->WindowEvent = GLES_WindowEvent;
    renderer->CreateTexture = GLES_CreateTexture;
    renderer->UpdateTexture = GLES_UpdateTexture;
    renderer->LockTexture = GLES_LockTexture;
    renderer->UnlockTexture = GLES_UnlockTexture;
    renderer->SetRenderTarget = GLES_SetRenderTarget;
    renderer->UpdateViewport = GLES_UpdateViewport;
    renderer->UpdateClipRect = GLES_UpdateClipRect;
    renderer->RenderClear = GLES_RenderClear;
    renderer->RenderDrawPoints = GLES_RenderDrawPoints;
    renderer->RenderDrawLines = GLES_RenderDrawLines;
    renderer->RenderFillRects = GLES_RenderFillRects;
    renderer->RenderCopy = GLES_RenderCopy;
    renderer->RenderCopyEx = GLES_RenderCopyEx;
    renderer->RenderReadPixels = GLES_RenderReadPixels;
    renderer->RenderPresent = GLES_RenderPresent;
    renderer->DestroyTexture = GLES_DestroyTexture;
    renderer->DestroyRenderer = GLES_DestroyRenderer;
    renderer->GL_BindTexture = GLES_BindTexture;
    renderer->GL_UnbindTexture = GLES_UnbindTexture;
    renderer->info = GLES_RenderDriver.info;
    renderer->info.flags = SDL_RENDERER_ACCELERATED;
    renderer->driverdata = data;
    renderer->window = window;

    data->context = SDL_GL_CreateContext(window);
    if (!data->context) {
        GLES_DestroyRenderer(renderer);
        return NULL;
    }
    if (SDL_GL_MakeCurrent(window, data->context) < 0) {
        GLES_DestroyRenderer(renderer);
        return NULL;
    }

    if (GLES_LoadFunctions(data) < 0) {
        GLES_DestroyRenderer(renderer);
        return NULL;
    }

    if (flags & SDL_RENDERER_PRESENTVSYNC) {
        SDL_GL_SetSwapInterval(1);
    } else {
        SDL_GL_SetSwapInterval(0);
    }
    if (SDL_GL_GetSwapInterval() > 0) {
        renderer->info.flags |= SDL_RENDERER_PRESENTVSYNC;
    }

#if SDL_VIDEO_DRIVER_PANDORA
    data->GL_OES_draw_texture_supported = SDL_FALSE;
    data->useDrawTexture = SDL_FALSE;
#else
    if (SDL_GL_ExtensionSupported("GL_OES_draw_texture")) {
        data->GL_OES_draw_texture_supported = SDL_TRUE;
        data->useDrawTexture = SDL_TRUE;
    } else {
        data->GL_OES_draw_texture_supported = SDL_FALSE;
        data->useDrawTexture = SDL_FALSE;
    }
#endif

    value = 0;
    data->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
    renderer->info.max_texture_width = value;
    value = 0;
    data->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
    renderer->info.max_texture_height = value;

    /* Android does not report GL_OES_framebuffer_object but the functionality seems to be there anyway */
    if (SDL_GL_ExtensionSupported("GL_OES_framebuffer_object") || data->glGenFramebuffersOES) {
        data->GL_OES_framebuffer_object_supported = SDL_TRUE;
        renderer->info.flags |= SDL_RENDERER_TARGETTEXTURE;

        value = 0;
        data->glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &value);
        data->window_framebuffer = (GLuint)value;
    }
    data->framebuffers = NULL;

    if (SDL_GL_ExtensionSupported("GL_OES_blend_func_separate")) {
        data->GL_OES_blend_func_separate_supported = SDL_TRUE;
    }

    /* Set up parameters for rendering */
    GLES_ResetState(renderer);

    return renderer;
}

static void
GLES_WindowEvent(SDL_Renderer * renderer, const SDL_WindowEvent *event)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    if (event->event == SDL_WINDOWEVENT_SIZE_CHANGED ||
        event->event == SDL_WINDOWEVENT_SHOWN ||
        event->event == SDL_WINDOWEVENT_HIDDEN) {
        /* Rebind the context to the window area and update matrices */
        SDL_CurrentContext = NULL;
    }

    if (event->event == SDL_WINDOWEVENT_MINIMIZED) {
        /* According to Apple documentation, we need to finish drawing NOW! */
        data->glFinish();
    }
}

static SDL_INLINE int
power_of_2(int input)
{
    int value = 1;

    while (value < input) {
        value <<= 1;
    }
    return value;
}

static GLenum
GetScaleQuality(void)
{
    const char *hint = SDL_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);

    if (!hint || *hint == '0' || SDL_strcasecmp(hint, "nearest") == 0) {
        return GL_NEAREST;
    } else {
        return GL_LINEAR;
    }
}

static int
GLES_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    GLES_RenderData *renderdata = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *data;
    GLint internalFormat;
    GLenum format, type;
    int texture_w, texture_h;
    GLenum scaleMode;
    GLenum result;

    GLES_ActivateRenderer(renderer);

    switch (texture->format) {
    case SDL_PIXELFORMAT_ABGR8888:
        internalFormat = GL_RGBA;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    default:
        return SDL_SetError("Texture format not supported");
    }

    data = (GLES_TextureData *) SDL_calloc(1, sizeof(*data));
    if (!data) {
        return SDL_OutOfMemory();
    }

    if (texture->access == SDL_TEXTUREACCESS_STREAMING) {
        data->pitch = texture->w * SDL_BYTESPERPIXEL(texture->format);
        data->pixels = SDL_calloc(1, texture->h * data->pitch);
        if (!data->pixels) {
            SDL_free(data);
            return SDL_OutOfMemory();
        }
    }

    
    if (texture->access == SDL_TEXTUREACCESS_TARGET) {
        if (!renderdata->GL_OES_framebuffer_object_supported) {
            SDL_free(data);
            return SDL_SetError("GL_OES_framebuffer_object not supported");
        }
        data->fbo = GLES_GetFBO(renderer->driverdata, texture->w, texture->h);
    } else {
        data->fbo = NULL;
    }
    

    renderdata->glGetError();
    renderdata->glEnable(GL_TEXTURE_2D);
    renderdata->glGenTextures(1, &data->texture);
    result = renderdata->glGetError();
    if (result != GL_NO_ERROR) {
        SDL_free(data);
        return GLES_SetError("glGenTextures()", result);
    }

    data->type = GL_TEXTURE_2D;
    /* no NPOV textures allowed in OpenGL ES (yet) */
    texture_w = power_of_2(texture->w);
    texture_h = power_of_2(texture->h);
    data->texw = (GLfloat) texture->w / texture_w;
    data->texh = (GLfloat) texture->h / texture_h;

    data->format = format;
    data->formattype = type;
    scaleMode = GetScaleQuality();
    renderdata->glBindTexture(data->type, data->texture);
    renderdata->glTexParameteri(data->type, GL_TEXTURE_MIN_FILTER, scaleMode);
    renderdata->glTexParameteri(data->type, GL_TEXTURE_MAG_FILTER, scaleMode);
    renderdata->glTexParameteri(data->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    renderdata->glTexParameteri(data->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    renderdata->glTexImage2D(data->type, 0, internalFormat, texture_w,
                             texture_h, 0, format, type, NULL);
    renderdata->glDisable(GL_TEXTURE_2D);

    result = renderdata->glGetError();
    if (result != GL_NO_ERROR) {
        SDL_free(data);
        return GLES_SetError("glTexImage2D()", result);
    }
    
    texture->driverdata = data;
    return 0;
}

static int
GLES_UpdateTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                   const SDL_Rect * rect, const void *pixels, int pitch)
{
    GLES_RenderData *renderdata = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *data = (GLES_TextureData *) texture->driverdata;
    Uint8 *blob = NULL;
    Uint8 *src;
    int srcPitch;
    int y;

    GLES_ActivateRenderer(renderer);

    /* Bail out if we're supposed to update an empty rectangle */
    if (rect->w <= 0 || rect->h <= 0)
        return 0;

    /* Reformat the texture data into a tightly packed array */
    srcPitch = rect->w * SDL_BYTESPERPIXEL(texture->format);
    src = (Uint8 *)pixels;
    if (pitch != srcPitch) {
        blob = (Uint8 *)SDL_malloc(srcPitch * rect->h);
        if (!blob) {
            return SDL_OutOfMemory();
        }
        src = blob;
        for (y = 0; y < rect->h; ++y) {
            SDL_memcpy(src, pixels, srcPitch);
            src += srcPitch;
            pixels = (Uint8 *)pixels + pitch;
        }
        src = blob;
    }

    /* Create a texture subimage with the supplied data */
    renderdata->glGetError();
    renderdata->glEnable(data->type);
    renderdata->glBindTexture(data->type, data->texture);
    renderdata->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    renderdata->glTexSubImage2D(data->type,
                    0,
                    rect->x,
                    rect->y,
                    rect->w,
                    rect->h,
                    data->format,
                    data->formattype,
                    src);
    SDL_free(blob);

    if (renderdata->glGetError() != GL_NO_ERROR)
    {
        return SDL_SetError("Failed to update texture");
    }
    return 0;
}

static int
GLES_LockTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                 const SDL_Rect * rect, void **pixels, int *pitch)
{
    GLES_TextureData *data = (GLES_TextureData *) texture->driverdata;

    *pixels =
        (void *) ((Uint8 *) data->pixels + rect->y * data->pitch +
                  rect->x * SDL_BYTESPERPIXEL(texture->format));
    *pitch = data->pitch;
    return 0;
}

static void
GLES_UnlockTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    GLES_TextureData *data = (GLES_TextureData *) texture->driverdata;
    SDL_Rect rect;

    /* We do whole texture updates, at least for now */
    rect.x = 0;
    rect.y = 0;
    rect.w = texture->w;
    rect.h = texture->h;
    GLES_UpdateTexture(renderer, texture, &rect, data->pixels, data->pitch);
}

static int
GLES_SetRenderTarget(SDL_Renderer * renderer, SDL_Texture * texture)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *texturedata = NULL;
    GLenum status;

    GLES_ActivateRenderer(renderer);
    
    if (!data->GL_OES_framebuffer_object_supported) {
        return SDL_SetError("Can't enable render target support in this renderer");
    }

    if (texture == NULL) {
        data->glBindFramebufferOES(GL_FRAMEBUFFER_OES, data->window_framebuffer);
        return 0;
    }

    texturedata = (GLES_TextureData *) texture->driverdata;
    data->glBindFramebufferOES(GL_FRAMEBUFFER_OES, texturedata->fbo->FBO);
    /* TODO: check if texture pixel format allows this operation */
    data->glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, texturedata->type, texturedata->texture, 0);
    /* Check FBO status */
    status = data->glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
    if (status != GL_FRAMEBUFFER_COMPLETE_OES) {
        return SDL_SetError("glFramebufferTexture2DOES() failed");
    }
    return 0;
}

static int
GLES_UpdateViewport(SDL_Renderer * renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    if (SDL_CurrentContext != data->context) {
        /* We'll update the viewport after we rebind the context */
        return 0;
    }

    data->glViewport(renderer->viewport.x, renderer->viewport.y,
               renderer->viewport.w, renderer->viewport.h);

    if (renderer->viewport.w && renderer->viewport.h) {
        data->glMatrixMode(GL_PROJECTION);
        data->glLoadIdentity();
        data->glOrthof((GLfloat) 0,
                 (GLfloat) renderer->viewport.w,
                 (GLfloat) renderer->viewport.h,
                 (GLfloat) 0, 0.0, 1.0);
    }
    return 0;
}

static int
GLES_UpdateClipRect(SDL_Renderer * renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    const SDL_Rect *rect = &renderer->clip_rect;

    if (SDL_CurrentContext != data->context) {
        /* We'll update the clip rect after we rebind the context */
        return 0;
    }

    if (!SDL_RectEmpty(rect)) {
        data->glEnable(GL_SCISSOR_TEST);
        data->glScissor(rect->x, renderer->viewport.h - rect->y - rect->h, rect->w, rect->h);
    } else {
        data->glDisable(GL_SCISSOR_TEST);
    }
    return 0;
}

static void
GLES_SetColor(GLES_RenderData * data, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    Uint32 color = ((a << 24) | (r << 16) | (g << 8) | b);

    if (color != data->current.color) {
        data->glColor4f((GLfloat) r * inv255f,
                        (GLfloat) g * inv255f,
                        (GLfloat) b * inv255f,
                        (GLfloat) a * inv255f);
        data->current.color = color;
    }
}

static void
GLES_SetBlendMode(GLES_RenderData * data, int blendMode)
{
    if (blendMode != data->current.blendMode) {
        switch (blendMode) {
        case SDL_BLENDMODE_NONE:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            data->glDisable(GL_BLEND);
            break;
        case SDL_BLENDMODE_BLEND:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            data->glEnable(GL_BLEND);
            if (data->GL_OES_blend_func_separate_supported) {
                data->glBlendFuncSeparateOES(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            } else {
                data->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            break;
        case SDL_BLENDMODE_ADD:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            data->glEnable(GL_BLEND);
            if (data->GL_OES_blend_func_separate_supported) {
                data->glBlendFuncSeparateOES(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
            } else {
                data->glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }
            break;
        case SDL_BLENDMODE_MOD:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            data->glEnable(GL_BLEND);
            if (data->GL_OES_blend_func_separate_supported) {
                data->glBlendFuncSeparateOES(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
            } else {
                data->glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            }
            break;
        }
        data->current.blendMode = blendMode;
    }
}

static void
GLES_SetTexCoords(GLES_RenderData * data, SDL_bool enabled)
{
    if (enabled != data->current.tex_coords) {
        if (enabled) {
            data->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        } else {
            data->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        data->current.tex_coords = enabled;
    }
}

static void
GLES_SetDrawingState(SDL_Renderer * renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    GLES_ActivateRenderer(renderer);

    GLES_SetColor(data, (GLfloat) renderer->r,
                        (GLfloat) renderer->g,
                        (GLfloat) renderer->b,
                        (GLfloat) renderer->a);

    GLES_SetBlendMode(data, renderer->blendMode);

    GLES_SetTexCoords(data, SDL_FALSE);
}

static int
GLES_RenderClear(SDL_Renderer * renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    GLES_ActivateRenderer(renderer);

    data->glClearColor((GLfloat) renderer->r * inv255f,
                 (GLfloat) renderer->g * inv255f,
                 (GLfloat) renderer->b * inv255f,
                 (GLfloat) renderer->a * inv255f);

    data->glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}

static int
GLES_RenderDrawPoints(SDL_Renderer * renderer, const SDL_FPoint * points,
                      int count)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    GLES_SetDrawingState(renderer);

    data->glVertexPointer(2, GL_FLOAT, 0, points);
    data->glDrawArrays(GL_POINTS, 0, count);

    return 0;
}

static int
GLES_RenderDrawLines(SDL_Renderer * renderer, const SDL_FPoint * points,
                     int count)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    GLES_SetDrawingState(renderer);

    data->glVertexPointer(2, GL_FLOAT, 0, points);
    if (count > 2 &&
        points[0].x == points[count-1].x && points[0].y == points[count-1].y) {
        /* GL_LINE_LOOP takes care of the final segment */
        --count;
        data->glDrawArrays(GL_LINE_LOOP, 0, count);
    } else {
        data->glDrawArrays(GL_LINE_STRIP, 0, count);
        /* We need to close the endpoint of the line */
        data->glDrawArrays(GL_POINTS, count-1, 1);
    }

    return 0;
}

static int
GLES_RenderFillRects(SDL_Renderer * renderer, const SDL_FRect * rects,
                     int count)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    int i;

    GLES_SetDrawingState(renderer);

    for (i = 0; i < count; ++i) {
        const SDL_FRect *rect = &rects[i];
        GLfloat minx = rect->x;
        GLfloat maxx = rect->x + rect->w;
        GLfloat miny = rect->y;
        GLfloat maxy = rect->y + rect->h;
        GLfloat vertices[8];
        vertices[0] = minx;
        vertices[1] = miny;
        vertices[2] = maxx;
        vertices[3] = miny;
        vertices[4] = minx;
        vertices[5] = maxy;
        vertices[6] = maxx;
        vertices[7] = maxy;

        data->glVertexPointer(2, GL_FLOAT, 0, vertices);
        data->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    return 0;
}

static int
GLES_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                const SDL_Rect * srcrect, const SDL_FRect * dstrect)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *texturedata = (GLES_TextureData *) texture->driverdata;
    GLfloat minx, miny, maxx, maxy;
    GLfloat minu, maxu, minv, maxv;
    GLfloat vertices[8];
    GLfloat texCoords[8];

    GLES_ActivateRenderer(renderer);

    data->glEnable(GL_TEXTURE_2D);

    data->glBindTexture(texturedata->type, texturedata->texture);

    if (texture->modMode) {
        GLES_SetColor(data, texture->r, texture->g, texture->b, texture->a);
    } else {
        GLES_SetColor(data, 255, 255, 255, 255);
    }

    GLES_SetBlendMode(data, texture->blendMode);

    GLES_SetTexCoords(data, SDL_TRUE);

    if (data->GL_OES_draw_texture_supported && data->useDrawTexture) {
        /* this code is a little funny because the viewport is upside down vs SDL's coordinate system */
        GLint cropRect[4];
        int w, h;
        SDL_Window *window = renderer->window;

        SDL_GetWindowSize(window, &w, &h);
        if (renderer->target) {
            cropRect[0] = srcrect->x;
            cropRect[1] = srcrect->y;
            cropRect[2] = srcrect->w;
            cropRect[3] = srcrect->h;
            data->glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES,
                                   cropRect);
            data->glDrawTexfOES(renderer->viewport.x + dstrect->x, renderer->viewport.y + dstrect->y, 0,
                                dstrect->w, dstrect->h);
        } else {
            cropRect[0] = srcrect->x;
            cropRect[1] = srcrect->y + srcrect->h;
            cropRect[2] = srcrect->w;
            cropRect[3] = -srcrect->h;
            data->glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES,
                                   cropRect);
            data->glDrawTexfOES(renderer->viewport.x + dstrect->x,
                        h - (renderer->viewport.y + dstrect->y) - dstrect->h, 0,
                        dstrect->w, dstrect->h);
        }
    } else {

        minx = dstrect->x;
        miny = dstrect->y;
        maxx = dstrect->x + dstrect->w;
        maxy = dstrect->y + dstrect->h;

        minu = (GLfloat) srcrect->x / texture->w;
        minu *= texturedata->texw;
        maxu = (GLfloat) (srcrect->x + srcrect->w) / texture->w;
        maxu *= texturedata->texw;
        minv = (GLfloat) srcrect->y / texture->h;
        minv *= texturedata->texh;
        maxv = (GLfloat) (srcrect->y + srcrect->h) / texture->h;
        maxv *= texturedata->texh;

        vertices[0] = minx;
        vertices[1] = miny;
        vertices[2] = maxx;
        vertices[3] = miny;
        vertices[4] = minx;
        vertices[5] = maxy;
        vertices[6] = maxx;
        vertices[7] = maxy;

        texCoords[0] = minu;
        texCoords[1] = minv;
        texCoords[2] = maxu;
        texCoords[3] = minv;
        texCoords[4] = minu;
        texCoords[5] = maxv;
        texCoords[6] = maxu;
        texCoords[7] = maxv;

        data->glVertexPointer(2, GL_FLOAT, 0, vertices);
        data->glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
        data->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    data->glDisable(GL_TEXTURE_2D);

    return 0;
}

static int
GLES_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture,
                const SDL_Rect * srcrect, const SDL_FRect * dstrect,
                const double angle, const SDL_FPoint *center, const SDL_RendererFlip flip)
{

    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *texturedata = (GLES_TextureData *) texture->driverdata;
    GLfloat minx, miny, maxx, maxy;
    GLfloat minu, maxu, minv, maxv;
    GLfloat centerx, centery;
    GLfloat vertices[8];
    GLfloat texCoords[8];


    GLES_ActivateRenderer(renderer);

    data->glEnable(GL_TEXTURE_2D);

    data->glBindTexture(texturedata->type, texturedata->texture);

    if (texture->modMode) {
        GLES_SetColor(data, texture->r, texture->g, texture->b, texture->a);
    } else {
        GLES_SetColor(data, 255, 255, 255, 255);
    }

    GLES_SetBlendMode(data, texture->blendMode);

    GLES_SetTexCoords(data, SDL_TRUE);

    centerx = center->x;
    centery = center->y;

    /* Rotate and translate */
    data->glPushMatrix();
    data->glTranslatef(dstrect->x + centerx, dstrect->y + centery, 0.0f);
    data->glRotatef((GLfloat)angle, 0.0f, 0.0f, 1.0f);

    if (flip & SDL_FLIP_HORIZONTAL) {
        minx =  dstrect->w - centerx;
        maxx = -centerx;
    } else {
        minx = -centerx;
        maxx = dstrect->w - centerx;
    }

    if (flip & SDL_FLIP_VERTICAL) {
        miny = dstrect->h - centery;
        maxy = -centery;
    } else {
        miny = -centery;
        maxy = dstrect->h - centery;
    }

    minu = (GLfloat) srcrect->x / texture->w;
    minu *= texturedata->texw;
    maxu = (GLfloat) (srcrect->x + srcrect->w) / texture->w;
    maxu *= texturedata->texw;
    minv = (GLfloat) srcrect->y / texture->h;
    minv *= texturedata->texh;
    maxv = (GLfloat) (srcrect->y + srcrect->h) / texture->h;
    maxv *= texturedata->texh;

    vertices[0] = minx;
    vertices[1] = miny;
    vertices[2] = maxx;
    vertices[3] = miny;
    vertices[4] = minx;
    vertices[5] = maxy;
    vertices[6] = maxx;
    vertices[7] = maxy;

    texCoords[0] = minu;
    texCoords[1] = minv;
    texCoords[2] = maxu;
    texCoords[3] = minv;
    texCoords[4] = minu;
    texCoords[5] = maxv;
    texCoords[6] = maxu;
    texCoords[7] = maxv;
    data->glVertexPointer(2, GL_FLOAT, 0, vertices);
    data->glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    data->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    data->glPopMatrix();
    data->glDisable(GL_TEXTURE_2D);

    return 0;
}

static int
GLES_RenderReadPixels(SDL_Renderer * renderer, const SDL_Rect * rect,
                    Uint32 pixel_format, void * pixels, int pitch)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    Uint32 temp_format = SDL_PIXELFORMAT_ABGR8888;
    void *temp_pixels;
    int temp_pitch;
    Uint8 *src, *dst, *tmp;
    int w, h, length, rows;
    int status;

    GLES_ActivateRenderer(renderer);

    temp_pitch = rect->w * SDL_BYTESPERPIXEL(temp_format);
    temp_pixels = SDL_malloc(rect->h * temp_pitch);
    if (!temp_pixels) {
        return SDL_OutOfMemory();
    }

    SDL_GetRendererOutputSize(renderer, &w, &h);

    data->glPixelStorei(GL_PACK_ALIGNMENT, 1);

    data->glReadPixels(rect->x, (h-rect->y)-rect->h, rect->w, rect->h,
                       GL_RGBA, GL_UNSIGNED_BYTE, temp_pixels);

    /* Flip the rows to be top-down */
    length = rect->w * SDL_BYTESPERPIXEL(temp_format);
    src = (Uint8*)temp_pixels + (rect->h-1)*temp_pitch;
    dst = (Uint8*)temp_pixels;
    tmp = SDL_stack_alloc(Uint8, length);
    rows = rect->h / 2;
    while (rows--) {
        SDL_memcpy(tmp, dst, length);
        SDL_memcpy(dst, src, length);
        SDL_memcpy(src, tmp, length);
        dst += temp_pitch;
        src -= temp_pitch;
    }
    SDL_stack_free(tmp);

    status = SDL_ConvertPixels(rect->w, rect->h,
                               temp_format, temp_pixels, temp_pitch,
                               pixel_format, pixels, pitch);
    SDL_free(temp_pixels);

    return status;
}

static void
GLES_RenderPresent(SDL_Renderer * renderer)
{
    GLES_ActivateRenderer(renderer);

    SDL_GL_SwapWindow(renderer->window);
}

static void
GLES_DestroyTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    GLES_RenderData *renderdata = (GLES_RenderData *) renderer->driverdata;

    GLES_TextureData *data = (GLES_TextureData *) texture->driverdata;

    GLES_ActivateRenderer(renderer);

    if (!data) {
        return;
    }
    if (data->texture) {
        renderdata->glDeleteTextures(1, &data->texture);
    }
    SDL_free(data->pixels);
    SDL_free(data);
    texture->driverdata = NULL;
}

static void
GLES_DestroyRenderer(SDL_Renderer * renderer)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;

    if (data) {
        if (data->context) {
            while (data->framebuffers) {
               GLES_FBOList *nextnode = data->framebuffers->next;
               data->glDeleteFramebuffersOES(1, &data->framebuffers->FBO);
               SDL_free(data->framebuffers);
               data->framebuffers = nextnode;
            }
            SDL_GL_DeleteContext(data->context);
        }
        SDL_free(data);
    }
    SDL_free(renderer);
}

static int GLES_BindTexture (SDL_Renderer * renderer, SDL_Texture *texture, float *texw, float *texh)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *texturedata = (GLES_TextureData *) texture->driverdata;
    GLES_ActivateRenderer(renderer);

    data->glEnable(GL_TEXTURE_2D);
    data->glBindTexture(texturedata->type, texturedata->texture);

    if(texw) *texw = (float)texturedata->texw;
    if(texh) *texh = (float)texturedata->texh;

    return 0;
}

static int GLES_UnbindTexture (SDL_Renderer * renderer, SDL_Texture *texture)
{
    GLES_RenderData *data = (GLES_RenderData *) renderer->driverdata;
    GLES_TextureData *texturedata = (GLES_TextureData *) texture->driverdata;
    GLES_ActivateRenderer(renderer);
    data->glDisable(texturedata->type);

    return 0;
}

#endif /* SDL_VIDEO_RENDER_OGL_ES && !SDL_RENDER_DISABLED */

/* vi: set ts=4 sw=4 expandtab: */
