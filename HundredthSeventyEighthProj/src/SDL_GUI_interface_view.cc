#include <views/interface_view.h>

#include <iostream>
#include <tuple>

#include<SDL_GUI_rect.h>
#include<SDL_GUI_text.h>

using namespace SDL_GUI;

InterfaceView::InterfaceView(SDL_Renderer *renderer, const InterfaceModel *interface_model)
    : _renderer(renderer), _interface_model(interface_model) {
    this->init();
}


bool InterfaceView::init() {
    return true;
}

void InterfaceView::deinit() {}

void InterfaceView::update() {
}

void InterfaceView::render() {
    SDL_SetRenderDrawColor(this->_renderer, 170, 170, 170, 0);
    SDL_RenderClear(this->_renderer);


    SDL_Renderer *renderer = this->_renderer;
    /* draw all drawables recursively */
    SDL_RenderSetClipRect(renderer, NULL);
    SDL_Rect initial_clip_rect = {
        0,
        0,
        static_cast<int>(this->_interface_model->window_width()),
        static_cast<int>(this->_interface_model->window_height())
    };
    this->_interface_model->drawable_root()->render(this->_renderer, {0,0}, initial_clip_rect,
                                                    false);

    SDL_RenderPresent(this->_renderer);
}
