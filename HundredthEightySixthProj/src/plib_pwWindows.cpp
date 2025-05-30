/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

     For further information visit http://plib.sourceforge.net

     $Id: pwWindows.cxx 2046 2005-09-02 21:03:43Z bram $
*/


#include "plib_ul.h"

/* ONLY COMPILE THIS FILE FOR MS-WINDOWS SYSTEMS */

#ifdef UL_WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>

#include "plib_pw.h"

static int             initialised  = 0 ;
static int             modifiers    = 0 ;
static int             origin [2]   = {   0,   0 } ;
static int             size   [2]   = { 640, 480 } ;

static bool            autoRepeat   = false ;
static DWORD           style        = 0 ;

static pwResizeCB     *resizeCB     = NULL ;
static pwExitCB       *exitCB       = NULL ;
static pwKeybdFunc    *kbCB         = NULL ;
static pwMouseFunc    *msCB         = NULL ;
static pwMousePosFunc *mpCB         = NULL ;

static HINSTANCE       currInstance = NULL ;
static HWND            currWnd      = NULL ;
static HDC             currDC       = NULL ;
static HGLRC           currGLRC     = NULL ;


void defaultExitFunc ()
{
  pwCleanup () ;
  exit ( 0 ) ;
}


static void refreshModifiers ()
{
  modifiers = 0 ;

  if( ( GetKeyState ( VK_SHIFT )   & 0x8000 ) != 0 ) modifiers |= PW_SHIFT ;
  if( ( GetKeyState ( VK_CONTROL ) & 0x8000 ) != 0 ) modifiers |= PW_CTRL  ;
  if( ( GetKeyState ( VK_MENU )    & 0x8000 ) != 0 ) modifiers |= PW_ALT   ;
}

inline void clientSizeToWindowSize ( int& w, int& h, DWORD dwStyle )
{
  RECT rect;
  rect.left   = 0 ;
  rect.right  = w ;
  rect.top    = 0 ;
  rect.bottom = h ;
  AdjustWindowRect( &rect, dwStyle, false ) ;
  w = rect.right - rect.left ;
  h = rect.bottom - rect.top ;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  static int key = -1 ;
  static int updown = PW_UP ;
  static int old_key = -1 ;  // We need this because "key" changes case
  int button = -1 ;
  static int mb = 0 ;
  static int lastx = 0 ;
  static int lasty = 0 ;

  refreshModifiers () ;

  switch (uMsg)
  {
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      updown = PW_DOWN ;
      if      ( uMsg == WM_LBUTTONDOWN ) { button = PW_LEFT_BUTTON   ; mb |= 1 ; }
      else if ( uMsg == WM_MBUTTONDOWN ) { button = PW_MIDDLE_BUTTON ; mb |= 2 ; }
      else                               { button = PW_RIGHT_BUTTON  ; mb |= 4 ; }
      SetCapture ( hWnd ) ;
      lastx = SHORT ( LOWORD ( lParam ) ) ;
      lasty = SHORT ( HIWORD ( lParam ) ) ;
      if ( button != -1 && msCB )
        (*msCB) ( button, updown, lastx, lasty ) ;
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      updown = PW_UP ;
      if      ( uMsg == WM_LBUTTONUP ) { button = PW_LEFT_BUTTON   ; mb &= ~1 ; }
      else if ( uMsg == WM_MBUTTONUP ) { button = PW_MIDDLE_BUTTON ; mb &= ~2 ; }
      else                             { button = PW_RIGHT_BUTTON  ; mb &= ~4 ; }
      if ( mb == 0 ) ReleaseCapture () ;
      lastx = SHORT ( LOWORD ( lParam ) ) ;
      lasty = SHORT ( HIWORD ( lParam ) ) ;
      if ( button != -1 && msCB )
        (*msCB) ( button, updown, lastx, lasty ) ;
      break;

    case WM_MOUSEMOVE:
      lastx = SHORT ( LOWORD ( lParam ) ) ;
      lasty = SHORT ( HIWORD ( lParam ) ) ;
      if ( mpCB )
        (*mpCB) ( lastx, lasty ) ;
      break;

    case WM_KEYDOWN:
      /* If the key is already down, we are on auto-repeat.  Break if the autorepeat is disabled. */
      if ( ( updown == PW_DOWN ) && ( int(wParam) == old_key ) )
      {
        if ( autoRepeat )
        {
          /* Disable CTRL, SHIFT, CapsLock keys from making a callback */
          if ( ( key == VK_CONTROL ) || ( key == VK_SHIFT ) || ( key == VK_CAPITAL ) )
            break ;

          if ( key != -1 && kbCB )  // Autorepeat enabled, call the callback with an "up" setting
            (*kbCB) ( key, PW_UP, lastx, lasty ) ;
        }
        else
          break ;  // Autorepeat not enabled, just break
      }

      updown = PW_DOWN ;
      old_key = wParam ;
      /* FALLTHROUGH */
    case WM_KEYUP:
      if ( uMsg == WM_KEYUP ) updown = PW_UP ;
      key = wParam ;

      /* Disable CTRL, SHIFT, CapsLock keys from making a callback */
      if ( ( key == VK_CONTROL ) || ( key == VK_SHIFT ) || ( key == VK_CAPITAL ) )
        break ;

      switch ( key )
      {
        case VK_F1:     key = PW_KEY_F1;        break;
        case VK_F2:     key = PW_KEY_F2;        break;
        case VK_F3:     key = PW_KEY_F3;        break;
        case VK_F4:     key = PW_KEY_F4;        break;
        case VK_F5:     key = PW_KEY_F5;        break;
        case VK_F6:     key = PW_KEY_F6;        break;
        case VK_F7:     key = PW_KEY_F7;        break;
        case VK_F8:     key = PW_KEY_F8;        break;
        case VK_F9:     key = PW_KEY_F9;        break;
        case VK_F10:    key = PW_KEY_F10;       break;
        case VK_F11:    key = PW_KEY_F11;       break;
        case VK_F12:    key = PW_KEY_F12;       break;

        case VK_LEFT:   key = PW_KEY_LEFT;      break;
        case VK_RIGHT:  key = PW_KEY_RIGHT;     break;
        case VK_UP:     key = PW_KEY_UP;        break;
        case VK_DOWN:   key = PW_KEY_DOWN;      break;

        case VK_PRIOR:  key = PW_KEY_PAGE_UP;   break;
        case VK_NEXT:   key = PW_KEY_PAGE_DOWN; break;
        case VK_HOME:   key = PW_KEY_HOME;      break;
        case VK_END:    key = PW_KEY_END;       break;
        case VK_INSERT: key = PW_KEY_INSERT;    break;

        default:
          // don't do this for WinCE
          BYTE state[ 256 ];
          WORD code[ 2 ];

          BOOL b = GetKeyboardState( state );
          assert(b);

          code [ 0 ] = 0; // WK: I need to do this, or on my Win2k box, the upper bits remain unchanged.
          if( ToAscii( key, 0, state, code, 0 ) == 1 )
            if((0xFF00 & code[0]) == 0) // setting a high bit in key causes crashes later on (out of range array access)
              key=code[ 0 ];

      }
      if ( key != -1 && kbCB )
        (*kbCB) ( key, updown, lastx, lasty ) ;
      break;

    case WM_SIZE:
      size[0] = LOWORD ( lParam ) ;
      size[1] = HIWORD ( lParam ) ;
      if ( resizeCB )
        (*resizeCB) ( size[0], size[1] ) ;
      break;

    case WM_CLOSE:
      (*exitCB)();
      break;

    default:
      return ( DefWindowProc ( hWnd, uMsg, wParam, lParam ) ) ;
  }

  return 0;
}


void pwSetAutoRepeatKey ( bool enable )
{
  autoRepeat = enable ;
}

void pwSetCallbacks ( pwKeybdFunc *kb, pwMouseFunc *ms,
                      pwMousePosFunc *mp, pwResizeCB *rcb, pwExitCB *ecb )
{
  if ( ! initialised )
  {
    fprintf ( stderr, "PW: You must not call pwSetCallbacks before pwInit.\n");
    exit ( 1 ) ;
  }

  kbCB = kb ;
  msCB = ms ;
  mpCB = mp ;
  resizeCB = rcb ;
  exitCB   = ecb ? ecb : defaultExitFunc ;
}


void pwInit ( int multisample, int num_samples )
{
  pwInit ( 0, 0, -1, -1, multisample, "NoName", FALSE, num_samples ) ;
}


//typedef const char * wglGetExtensionsStringARBtype ( HDC hdc ) ;

void pwInit ( int x, int y, int w, int h, int multisample,
              char *title, int border, int num_samples )
{
  currInstance = GetModuleHandleA( NULL ) ;

  /* Register the window class */
  WNDCLASS wc;
  wc.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW ;
  wc.lpfnWndProc   = WndProc ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = currInstance ;
  wc.hIcon         = LoadIcon ( NULL, IDI_APPLICATION ) ;
  wc.hCursor       = LoadCursor ( NULL, IDC_ARROW ) ;
  wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 ) ;
  wc.lpszMenuName  = NULL ;
  wc.lpszClassName = "PlibAppClass" ;
  if ( !RegisterClass( &wc ) )
  {
    fprintf ( stderr, "PW: Can't register window class" ) ;
    exit ( 1 ) ;
  }

  /* Setup window style */
  bool fullscn = (w == -1 && h == -1 ) ? true : false ;

  if ( w == -1 ) w = GetSystemMetrics( SM_CXSCREEN ) ;
  if ( h == -1 ) h = GetSystemMetrics( SM_CYSCREEN ) ;

  origin [ 0 ] = x ;
  origin [ 1 ] = y ;
  size   [ 0 ] = w ;
  size   [ 1 ] = h ;

  RECT rect ;
  rect.left   = x ;
  rect.top    = y ;
  rect.right  = rect.left + w ;
  rect.bottom = rect.top + h ;

  if ( !fullscn )
    style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
  else
    style = WS_POPUP ;

  AdjustWindowRect( &rect, style, false ) ;
  
  // The origin is really where the upper left of the window should be. 
  // rect is the setting for the window if the origin is for the client area.
  // Adjust the window rect so that it will be located at the origin (x,y).

  int deltaX = x - rect.left ;
  int deltaY = y - rect.top ;

  rect.left   += deltaX ;
  rect.right  += deltaX ;
  rect.top    += deltaY ;
  rect.bottom += deltaY ;

  /* Create the window */
  currWnd = CreateWindow(
    "PlibAppClass", title, style,
    rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
    NULL, NULL, currInstance, NULL ) ;

  if ( !currWnd )
  {
    fprintf ( stderr, "PW: Can't create window" ) ;
    exit ( 1 ) ;
  }

  /* Get DC for this window */
  currDC = GetDC ( currWnd ) ;
  if ( !currDC )
  {
    fprintf ( stderr, "PW: Can't get window DC" ) ;
    exit ( 1 ) ;
  }

//  wglGetExtensionsStringARBtype *wglGetExtensionsStringARB = (wglGetExtensionsStringARBtype *)wglGetProcAddress ( "wglGetExtensionsStringARB" ) ;
//  const char *extensionsString = wglGetExtensionsStringARB ( currDC ) ;

//  printf ( "%s %x %s\n", glGetString ( GL_EXTENSIONS ), wglGetExtensionsStringARB, extensionsString ) ;

//  if (!GLExtensionExists("WGL_ARB_multisample "))
//    return suggestedFormat;

  /* Set pixel format for DC */
  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), /* size of this pfd       */
    1,                             /* version number         */
    PFD_DRAW_TO_WINDOW |           /* support window         */
    PFD_SUPPORT_OPENGL |           /* support OpenGL         */
    PFD_DOUBLEBUFFER,              /* double buffered        */
    PFD_TYPE_RGBA,                 /* RGBA type              */
    24,                            /* 24-bit color depth     */
    0, 0, 0, 0, 0, 0,              /* color bits ignored     */
    0,                             /* no alpha buffer        */
    0,                             /* shift bit ignored      */
    0,                             /* no accumulation buffer */
    0, 0, 0, 0,                    /* accum bits ignored     */
    32,                            /* 32-bit z-buffer        */
    0,                             /* no stencil buffer      */
    0,                             /* no auxiliary buffer    */
    PFD_MAIN_PLANE,                /* main layer             */
    0,                             /* reserved               */
    0, 0, 0                        /* layer masks ignored    */
  };

  /* Get best available match of pixel format for DC */
  int iPixelFormat = ChoosePixelFormat ( currDC, &pfd ) ;
  if ( iPixelFormat == 0 )
  {
    fprintf ( stderr, "PW: Can't get OpenGL pixel format" ) ;
    exit ( 1 ) ;
  }

  /* Set the pixel format of the DC */
  if ( !SetPixelFormat( currDC, iPixelFormat, &pfd ) )
  {
    fprintf ( stderr, "PW: Can't set OpenGL pixel format" ) ;
    exit ( 1 ) ;
  }

  /* Show the window and paint its contents */
  ShowWindow ( currWnd, SW_SHOW ) ;
  UpdateWindow ( currWnd ) ;

  /* Create openGL context and make it current */
  currGLRC = wglCreateContext ( currDC ) ;
  if ( !currGLRC )
  {
    fprintf ( stderr, "PW: Can't create OpenGL context" ) ;
    exit ( 1 ) ;
  }
  if ( !wglMakeCurrent ( currDC, currGLRC ) )
  {
    fprintf ( stderr, "PW: Can't make OpenGL context current" ) ;
    exit ( 1 ) ;
  }
  
  kbCB = NULL ;
  msCB = NULL ;
  mpCB = NULL ;
  resizeCB = NULL ;
  exitCB   = defaultExitFunc ;
  initialised  = 1 ;

  glClear ( GL_COLOR_BUFFER_BIT ) ;
  pwSwapBuffers () ;
  glClear ( GL_COLOR_BUFFER_BIT ) ;
  pwSwapBuffers () ;
}


void pwGetSize ( int *w, int *h )
{
  if ( w ) *w = size[0] ;
  if ( h ) *h = size[1] ;
}


void pwSetCursor ( int c )
{
  LPCSTR cursor;
  switch ( c )
  {
    case PW_CURSOR_RIGHT  : cursor = IDC_ARROW ; break ;
    case PW_CURSOR_LEFT   : cursor = IDC_ARROW ; break ;
    case PW_CURSOR_QUERY  : cursor = IDC_HELP  ; break ;
  /*case PW_CURSOR_AIM    : cursor = ???       ; break ;*/
    case PW_CURSOR_CIRCLE : cursor = IDC_NO    ; break ;
    case PW_CURSOR_WAIT   : cursor = IDC_WAIT  ; break ;
    case PW_CURSOR_CROSS  : cursor = IDC_CROSS ; break ;
    case PW_CURSOR_NONE   : cursor = 0         ; break ;
    default:
      cursor = IDC_ARROW;
  }
  SetCursor( LoadCursor ( NULL, cursor ) ) ;
}

void pwSetSize ( int w, int h )
{
  clientSizeToWindowSize ( w, h, style ) ;
  SetWindowPos ( currWnd, HWND_TOP, 0, 0, w, h, SWP_NOMOVE ) ;
}


void pwSetOrigin ( int x, int y )
{
  SetWindowPos ( currWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE ) ;
}


void pwSetSizeOrigin ( int x, int y, int w, int h )
{
  clientSizeToWindowSize ( w, h, style ) ;
  SetWindowPos ( currWnd, HWND_TOP, x, y, w, h, 0 ) ;
}


int pwGetModifiers ()
{
  return modifiers ;
}


static void getEvents ()
{
  MSG msg;
  while ( PeekMessage( &msg, currWnd, 0, 0, PM_REMOVE ) )
  {
      TranslateMessage( &msg ) ;
      DispatchMessage ( &msg ) ;
  }
}


void pwSwapBuffers ()
{
  glFlush () ;
  SwapBuffers ( currDC ) ;
  getEvents () ;
}


void pwCleanup ()
{
  wglMakeCurrent ( NULL, NULL ) ;
  wglDeleteContext ( currGLRC ) ;
  DestroyWindow ( currWnd ) ;
}


#endif

