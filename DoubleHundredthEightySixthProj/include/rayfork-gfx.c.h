#include "rayfork-gfx.h"

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    #include "rayfork-backend-opengl.c.h"
#endif

#if defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    #include "rayfork-backend-sokol.c.h"
#endif

#include "rayfork-camera.c.h"
#include "rayfork-colors.c.h"
#include "rayfork-image.c.h"
#include "rayfork-texture.c.h"
#include "rayfork-font.c.h"
#include "rayfork-model.c.h"
#include "rayfork-high-level-renderer.c.h"
#include "rayfork-render-batch.c.h"
#include "rayfork-context.c.h"