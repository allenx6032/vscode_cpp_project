#include "window.h"
#include <stdlib.h>
#include <stdio.h>
#include <glad.h>

void APIENTRY gl_debug_proc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
{
    (void) source;
    (void) id;
    (void) severity;
    (void) length;
    (void) user_param;

    if (type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "%s", message);
        exit(1);
    }
}


#include "plat.h"
#ifdef LINUX
#ifdef OS_LINUX
#include "window.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <glad.h>
#include <GL/gl.h>
#include <GL/glx.h>
#define GLX_GLXEXT_PROTOTYPES
#include <glxext.h>

struct wnd_internal
{
    Display* display;
    GLXContext glx_ctx;
    Colormap cmap;
    Window wnd;
};

static GLXFBConfig pick_fb_config(Display* display)
{
    static const int visual_attribs[] = {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , True,
        /*
        GLX_SAMPLE_BUFFERS  , 1,
        GLX_SAMPLES         , 4,
         */
        None
    };

    /* Get matching FB configs */
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (!fbc)
        return 0;

    /* Pick the FB config/visual with the most samples per pixel that also has an alpha channel */
    int best_fbc_idx = -1, best_num_samp = -1;
    for (int i = 0; i < fbcount; ++i) {
        XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[i]);
        if (vi) {
            /* Check for alpha channel first */
            XRenderPictFormat* pict_format = XRenderFindVisualFormat(display, vi->visual);
            if(pict_format && (pict_format->direct.alphaMask > 0)) {
                /* Get sample info */
                int samp_buf, samples;
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);
                /* Check if this fbc is better than the current set one */
                if (best_fbc_idx < 0 || (samp_buf && (samples > best_num_samp))) {
                    best_fbc_idx = i;
                    best_num_samp = samples;
                }
            }
        }
        XFree(vi);
    }

    /* Returned value */
    GLXFBConfig best_fbc = fbc[best_fbc_idx];

    /* Free the FBConfig list allocated by glXChooseFBConfig() */
    XFree(fbc);

    return best_fbc;
}

/* Helper to check for extension string presence.
   Adapted from: http://www.opengl.org/resources/features/OGLextensions/ */
static int is_extension_supported(const char* ext_list, const char* extension)
{
    const char* start;
    const char* where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;

    /* It takes a bit of care to be fool-proof about parsing the
       OpenGL extensions string. Don't be fooled by sub-strings, etc. */
    for (start = ext_list;;) {
        where = strstr(start, extension);
        if (!where)
            break;

        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return 1;

        start = terminator;
    }

    return 0;
}

static int ctx_error_occurred = 0;
static int ctx_error_handler(Display* dpy, XErrorEvent* ev)
{
    (void) dpy;
    (void) ev;
    ctx_error_occurred = 1;
    return 0;
}

static GLXContext create_glx_context(Display* display, GLXFBConfig fbc, int ver_maj, int ver_min)
{
    /* Get the default screen's GLX extension list */
    const char* glx_exts = glXQueryExtensionsString(display, DefaultScreen(display));

    /* NOTE: It is not necessary to create or make current to a context before
       calling glXGetProcAddressARB */
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
        glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    /* Install an X error handler so the application won't exit if GL 3.0
       context allocation fails.
       Note this error handler is global.  All display connections in all threads
       of a process use the same error handler, so be sure to guard against other
       threads issuing X commands while this code is running. */
    ctx_error_occurred = 0;
    int (*old_handler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctx_error_handler);

    /* Check for the GLX_ARB_create_context extension string and the function.
       If either is not present, use GLX 1.3 context creation method. */
    GLXContext glx_ctx = 0;
    if (!is_extension_supported(glx_exts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
        printf("glXCreateContextAttribsARB() not found ... using old-style GLX context\n");
        glx_ctx = glXCreateNewContext(display, fbc, GLX_RGBA_TYPE, 0, True);
    } else {
        /* If it does, try to get a GL 3.+ context! */
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, ver_maj,
            GLX_CONTEXT_MINOR_VERSION_ARB, ver_min,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
            /* GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, */
            None
        };
        glx_ctx = glXCreateContextAttribsARB(display, fbc, 0, True, context_attribs);

        /* Sync to ensure any errors generated are processed. */
        XSync(display, False);
        if (ctx_error_occurred || !glx_ctx) {
            /* Couldn't create GL 3.+ context. Fall back to old-style 2.x context.
               When a context version below 3.+ is requested, implementations will
               return the newest context version compatible with OpenGL versions less
               than version 3.+. */
            printf("Failed to create GL %d.%d context ... using old-style GLX context\n",
                    context_attribs[1], context_attribs[3]);
            context_attribs[1] = 1; /* GLX_CONTEXT_MAJOR_VERSION_ARB = 1 */
            context_attribs[3] = 0; /* GLX_CONTEXT_MINOR_VERSION_ARB = 0 */
            ctx_error_occurred = 0;
            glx_ctx = glXCreateContextAttribsARB(display, fbc, 0, True, context_attribs);
        }
    }

    /* Sync to ensure any errors generated are processed. */
    XSync(display, False);

    /* Restore the original error handler */
    XSetErrorHandler(old_handler);

    return glx_ctx;
}

static int create_window(struct window* w, GLXFBConfig fbc, int width, int height, const char* title)
{
    Display* display = w->internal->display;
    /* Get a visual */
    XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc);
    /* Create colormap */
    Window rwnd = RootWindow(display, vi->screen);
    Colormap cmap = XCreateColormap(display, rwnd, vi->visual, AllocNone);

    /* Create window */
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask|KeyReleaseMask;
    Window wnd = XCreateWindow(
        display,
        rwnd,
        0, 0, width, height, 0,
        vi->depth, InputOutput,
        vi->visual,
        CWBorderPixel|CWColormap|CWEventMask,
        &swa
    );
    if (!wnd)
        goto fail_cleanup;

    /* Done with the visual info data */
    XFree(vi);

    /*
     * Make it floating non-resizable window
     */
    /* Set size hints */
    XSizeHints* hints = XAllocSizeHints();
    hints->flags |= (PMinSize | PMaxSize);
    hints->min_width  = hints->max_width  = width;
    hints->min_height = hints->max_height = height;
    hints->flags |= PWinGravity;
    hints->win_gravity = StaticGravity;
    XSetWMNormalHints(display, wnd, hints);
    XFree(hints);
    /* Resize */
    XResizeWindow(display, wnd, width, height);
    XFlush(display);

    /* Set title */
    XStoreName(display, wnd, title);
    /* Map window */
    XMapWindow(display, wnd);

    /* Store handles */
    w->internal->wnd = wnd;
    w->internal->cmap = cmap;
    return 1;

fail_cleanup:
    XFree(vi);
    XFreeColormap(display, w->internal->cmap);
    return 0;
}

static void window_release(struct window* w)
{
    Display* display = w->internal->display;
    glXMakeCurrent(display, 0, 0);

    glXDestroyContext(display, w->internal->glx_ctx);
    XDestroyWindow(display, w->internal->wnd);
    XFreeColormap(display, w->internal->cmap);
    XCloseDisplay(display);

    free(w->internal);
    w->internal = 0;
}

static void window_swap_buffers(struct window* w)
{
    glXSwapBuffers(w->internal->display, w->internal->wnd);
}

void window_open(struct window* w)
{
    /* Create internal struct */
    w->internal = calloc(1, sizeof(struct wnd_internal));

    /* Open display */
    Display* display = XOpenDisplay(0);
    if (!display) {
        printf("Failed to open X display\n");
        return;
    }
    w->internal->display = display;

    /* FBConfigs were added in GLX version 1.3. */
    int glx_major, glx_minor;
    if (!glXQueryVersion(display, &glx_major, &glx_minor)
        || (glx_major == 1 && glx_minor < 3) || glx_major < 1) {
        printf("Invalid GLX version");
        return;
    }

    /* Pick most appropriate framebuffer config */
    GLXFBConfig fbc = pick_fb_config(display);
    if (!fbc) {
        printf("Failed to retrieve a framebuffer config\n");
        return;
    }

    /* Create window */
    int result = create_window(w, fbc, w->width, w->height, w->title);
    if (!result) {
        printf("Failed to create window.\n");
        return;
    }

    /* Create context and make current */
    int ver_maj = 3, ver_min = 3;
    GLXContext glx_ctx = create_glx_context(display, fbc, ver_maj, ver_min);
    if (ctx_error_occurred || !glx_ctx) {
        printf("Failed to create an OpenGL context\n");
        exit(1);
    }
    glXMakeCurrent(display, w->internal->wnd, glx_ctx);
    w->internal->glx_ctx = glx_ctx;

    /* Verifying that context is a direct context */
    //printf("%s GLX rendering context obtained\n", !glXIsDirect(display, glx_ctx) ? "Indirect" : "Direct");

    /* Load OpenGL function ptrs */
    gladLoadGL();

    /* Register debug callback */
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_proc, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
}

void window_loop(struct window* w)
{
    Display* display = w->internal->display;
    while (!w->should_close) {
        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);
            switch (event.type) {
                case KeyRelease: {
                    KeySym key_symbol = XLookupKeysym(&event.xkey, 0);
                    if (key_symbol == XK_Escape)
                        w->should_close = 1;
                    break;
                }
            }
        }

        w->render_fn(w->render_data);
        window_swap_buffers(w);
    }
    window_release(w);
}
#endif
#else
#ifdef OS_WINDOWS
#include "window.h"
#include <windows.h>
#include <stdlib.h>
#include <assert.h>
#include <glad.h>
#include "wglctx.h"
#include <stdio.h>

/*----------------------------------------------------------------------
 * Platform specific data
 *----------------------------------------------------------------------*/
/* Window positioning state */
struct wnd_pos_state
{
    int is_moving;
    POINT mpos;
};

/* Window internal data */
struct wnd_internal
{
    /* Main state */
    HWND hwnd;       /* The window handle */
    /* Positioning state */
    struct wnd_pos_state ps;
    /* Rendering window */
    HWND rhwnd;
    HDC rhdc;        /* The window device context */
    HGLRC context;   /* The opengl context handle */
};

/*----------------------------------------------------------------------
 * Rendering window
 *----------------------------------------------------------------------*/
/* The window class unique identifier */
static const char* rendering_window_class_name = "rendering_window";
static const char* rendering_window_name = "";

static LRESULT CALLBACK rendering_window_callback_func(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    switch (mm) {
        case WM_SIZE: {
            int w = LOWORD(ll);
            int h = HIWORD(ll);
            glViewport(0, 0, w, h);
            break;
        }
        default:
            return DefWindowProc(hh, mm, ww, ll);
    }
    return 0;
}

static void register_rendering_window_class()
{
    /* The window class information structure */
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(WNDCLASSEX));

    /* Fill class info structure */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.hInstance = GetModuleHandle(0);
    wc.lpfnWndProc = rendering_window_callback_func;
    wc.lpszClassName = rendering_window_class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 0;

    /* Register the window class */
    RegisterClassEx(&wc);
}

static HWND create_rendering_window()
{
    /* The window handle */
    HWND hwnd;

    /* Register the window class */
    register_rendering_window_class();

    /* Styles */
    int style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    int exstyle = WS_EX_APPWINDOW;

    /* Create the Window */
    hwnd = CreateWindowExA(
        exstyle,                     /* dwExStyle    */
        rendering_window_class_name, /* lpClassName  */
        rendering_window_name,       /* lpWindowName */
        style,                       /* dwStyle      */
        CW_USEDEFAULT,               /* x            */
        CW_USEDEFAULT,               /* y            */
        0,                           /* nWidth       */
        0,                           /* nHeight      */
        0,                           /* nWndParent   */
        0,                           /* nMenu        */
        GetModuleHandle(0),          /* hInstance    */
        0                            /* lpParam      */
    );

    assert(hwnd);
    return hwnd;
}

/*----------------------------------------------------------------------
 * Actual window
 *----------------------------------------------------------------------*/
/* The window class unique identifier */
static const char* window_class_name = "launcher_window";

static LRESULT CALLBACK window_callback_func(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    struct window* wnd = (struct window*) GetWindowLongPtr(hh, GWLP_USERDATA);
    switch (mm) {
        case WM_CREATE: {
            struct window* window = (struct window*) ((CREATESTRUCT*)ll)->lpCreateParams;
            SetWindowLongPtr(hh, GWLP_USERDATA, (LONG_PTR) window);

            /* Remove borders and stuff */
            /*
            SetWindowLongPtr(hh, GWL_STYLE, 0);
            ShowWindow(hh, SW_SHOW);
            UpdateWindow(hh);
            */

            break;
        }
        case WM_LBUTTONDOWN: {
            if ((GetKeyState(VK_LMENU) & 0x8000) && (ww & MK_LBUTTON)) {
                wnd->internal->ps.is_moving = 1;
                SetCapture(hh);
                POINT cursor;
                memset(&cursor, 0, sizeof(POINT));
                GetCursorPos(&cursor);
                wnd->internal->ps.mpos = cursor;
            }
            break;
        }
        case WM_LBUTTONUP: {
            wnd->internal->ps.is_moving = 0;
            ReleaseCapture();
            memset(&wnd->internal->ps.mpos, 0, sizeof(POINT));
            break;
        }
        case WM_MOUSEMOVE: {
            if (wnd->internal->ps.is_moving) {
                POINT prevpos = wnd->internal->ps.mpos;
                POINT curpos;
                memset(&curpos, 0, sizeof(POINT));
                GetCursorPos(&curpos);

                RECT wrect;
                memset(&wrect, 0, sizeof(RECT));
                GetWindowRect(hh, &wrect);

                int xDiff = curpos.x - prevpos.x;
                int yDiff = curpos.y - prevpos.y;

                SetWindowPos(hh, NULL,
                    wrect.left + xDiff,
                    wrect.top + yDiff,
                    0, 0, SWP_NOSIZE | SWP_SHOWWINDOW
                );
                wnd->internal->ps.mpos = curpos;
            }
            break;
        }
        case WM_KEYDOWN: {
            if (ww == VK_ESCAPE)
                wnd->should_close = 1;
            break;
        }
        case WM_KEYUP: {
            switch (ww) {
                case VK_LMENU: {
                    wnd->internal->ps.is_moving = 0;
                    ReleaseCapture();
                    memset(&wnd->internal->ps.mpos, 0, sizeof(POINT));
                    break;
                }
            }
            break;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_SIZE: {
            int nw = LOWORD(ll);
            int nh = HIWORD(ll);
            RECT rect = { 0, 0, nw, nh };
            AdjustWindowRectEx(
                &rect,
                GetWindowLongPtr(wnd->internal->rhwnd, GWL_STYLE),
                0,
                GetWindowLongPtr(wnd->internal->rhwnd, GWL_EXSTYLE));
            SetWindowPos(wnd->internal->rhwnd, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOREPOSITION | SWP_NOZORDER);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hh, &ps);

            /* Gather window dimensions */
            RECT rect;
            GetWindowRect(hh, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            /* Create memory DC */
            HDC hdc_mem = CreateCompatibleDC(0);

            /* Create DIB */
            void* pdata;
            BITMAPINFOHEADER bih;
            memset(&bih, 0, sizeof(BITMAPINFOHEADER));
            bih.biSize = sizeof(BITMAPINFOHEADER);
            bih.biWidth = width;
            bih.biHeight = height;
            bih.biPlanes = 1;
            bih.biBitCount = 32;
            bih.biCompression = BI_RGB;
            HBITMAP hbmp = CreateDIBSection(hdc_mem, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &pdata, 0, 0);
            SelectObject(hdc_mem, hbmp);

            /* Gather image from GPU into temporary buffer */
            unsigned char* tmp_img = malloc(width * height * 4);
            memset(tmp_img, 0, width * height * 4);
            glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, tmp_img);
            /* Copy it to DIB flipped */
            for (int i = 0; i <  height; ++i) {
                for (int j = 0; j < width; ++j) {
                    unsigned char* src = (unsigned char*)tmp_img + ((height - i) * width + j) * 4;
                    unsigned char* dst = (unsigned char*)pdata + (i * width + j) * 4;
                    memcpy(dst, src, 4);
                }
            }
            /* Free temporary buffer */
            free(tmp_img);

            /* Blend transparent window */
            POINT pt_origin = { 0, 0 };
            SIZE sz = { width, height };
            BLENDFUNCTION blend;
            memset(&blend, 0, sizeof(BLENDFUNCTION));
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;
            UpdateLayeredWindow(hh, hdc, 0, &sz, hdc_mem, &pt_origin, RGB(0, 0, 0), &blend, ULW_ALPHA);

            /* Free resources */
            DeleteDC(hdc_mem);
            DeleteObject(hbmp);
            ReleaseDC(hh, hdc);
            EndPaint(hh, &ps);
            break;
        }
        case WM_CLOSE: {
            wnd->should_close = 1;
            break;
        }
        default:
            return DefWindowProc(hh, mm, ww, ll);
    }
    return 0;
}

static void register_window_class()
{
    /* The window class information structure */
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(WNDCLASSEX));

    /* Fill class info structure */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = window_callback_func;
    wc.lpszClassName = window_class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 0;

    /* Register the window class */
    RegisterClassEx(&wc);
}

static HWND create_window(struct window* wnd)
{
    /* The window handle */
    HWND hwnd;

    /* Register the window class */
    register_window_class();

    /* Create the Window */
    hwnd = CreateWindowExA(
        WS_EX_LAYERED,         /* dwExStyle    */
        window_class_name,     /* lpClassName  */
        wnd->title,            /* lpWindowName */
        WS_POPUP | WS_VISIBLE, /* dwStyle      */
        CW_USEDEFAULT,         /* x            */
        CW_USEDEFAULT,         /* y            */
        wnd->width,            /* nWidth       */
        wnd->height,           /* nHeight      */
        0,                     /* nWndParent   */
        0,                     /* nMenu        */
        GetModuleHandle(0),    /* hInstance    */
        wnd                    /* lpParam      */
    );

    assert(hwnd);
    return hwnd;
}

static void poll_window_events(struct window* window)
{
    /* Window message var */
    MSG msg;
    /* Peek message loop, poll for events */
    while(PeekMessage(&msg, window->internal->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    while(PeekMessage(&msg, window->internal->rhwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

static void window_release(struct window* wnd)
{
    /* Release rendering window resources */
    DestroyWindow(wnd->internal->hwnd);
    wglMakeCurrent(wnd->internal->rhdc, 0);
    wglDeleteContext(wnd->internal->context);
    ReleaseDC(wnd->internal->rhwnd, wnd->internal->rhdc);
    DestroyWindow(wnd->internal->rhwnd);
    /* Release internal data struct */
    free(wnd->internal);
    wnd->internal = 0;
}

static long long get_timer_value()
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return count.QuadPart;
}

static void sleep(long msec)
{
    Sleep(msec);
}

#define FPS 25
typedef long long time_val_t;

void window_loop(struct window* wnd)
{
    time_val_t t1 = get_timer_value();
    while(!wnd->should_close) {
        time_val_t t2 = get_timer_value();
        if (t2 - t1 >= 1000 / FPS) {
            wnd->render_fn(wnd->render_data);
            SwapBuffers(wnd->internal->rhdc);
            UpdateWindow(wnd->internal->hwnd);
            RedrawWindow(wnd->internal->hwnd, 0, 0, RDW_ERASE | RDW_INTERNALPAINT | RDW_INVALIDATE);
            poll_window_events(wnd);
        }
        else
            sleep((long)((1000 / FPS) - (t2 - t1)));
        t1 = t2;
    }
    window_release(wnd);
}

void window_open(struct window* window)
{
    /* Create internal struct */
    window->internal = calloc(1, sizeof(struct wnd_internal));

    /* Create rendering winodw instance */
    window->internal->rhwnd = create_rendering_window();

    /* Create opengl context */
    create_wgl_context(
        window->internal->rhwnd,
        &window->internal->rhdc,
        &window->internal->context,
        3, 3
    );

    /* Register debug callback */
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_proc, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }

    /* Create the window instance */
    window->internal->hwnd = create_window(window);
}
#endif
#endif