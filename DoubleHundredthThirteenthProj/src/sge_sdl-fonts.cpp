#include <SDL.h>
#include <SDL_ttf.h>

#include <sge_sdl-fonts.hpp>

namespace sge
{
    void SDLFontsInitializer::do_initialize()
    {
        if (TTF_Init() != 0)
        {
            throw InitError("SDL_ttf", TTF_GetError());
        }
    }

    void SDLFontsInitializer::do_shutdown()
    {
        TTF_Quit();
    }
}
