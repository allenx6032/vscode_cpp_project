#define SOKOL_WIN32_NO_GL_LOADER
#define SOKOL_IMPL
#include "sokol_audio.h"
#include "sokol_gfx.h"
#include "sokol_time.h"

#define SOKOL_WIN32_FORCE_MAIN
#include "sokol_app.h"
#include "sokol_glue.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

#define SOKOL_IMGUI_IMPL
#include "sokol_imgui.h"
