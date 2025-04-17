#ifndef __SGE_SDL_IMAGE_INIT_HPP
#define __SGE_SDL_IMAGE_INIT_HPP

#include <sge_init.hpp>

namespace sge
{
    class SDLImageInitializer : public Initializer
    {
        public:
            void do_initialize();
            void do_shutdown();
    };
}

#endif /* __SGE_SDL_IMAGE_INIT_HPP */
