#ifndef __SGE_ENGINE_HPP
#define __SGE_ENGINE_HPP

#include <sge_config.hpp>

#include <sge_init.hpp>
#include <sge_sdl.hpp>
#include <sge_sdl-image.hpp>
#include <sge_sdl-mixer.hpp>
#include <sge_sdl-fonts.hpp>
#include <sge_sdl-window.hpp>

#include <sge_renderer.hpp>
#include <sge_mainloop.hpp>
#include <sge_actionmgr.hpp>
#include <sge_audiomgr.hpp>
#include <sge_scenemgr.hpp>
#include <sge_nodemgr.hpp>
#include <sge_physicsmanager.hpp>

#include <sge_assetmanager.hpp>
#include <sge_file.hpp>
#include <sge_image.hpp>
#include <sge_audio.hpp>
#include <sge_font.hpp>
#include <sge_json.hpp>

#include <memory>

namespace sge
{
    class Engine
    {
        public:
            Engine(Configuration &configuration);
            ~Engine();

            void init();

            Configuration &configuration();
            Startup &startup();
            MainLoop &mainloop();
            ActionManager &actions();
            AudioManager &audio();
            AssetManager &assets();
            SceneManager &scenes();
            NodeManager &nodes();
            PhysicManager &physics();

            SDL_Window *window() const;
            Renderer &renderer();

        private:
            Configuration _configuration;

            Startup _startup;
            std::shared_ptr<SDLInitializer> _sdl_init;
            std::shared_ptr<SDLImageInitializer> _sdl_img_init;
            std::shared_ptr<SDLMixerInitializer> _sdl_mixer_init;
            std::shared_ptr<SDLFontsInitializer> _sdl_fonts_init;
            std::shared_ptr<SDLWindowInitializer> _sdl_window_init;

            Renderer _renderer;
            MainLoop _mloop;
            ActionManager _amgr;
            AudioManager _audiomgr;
            SceneManager _scmgr;
            NodeManager _nodemgr;
            PhysicManager _pmgr;

            AssetManager _assets;

            std::shared_ptr<FileLocator> _asset_file_locator;
            std::shared_ptr<ImageLoader> _asset_image_loader;
            std::shared_ptr<AudioLoader> _asset_audio_loader;
            std::shared_ptr<FontLoader> _asset_font_loader;
            std::shared_ptr<JSONLoader> _asset_json_loader;
    };
}

#endif /* __SGE_ENGINE_HPP */
