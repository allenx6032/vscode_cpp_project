#include <imgui.cpp>
#include <imgui_demo.cpp>
#include <imgui_draw.cpp>

#define LOGURU_IMPLEMENTATION 1
#include <emilib_loguru.hpp>

#include <emilib_imgui_helpers.cpp>
#include <emilib_imgui_sdl.cpp>
#include <emilib_timer.cpp>

#ifdef OPENGL_REFERENCE_RENDERER
	#include <emilib_gl_lib.cpp>
	#include <emilib_gl_lib_sdl.cpp>
	#include <emilib_imgui_gl_lib.cpp>
#endif
