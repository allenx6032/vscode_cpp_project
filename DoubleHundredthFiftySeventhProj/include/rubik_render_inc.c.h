#include "rubik_render_core.c.h"

#if R_BACKEND == R_BACKEND_VULKAN
# include "rubik_render_vulkan.c.h"
#else
# error Renderer backend not specified.
#endif
