#ifndef _LIBCLIPBOARD_CONFIG_H
#define _LIBCLIPBOARD_CONFIG_H
   #define LIBCLIPBOARD_VERSION_MAJOR "1"
   #define LIBCLIPBOARD_VERSION_MINOR "1"
   #ifdef WIN32
      #define LIBCLIPBOARD_FORCE_WIN32  1
      #define LIBCLIPBOARD_BUILD_WIN32 1
   #else
      #define LIBCLIPBOARD_FORCE_X11 1
      #define LIBCLIPBOARD_BUILD_X11 1
   #endif
   #define LIBCLIPBOARD_USE_STDCALL 1
   // #define LIBCLIPBOARD_BUILD_SHARED 0
#endif /* _LIBCLIPBOARD_CONFIG_H */
