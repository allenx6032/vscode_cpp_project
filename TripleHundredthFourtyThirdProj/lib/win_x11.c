
#include <GL/gl.h>
#include <stdint.h>
#ifdef LF_X11
#include <X11/X.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <assert.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include "../../include/leif/win.h"
#include "../../include/leif/ui_core.h"
#include "../../include/leif/event.h"
#include "../../include/leif/widget.h"
#include "../../include/leif/util.h"

#define MAX_WINDOWS 16

typedef struct {
  lf_win_mouse_press_func ev_mouse_press_cb;
  lf_win_mouse_release_func ev_mouse_release_cb;
  lf_win_refresh_func ev_refresh_cb;
  lf_win_resize_func ev_resize_cb;
  lf_win_close_func ev_close_cb;
  lf_win_mouse_move_func ev_move_cb;
  Window win;
  int32_t win_width, win_height;
  lf_ui_state_t* ui;
  GLXContext glcontext;
} window_callbacks_t;

typedef struct {
    Visual *visual;
    VisualID visualid;
    int screen;
    unsigned int depth;
    int class;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    int colormap_size;
  int bits_per_rgb;
} visual_info_t;

static lf_windowing_event_func windowing_event_cb = NULL;

static Display *display = NULL;
static window_callbacks_t window_callbacks[MAX_WINDOWS];
static uint32_t n_windows = 0;
static lf_event_type_t current_event = WinEventNone;
static Atom wm_protocols_atom, wm_delete_window_atom, motif_wm_hints;
static GLXContext share_gl_context = 0;

static int last_mouse_x = 0;
static int last_mouse_y = 0;

static window_callbacks_t* win_data_from_native(lf_window_t win);

static void handle_event(XEvent *event);

static lf_window_t create_window(uint32_t width, uint32_t height, const char* title, uint32_t flags, lf_windowing_hint_kv_t* hints, uint32_t nhints);

void get_window_size(Display* display, Window window, int32_t* width, int32_t* height) {
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, window, &attrs)) {
        *width = attrs.width;
        *height = attrs.height;
    } else {
    assert(false && "reif: failed to get window attributes\n");
    }
}

window_callbacks_t* win_data_from_native(lf_window_t win) {
  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].win == win) {
      return &window_callbacks[i];
    }
  }
  return NULL;
}

#include <time.h>

static struct timespec last_resize_time = {0, 0};  // Time of the last resize event
static const uint32_t resize_debounce_delay_ms = 50;  // Debounce delay in milliseconds
// Function to check time difference
bool is_resize_event_debounce_time_passed(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);  // Get the current time

    // Calculate the time difference in milliseconds
    uint32_t time_diff_ms = (now.tv_sec - last_resize_time.tv_sec) * 1000 + (now.tv_nsec - last_resize_time.tv_nsec) / 1000000;

    // If the time difference is less than the debounce delay, return false
    if (time_diff_ms < resize_debounce_delay_ms) {
        return false;
    }

    // Update the last resize event time to the current time
    last_resize_time = now;
    return true;
}
void 
handle_event(XEvent *event) {
  lf_event_t ev = {0};
  for (uint32_t i = 0; i < n_windows; ++i) {
      window_callbacks_t win_data = window_callbacks[i]; 
    if(event->xany.window != win_data.win) continue; 
      switch (event->type) {
        case Expose:
          ev.type = WinEventRefresh;
          current_event = ev.type;
          if(win_data.ui) { 
            lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
          }
          if (window_callbacks[i].ev_refresh_cb && win_data.ui) {
            window_callbacks[i].ev_refresh_cb(
              win_data.ui, 
              (lf_window_t)event->xany.window);
          }
          break;
        case ConfigureNotify:
          if (event->xconfigure.width == window_callbacks[i].win_width &&
            event->xconfigure.height == window_callbacks[i].win_height) {
            break;  // No need to process the event if the size is the same
          }

          // Get the actual size of the window from the X server to confirm the resize
          XWindowAttributes wa;
          XGetWindowAttributes(display, window_callbacks[i].win, &wa);

          // If the window's size has changed, handle the resize event
            printf("resize event on window %i: new size = %dx%d\n", i, wa.width, wa.height);

            // Create the resize event and update the current event type
            ev.type = WinEventResize;
            ev.width = wa.width;
            ev.height = wa.height;
            current_event = ev.type;

            // Update the window size
            window_callbacks[i].win_width = wa.width;
            window_callbacks[i].win_height = wa.height;

            // Call the widget event handler if applicable
            if (win_data.ui) {
          lf_win_make_gl_context(win_data.win);
              lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
            }

            // Call the resize callback if defined
            if (window_callbacks[i].ev_resize_cb && win_data.ui) {
              window_callbacks[i].ev_resize_cb(
                win_data.ui, 
                (lf_window_t)event->xany.window, 
                wa.width, wa.height
              );
            }
          break;
        case ButtonPress:
          if(event->xbutton.button == Button2) { // Scrollwheel press
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 0;
            ev.scroll_y = 0;
          }
          if(event->xbutton.button == Button4) { // Scrollwheel up
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 0;
            ev.scroll_y = 1;
          } else if(event->xbutton.button == Button5){ // Scrollwheel down
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 0;
            ev.scroll_y = -1;
          } else if(event->xbutton.button == 6){ // Scrollwheel left
            ev.type = WinEventMouseWheel;
            ev.scroll_x = -1;
            ev.scroll_y = 0;
          } else if(event->xbutton.button == 7){ // SCrollwheel right
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 1;
            ev.scroll_y = 0;
          } else {
            ev.type = WinEventMousePress;
          }
          ev.button = event->xbutton.button;

          if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
          if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
          ev.x = event->xbutton.x;
          ev.y = event->xbutton.y;
          ev.delta_x = last_mouse_x - event->xbutton.x;
          ev.delta_y = last_mouse_y - event->xbutton.y;
          last_mouse_x = event->xbutton.x;
          last_mouse_y = event->xbutton.y;

          current_event = ev.type;
          if(win_data.ui)
            lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
          if (window_callbacks[i].ev_mouse_press_cb && win_data.ui)
            window_callbacks[i].ev_mouse_press_cb(
              win_data.ui, 
              (lf_window_t)event->xany.window, 
              event->xbutton.button);
          break;
        case ButtonRelease:
          ev.button = event->xbutton.button;
          ev.type = WinEventMouseRelease; 
          if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
          if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
          ev.x = event->xbutton.x;
          ev.y = event->xbutton.y;
          ev.delta_x = last_mouse_x - event->xbutton.x;
          ev.delta_y = last_mouse_y - event->xbutton.y;
          last_mouse_x = event->xbutton.x;
          last_mouse_y = event->xbutton.y;

          current_event = ev.type;
          if(win_data.ui)
            lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
          if (window_callbacks[i].ev_mouse_release_cb && win_data.ui)
            window_callbacks[i].ev_mouse_release_cb(
              win_data.ui, 
              (lf_window_t)event->xany.window,
              event->xbutton.button);
          break;
        case MotionNotify:
          if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
          if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
          ev.x = (uint16_t)event->xmotion.x;
          ev.y = (uint16_t)event->xmotion.y;
          ev.delta_x = last_mouse_x - event->xmotion.x;
          ev.delta_y = last_mouse_y - event->xmotion.y;
          last_mouse_x = event->xmotion.x;
          last_mouse_y = event->xmotion.y;

          ev.type = WinEventMouseMove;
          current_event = ev.type; 
          if(win_data.ui)
            lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
          if (window_callbacks[i].ev_move_cb && win_data.ui)
            window_callbacks[i].ev_move_cb(
              win_data.ui, 
              (lf_window_t)event->xany.window, 
              event->xmotion.x, event->xmotion.y);
          break;
        case ClientMessage:
          if (event->xclient.message_type == wm_protocols_atom && 
            (Atom)event->xclient.data.l[0] == wm_delete_window_atom) {
            ev.type = WinEventClose;
            current_event = ev.type;
            if(win_data.ui)
              lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
            if (window_callbacks[i].ev_close_cb && win_data.ui) {
              window_callbacks[i].ev_close_cb(win_data.ui, (lf_window_t)event->xany.window);
            }
          glXDestroyContext(display, window_callbacks[i].glcontext);
          }
          break;
        case LeaveNotify: 
          {
            XWindowAttributes wa;
            XGetWindowAttributes(display, event->xcrossing.window, &wa);

            if (wa.override_redirect) {
              ev.x = -1; 
              ev.y = -1; 
              ev.delta_x = 0; 
              ev.delta_y = 0; 
              ev.type = WinEventMouseMove;
              current_event = ev.type; 
              if(win_data.ui)
                lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
              if (window_callbacks[i].ev_move_cb && win_data.ui)
                window_callbacks[i].ev_move_cb(
                    win_data.ui, 
                    (lf_window_t)event->xany.window, 
                    -1, -1);
            }
            break;
      }
    }
  }
}

lf_window_t
create_window(
    uint32_t width, 
  uint32_t height, 
  const char* title,
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints,
  uint32_t nhints) {
  Window root = DefaultRootWindow(display);

  uint32_t winpos_x = 0, winpos_y = 0;

  bool transparent_framebuffer, decorated = false;
  for(uint32_t i = 0; i < nhints; i++) {
    if(hints[i].key == LF_WINDOWING_HINT_TRANSPARENT_FRAMEBUFFER
    && hints[i].value == true) {
      transparent_framebuffer = true;
    } 
    if(hints[i].key == LF_WINDOWING_HINT_DECORATED 
    && hints[i].value == true) {
      decorated = true;
    } 
    if(hints[i].key == LF_WINDOWING_HINT_POS_X) {
      winpos_x = hints[i].value;
    }
    else if(hints[i].key == LF_WINDOWING_HINT_POS_Y) {
      winpos_y = hints[i].value;
    }
  }

  int screen_num = DefaultScreen(display);

  static int visdata[] = {
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_DOUBLEBUFFER, True,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_ALPHA_SIZE, 1,
    GLX_DEPTH_SIZE, 1,
    None
  };

  int32_t nfb;
  visual_info_t* visual = NULL;
  XRenderPictFormat* pic_fmt;
  GLXFBConfig fbconfig = NULL;  
  GLXFBConfig* fbconfigs = glXChooseFBConfig(display, screen_num, visdata, &nfb);
  for(int i = 0; i < nfb; i++) {
    visual = (visual_info_t*) glXGetVisualFromFBConfig(display, fbconfigs[i]);
    if(!visual)
      continue;

    pic_fmt = XRenderFindVisualFormat(display, visual->visual);
    if(!pic_fmt)
      continue;

    if(pic_fmt->direct.alphaMask > 0) {
      fbconfig = fbconfigs[i];
      break;
    }
  }
  if(!visual) return 0;
  if(!fbconfig) return 0;

  Colormap cmap = XCreateColormap(display, DefaultRootWindow(display), visual->visual, AllocNone);

  int dummy;
  if (!glXQueryExtension(display, &dummy, &dummy)) {
    fprintf(stderr, "reif: OpenGL not supported by X server.\n");
    assert(false);
  }
  GLXContext glcontext = glXCreateNewContext(display, fbconfig, GLX_RGBA_TYPE, share_gl_context, True);
  if(!share_gl_context)
    share_gl_context = glcontext;
  if (!glcontext) {
    fprintf(stderr, "reif: failed to create an OpenGL context.\n");
    assert(false);
  }


  XTextProperty textprop;
  XSizeHints size_hints;
  XWMHints* startup_state;
  Window win;
  if(transparent_framebuffer) {
    XSetWindowAttributes attr;
    if(transparent_framebuffer) {
      attr.colormap = cmap;
      attr.background_pixmap = None;
    }
    attr.border_pixel = 0;
    attr.event_mask =  
      StructureNotifyMask | KeyPressMask      | KeyReleaseMask    |
      ButtonPressMask     | ButtonReleaseMask | PointerMotionMask | 
      ExposureMask        | LeaveWindowMask;
    int32_t attr_mask;
    attr_mask =
      CWBackPixmap  |
      CWColormap    |
      CWBorderPixel |
      CWEventMask;   
    win = XCreateWindow(display, root, 
                               winpos_x, winpos_y, width, height, 1,
                               visual->depth, InputOutput,  
                               visual->visual, attr_mask, &attr); 
  } else {
    int screen = DefaultScreen(display);
    win = XCreateSimpleWindow(display, root, winpos_x, winpos_y, width, height, 0,
                              BlackPixel(display, screen), BlackPixel(display, screen));
  XSelectInput(display, win, StructureNotifyMask | KeyPressMask | KeyReleaseMask |
               ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | LeaveWindowMask);
  }
  textprop.value = (unsigned char*)title;
  textprop.encoding = XA_STRING;
  textprop.format = 8;
  textprop.nitems = strlen(title);

  size_hints.x = winpos_x;
  size_hints.y = winpos_y;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.flags = USPosition|USSize;

  startup_state = XAllocWMHints();
  startup_state->initial_state = NormalState;
  startup_state->flags = StateHint;

  XSetWMProperties(display, win, &textprop, &textprop,
                   NULL, 0,
                   &size_hints, 
                   startup_state,
                   NULL);

  XFree(startup_state);

  if(lf_flag_exists(&flags, LF_WINDOWING_X11_OVERRIDE_REDIRECT)) {
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(display, win, CWOverrideRedirect, &attributes);  
  }

  XMapWindow(display, win);

  struct
  {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
  } decoration_hints = {0};

  static const uint32_t MWM_HINTS_DECORATIONS   =   2;
  static const uint32_t MWM_DECOR_ALL           =   1;
  decoration_hints.flags = MWM_HINTS_DECORATIONS;
  decoration_hints.decorations = decorated ? MWM_DECOR_ALL : 0;

  XChangeProperty(display, win,
                  motif_wm_hints,
                  motif_wm_hints, 32,
                  PropModeReplace,
                  (unsigned char*) &hints,
                  sizeof(decoration_hints) / sizeof(long));
    

  if (n_windows + 1 <= MAX_WINDOWS) {
    window_callbacks[n_windows].win = win;
    int32_t w, h;
    get_window_size(display, win, &w, &h);
    window_callbacks[n_windows].win = win;
    window_callbacks[n_windows].win_width = w;
    window_callbacks[n_windows].win_height = h;
    window_callbacks[n_windows].ev_mouse_press_cb = NULL;
    window_callbacks[n_windows].ev_mouse_release_cb = NULL;
    window_callbacks[n_windows].ev_close_cb = NULL;
    window_callbacks[n_windows].ev_refresh_cb = NULL;
    window_callbacks[n_windows].ev_resize_cb = NULL;
    window_callbacks[n_windows].ui = NULL;
    window_callbacks[n_windows].glcontext = glcontext;
    ++n_windows;
  } else {
    fprintf(stderr, "warning: reached maximum amount of windows to define callbacks for.\n");
  }

  return win;
}

int32_t 
lf_windowing_init(void) {
  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "reif: cannot open X display.\n");
    return 1;
  }
  wm_protocols_atom = XInternAtom(display, "WM_PROTOCOLS", False);
  wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", False);
  motif_wm_hints = XInternAtom(display, "_MOTIF_WM_HINTS", False);
  return 0;
}

int32_t 
lf_windowing_terminate(void) {
  XCloseDisplay(display);
  return 0;
}

void
lf_windowing_update(void) {
  current_event = WinEventNone;
}

void
lf_window_set_ui_state(lf_window_t win, lf_ui_state_t* state) {
  window_callbacks_t* data;
  if(!(data = win_data_from_native(win))) return;
  data->ui = state;
}

lf_event_type_t 
lf_windowing_get_current_event(void) {
  return current_event;
}

void 
lf_windowing_next_event(void) {
  XEvent event;
  while (XPending(display)) {
    XNextEvent(display, &event);
    if(windowing_event_cb)
      windowing_event_cb(&event);
    handle_event(&event);
  }
}

void* 
lf_win_get_display(void) {
  return display;
}

lf_window_t 
lf_win_create(uint32_t width, uint32_t height, const char* title) {
  return create_window(width, height, title, 0, NULL, 0); 
}

lf_window_t 
lf_win_create_ex(
  uint32_t width, uint32_t height, 
  const char* title, 
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints, uint32_t nhints) {
  return create_window(width, height, title, flags, hints, nhints); 
}


void 
lf_win_set_title(lf_window_t win, const char* title) {
  XStoreName(display, (Window)win, title);
}

int32_t 
lf_win_make_gl_context(lf_window_t win) {
  window_callbacks_t* data = win_data_from_native(win);
  if(!data) return 1;
  glXMakeCurrent(display, win, data->glcontext);
  return 0;
}

void 
lf_win_swap_buffers(lf_window_t win) {
  glXSwapBuffers(display, win);
}

void 
lf_win_destroy(lf_window_t win) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      XDestroyWindow(display, window_callbacks[i].win);
      break;
    }
  }
}

vec2s 
lf_win_cursor_pos(lf_window_t win) {
  Window root, child;
  int root_x, root_y, win_x, win_y;
  unsigned int mask;
  XQueryPointer(display, (Window)win, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);

  return (vec2s){
    .x = (float)win_x,
    .y = (float)win_y
  };
}

void 
lf_win_set_close_cb(lf_window_t win, lf_win_close_func close_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_close_cb = close_cb;
      break;
    }
  }
}

void 
lf_win_set_refresh_cb(lf_window_t win, lf_win_refresh_func refresh_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_refresh_cb = refresh_cb;
      break;
    }
  }
}

void 
lf_win_set_resize_cb(lf_window_t win, lf_win_resize_func resize_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_resize_cb = resize_cb;
      break;
    }
  }
}

void
lf_win_set_mouse_press_cb(lf_window_t win, lf_win_mouse_press_func mouse_press_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_mouse_press_cb = mouse_press_cb;
      break;
    }
  }
}

void
lf_win_set_mouse_release_cb(lf_window_t win, lf_win_mouse_release_func mouse_release_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_mouse_release_cb = mouse_release_cb;
      break;
    }
  }
}

void 
lf_win_set_mouse_move_cb(lf_window_t win, lf_win_mouse_move_func mouse_move_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_move_cb = mouse_move_cb;
      break;
    }
  }
}


vec2s 
lf_win_get_size(lf_window_t win) {
  XWindowAttributes attr;
  XGetWindowAttributes(display, (Window)win, &attr);
  return (vec2s){
    .x = (float)attr.width,
    .y = (float)attr.height
  };
}


int32_t lf_win_get_refresh_rate(lf_window_t win) {
  return 144;
}
void 
lf_windowing_set_event_cb(lf_windowing_event_func cb) {
  windowing_event_cb = cb;
}

#endif
