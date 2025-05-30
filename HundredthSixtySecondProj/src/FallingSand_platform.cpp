
#include "FallingSand_platform.h"

#include <string.h>

#include "FallingSand_base_memory.h"
#include "FallingSand_audio.h"
#include "FallingSand_const.h"
#include "FallingSand_core.hpp"
#include "FallingSand_global.hpp"
#include "FallingSand_filesystem.h"
#include "FallingSand_packer.hpp"
#include "FallingSand_macros.hpp"
#include "FallingSand_sdl_wrapper.h"
#include "FallingSand_engine.hpp"
#include "FallingSand_renderer_gpu.h"
#include "FallingSand_player.hpp"
#include "glad.h"

namespace ME {

int ParseRunArgs(int argc, char *argv[]) {

    if (argc > 1) {
        char *v1 = argv[1];
        if (v1) {
            if (!strcmp(v1, "packer")) {
                if (argc <= 3) {
                    printf("Incorrect parameters\n");
                    return METADOT_FAILED;
                }

                ME_pack_result result = ME_pack_files(argv[2], argc - 3, (const char **)argv + 3, true);

                if (result != SUCCESS_PACK_RESULT) {
                    printf("\nError: %s.\n", pack_result_to_string(result));
                    return METADOT_FAILED;
                }

                return RUNNER_EXIT;

            } else if (!strcmp(v1, "unpacker")) {
                if (argc != 3) {
                    printf("Incorrect parameters\n");
                    return METADOT_FAILED;
                }

                ME_pack_result result = ME_unpack_files(argv[2], true);

                if (result != SUCCESS_PACK_RESULT) {
                    printf("\nError: %s.\n", pack_result_to_string(result));
                    return METADOT_FAILED;
                }
                return RUNNER_EXIT;

            } else if (!strcmp(v1, "packinfo")) {
                if (argc != 3) {
                    printf("Incorrect parameters\n");
                    return METADOT_FAILED;
                }

                uint8_t majorVersion;
                uint8_t minorVersion;
                uint8_t patchVersion;
                bool isLittleEndian;
                uint64_t itemCount;

                ME_pack_result result = ME_get_pack_info(argv[2], &majorVersion, &minorVersion, &patchVersion, &isLittleEndian, &itemCount);

                if (result != SUCCESS_PACK_RESULT) {
                    printf("\nError: %s.\n", pack_result_to_string(result));
                    return METADOT_FAILED;
                }

                printf("MetaDot Pack [v%d.%d.%d]\n"
                       "Pack information:\n"
                       " Version: %d.%d.%d.\n"
                       " Little endian: %s.\n"
                       " Item count: %llu.\n\n",
                       PACK_VERSION_MAJOR, PACK_VERSION_MINOR, PACK_VERSION_PATCH, majorVersion, minorVersion, patchVersion, isLittleEndian ? "true" : "false", (long long unsigned int)itemCount);

                ME_pack_reader packReader;

                result = ME_create_file_pack_reader(argv[2], 0, false, &packReader);

                if (result != SUCCESS_PACK_RESULT) {
                    printf("\nError: %s.\n", pack_result_to_string(result));
                    return METADOT_FAILED;
                }

                itemCount = ME_get_pack_item_count(packReader);

                for (uint64_t i = 0; i < itemCount; ++i) {
                    printf("Item %llu:\n"
                           " Path: %s.\n"
                           " Size: %u.\n",
                           (long long unsigned int)i, ME_get_pack_item_path(packReader, i), ME_get_pack_item_data_size(packReader, i));
                    fflush(stdout);
                }

                return RUNNER_EXIT;
            }
        } else {
        }
    }

    return METADOT_OK;
}

void metadot_gl_get_max_texture_size(int *w, int *h) {
    int max_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);

    if (w) *w = max_size;
    if (h) *h = max_size;
}

int ME_initwindow() {

    // metadot_platform_init_dpi();

    // init sdl
    METADOT_INFO("Initializing SDL...");
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        METADOT_ERROR("SDL_Init failed: %s", SDL_GetError());
        return METADOT_FAILED;
    }

    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);

// the<engine>().eng()->ing on Catalina with High DPI (retina)
// https://github.com/grimfang4/sdl-gpu/issues/201
#if defined(METADOT_ALLOW_HIGHDPI)
    R_WindowFlagEnum SDL_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
#else
    R_WindowFlagEnum SDL_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
#endif

    // create the window
    METADOT_INFO("Creating game window...");

    auto eng = the<engine>().eng();

    eng->window = SDL_CreateWindow(win_game, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, eng->windowWidth, eng->windowHeight, SDL_flags);

    if (eng->window == nullptr) {
        METADOT_ERROR("Could not create SDL_Window: %s", SDL_GetError());
        return METADOT_FAILED;
    }

    // SDL_SetWindowIcon(window, Textures::LoadTexture("data/assets/Icon_32x.png"));

    // create gpu target
    METADOT_INFO("Creating gpu target...");

    R_SetPreInitFlags(R_INIT_DISABLE_VSYNC);
    R_SetInitWindow(SDL_GetWindowID(eng->window));

    eng->target = R_Init(eng->windowWidth, eng->windowHeight, SDL_flags);

    if (eng->target == NULL) {
        METADOT_ERROR("Could not create R_Target: %s", SDL_GetError());
        return METADOT_FAILED;
    }

#if defined(METADOT_ALLOW_HIGHDPI)
    R_SetVirtualResolution(eng->Target_.target, WIDTH * 2, HEIGHT * 2);
#endif

    eng->realTarget = eng->target;

    eng->glContext = (C_GLContext *)eng->target->context->context;

    SDL_GL_MakeCurrent(eng->window, eng->glContext);

    auto metadot_gl_global_init = [](ME_gl_loader_fn loader_fp) {
        if (NULL == loader_fp) {
            if (!gladLoadGL()) {
                METADOT_ERROR("GLAD GL3 loader failed");
                return -1;
            }
        } else {
            if (!gladLoadGLLoader((GLADloadproc)loader_fp)) {
                METADOT_ERROR("GLAD GL3 loader failed");
                return -1;
            }
        }
        return 0;
    };

    if (metadot_gl_global_init(NULL) == -1) {
        METADOT_ERROR("Failed to initialize OpenGL loader!");
        return METADOT_FAILED;
    }

#ifdef METADOT_DEBUG
    const unsigned char *vendor = glGetString(GL_VENDOR);
    const unsigned char *renderer = glGetString(GL_RENDERER);
    const unsigned char *gl_version = glGetString(GL_VERSION);
    const unsigned char *glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

    int tex_w, tex_h;
    metadot_gl_get_max_texture_size(&tex_w, &tex_h);

    METADOT_INFO("OpenGL info:");
    METADOT_INFO("Vendor: %s", vendor);
    METADOT_INFO("Renderer: %s", renderer);
    METADOT_INFO("GL Version: %s", gl_version);
    METADOT_INFO("GLSL Version: %s", glsl_version);
    METADOT_INFO("Max texture size: %ix%i", tex_w, tex_h);
#endif

    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(ME::DebugOutputGL::GLerrorCallback, nullptr);

    global.audio.Init();

    // METADOT_INFO("Initializing InitFont...");
    // if (!Drawing::InitFont(&gl_context)) {
    //     METADOT_ERROR("InitFont failed");
    //     return METADOT_FAILED;
    // }

    // if (global.game->GameIsolate_.settings.networkMode != NetworkMode::SERVER) {

    //     font64 = Drawing::LoadFont(METADOT_RESLOC("data/assets/fonts/pixel_operator/PixelOperator.ttf"), 64);
    //     font16 = Drawing::LoadFont(METADOT_RESLOC("data/assets/fonts/pixel_operator/PixelOperator.ttf"), 16);
    //     font14 = Drawing::LoadFont(METADOT_RESLOC("data/assets/fonts/pixel_operator/PixelOperator.ttf"), 14);
    // }

#if defined(_WIN32)
    SDL_SysWMinfo info{};
    SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(eng->window, &info)) {
        ME_ASSERT(IsWindow(info.info.win.window));
        // eng->wndh = info.info.win.window;
    } else {
        // eng->wndh = NULL;
    }
#elif defined(__linux)
    global.HostData.wndh = 0;
#elif defined(__APPLE__)
    // global.HostData.wndh = 0;
#else
#error "GetWindowWMInfo Error"
#endif
    // this->data->window = window;
    // this->data->imgui_context = m_ImGuiCore->getImGuiCtx();

    // ME::AnyFunction func1{&IamAfuckingNamespace::func1};
    // ME::AnyFunction func2{&IamAfuckingNamespace::func2};

    // this->data->Functions.insert(std::make_pair("func1", func1));
    // this->data->Functions.insert(std::make_pair("func2", func2));

    // RegisterFunctions(func_log_info, IamAfuckingNamespace::func_log_info);

    return METADOT_OK;
}

void ME_endwindow() {

    if (NULL != the<engine>().eng()->target) R_FreeTarget(the<engine>().eng()->target);
    // if (NULL != the<engine>().eng()->realTarget) R_FreeTarget(the<engine>().eng()->realTarget);
    if (the<engine>().eng()->glContext) SDL_GL_DeleteContext(the<engine>().eng()->glContext);
    if (the<engine>().eng()->window) SDL_DestroyWindow(the<engine>().eng()->window);

    R_Quit();
}

void ME_win_set_displaymode(E_DisplayMode mode) {
    switch (mode) {
        case WINDOWED:
            SDL_SetWindowDisplayMode(the<engine>().eng()->window, NULL);
            SDL_SetWindowFullscreen(the<engine>().eng()->window, 0);
            // GameUI::OptionsUI::item_current_idx = 0;
            break;
        case BORDERLESS:
            SDL_SetWindowDisplayMode(the<engine>().eng()->window, NULL);
            SDL_SetWindowFullscreen(the<engine>().eng()->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            // GameUI::OptionsUI::item_current_idx = 1;
            break;
        case FULLSCREEN:
            SDL_MaximizeWindow(the<engine>().eng()->window);

            int w;
            int h;
            SDL_GetWindowSize(the<engine>().eng()->window, &w, &h);

            SDL_DisplayMode disp;
            SDL_GetWindowDisplayMode(the<engine>().eng()->window, &disp);

            disp.w = w;
            disp.h = h;

            SDL_SetWindowDisplayMode(the<engine>().eng()->window, &disp);
            SDL_SetWindowFullscreen(the<engine>().eng()->window, SDL_WINDOW_FULLSCREEN);
            // GameUI::OptionsUI::item_current_idx = 2;
            break;
    }
}

void ME_win_set_windowflash(E_WindowFlashaction action, int count, int period) {
    // TODO: look into alternatives for linux/crossplatform
#ifdef ME_PLATFORM_WINDOWS

    FLASHWINFO flash;
    flash.cbSize = sizeof(FLASHWINFO);
    // flash.hwnd = hwnd;
    flash.uCount = count;
    flash.dwTimeout = period;

    // pretty sure these flags are supposed to work but they all seem to do the same thing on my machine so idk
    switch (action) {
        case E_WindowFlashaction::START:
            flash.dwFlags = FLASHW_ALL;
            break;
        case E_WindowFlashaction::START_COUNT:
            flash.dwFlags = FLASHW_ALL | FLASHW_TIMER;
            break;
        case E_WindowFlashaction::START_UNTIL_FG:
            flash.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
            break;
        case E_WindowFlashaction::STOP:
            flash.dwFlags = FLASHW_STOP;
            break;
    }

    FlashWindowEx(&flash);

#endif
}

void ME_set_vsync(bool vsync) {
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    // GameUI::OptionsUI::vsync = vsync;
}

void ME_win_set_minimize_onlostfocus(bool minimize) { SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, minimize ? "1" : "0"); }

void ME_win_set_windowtitle(const char *title) { SDL_SetWindowTitle(the<engine>().eng()->window, title); }

char *ME_clipboard_get() {
    char *text = SDL_GetClipboardText();
    return text;
}

void ME_clipboard_set(const char *string) { SDL_SetClipboardText(string); }

}  // namespace ME