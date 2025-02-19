#include <SDL_GUI_application.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>

#include <SDL.h>
#include <SDL_ttf.h>

#include <SDL_GUI_input_controller.h>
#include <SDL_GUI_command_line.h>


using namespace SDL_GUI;


inline std::chrono::high_resolution_clock::time_point t_sys() {
    return std::chrono::high_resolution_clock::now();
}

ApplicationBase::~ApplicationBase() {
    for (ModelBase *model: this->_model_list) {
        delete model;
    }
    for (ViewBase *view: this->_view_list) {
        delete view;
    }
    for (ControllerBase *controller: this->_controller_list) {
        delete controller;
    }
    for (std::pair<std::string, SDL_Texture *> t: Texture::_textures) {
        SDL_DestroyTexture(t.second);
    }

    /* properly destroy renderer and window */
    SDL_DestroyRenderer(this->_renderer);
    SDL_DestroyWindow(this->_window);

    /* properly shut down SDL */
    TTF_Quit();
    SDL_Quit();
}

void ApplicationBase::init_SDL() {
    if (0 != SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    if (0 != TTF_Init()) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

SDL_Window *ApplicationBase::init_window(std::string title, unsigned width, unsigned height) {
    /* init window at {0,0} */
    SDL_Window *window = SDL_CreateWindow(title.c_str(), 0, 0, width, height, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "unable to create window: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    return window;
}

SDL_Renderer *ApplicationBase::init_renderer(SDL_Window *window) {
    /* init renderer  for first found rendering device (-1) */
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                                            SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "unable to create renderer: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return renderer;
}

ApplicationBase::ApplicationBase(std::string application_title, int argc, char *argv[],
                                 unsigned window_width, unsigned window_height)
    : _application_title(application_title), _window_width(window_width),
      _window_height(window_height) {
    this->_command_line.register_flag("headless", "", "headless");
    this->_command_line.parse(argc, argv);

    if (this->_command_line.get_flag("headless")) {
        this->_is_headless = true;
        std::cerr << std::endl << "== Running in headless mode. Press Ctrl-C to quit. =="
                  << std::endl;
        return;
    }

    ApplicationBase::init_SDL();
    this->_window = ApplicationBase::init_window(application_title, window_width, window_height);
    this->_renderer = ApplicationBase::init_renderer(this->_window);
}

void ApplicationBase::run() {
    using namespace std::literals;
    using duration_t = std::chrono::high_resolution_clock::duration;
    using time_point_t = std::chrono::high_resolution_clock::time_point;
    std::list<time_point_t> ticks;
    std::list<time_point_t> frames;
    std::list<time_point_t> loops;


    const duration_t frame_interval = 1000000us / this->_target_fps;
    const duration_t tick_interval = 1000000us / this->_target_tps;

    time_point_t now = t_sys();

    time_point_t last_tick_time = now;
    time_point_t next_tick_time = now;
    time_point_t last_frame_time = now;
    time_point_t next_frame_time = now;
    time_point_t last_loop_time = now;

    time_point_t one_second_ago = now - 1s;

    while (this->_is_running) {
        now = t_sys();
        if (next_tick_time - now <= 1ms) {

            last_tick_time = now;
            next_tick_time += tick_interval;

            read_sdl_events();
            this->update_controllers();
            this->update_views();
            clear_sdl_events();


            /* update tps stat */
            ticks.push_back(last_tick_time);

            one_second_ago = now - 1s;
            while (not ticks.empty() and ticks.front() < one_second_ago) {
                ticks.pop_front();
            }
            this->_current_tps = ticks.size();
        }

        now = t_sys();
        if (next_frame_time - now <= 1ms) {

            last_frame_time = now;
            next_frame_time += frame_interval;

            if (not this->_is_headless) {
                /* render */
                this->render_views();
            }

            /* update fps stat */
            frames.push_back(last_frame_time);

            one_second_ago = now - 1s;
            while (not frames.empty() and frames.front() < one_second_ago) {
                frames.pop_front();
            }
            this->_current_fps = frames.size();
        }

        now = t_sys();

        /* update loop stat */
        last_loop_time = now;
        loops.push_back(last_loop_time);

        one_second_ago = now - 1s;
        while (not loops.empty() and loops.front() < one_second_ago) {
            loops.pop_front();
        }

        this->_current_loops = loops.size();

        duration_t time_until_next_tick = next_tick_time - now;
        duration_t time_until_next_frame = next_frame_time - now;
        duration_t time_until_next = std::min(time_until_next_tick, time_until_next_frame);
        int delay_ms = time_until_next / 1ms;
        if (delay_ms >= 0) {
            SDL_Delay(std::max(1, delay_ms));
        }

    }
    this->deinit();
}

void ApplicationBase::update_controllers() {
    for (ControllerBase *controller: this->_controller_list) {
        controller->update();
    }
}

void ApplicationBase::update_views() {
    for (ViewBase *view: this->_view_list) {
        view->update();
    }
}

void ApplicationBase::render_views() {
    for (ViewBase *view: this->_view_list) {
        view->render();
    }
}

SDL_Window *ApplicationBase::window() {
    return this->_window;
}

SDL_Renderer *ApplicationBase::renderer() {
    return this->_renderer;
}

unsigned ApplicationBase::window_width() const {
    return this->_window_width;
}

unsigned ApplicationBase::window_height() const {
    return this->_window_height;
}

unsigned ApplicationBase::target_fps() const {
    return this->_target_fps;
}

unsigned ApplicationBase::current_fps() const {
    return this->_current_fps;
}

unsigned ApplicationBase::target_tps() const {
    return this->_target_tps;
}

unsigned ApplicationBase::current_tps() const {
    return this->_current_tps;
}

unsigned ApplicationBase::current_loops() const {
    return this->_current_loops;
}

bool ApplicationBase::is_headless() const {
    return this->_is_headless;
}

CommandLine *ApplicationBase::command_line() {
    return &this->_command_line;
}

void ApplicationBase::add_model(ModelBase *model) {
    this->_model_list.push_back(model);
    std::sort(this->_model_list.begin(), this->_model_list.end(),
        [](ModelBase *a, ModelBase *b){
            return a->_weight < b->_weight;
        }
    );
}

void ApplicationBase::add_controller(ControllerBase *controller) {
    this->_controller_list.push_back(controller);
    std::sort(this->_controller_list.begin(), this->_controller_list.end(),
        [](ControllerBase *a, ControllerBase *b){
            return a->_weight < b->_weight;
        }
    );

}

void ApplicationBase::add_view(ViewBase *view) {
    this->_view_list.push_back(view);
    std::sort(this->_view_list.begin(), this->_view_list.end(),
        [](ViewBase *a, ViewBase *b){
            return a->_weight < b->_weight;
        }
    );
}
