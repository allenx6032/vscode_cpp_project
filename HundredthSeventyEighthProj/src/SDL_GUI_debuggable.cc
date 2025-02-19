#include<SDL_GUI_debuggable.h>
#include<SDL_GUI_drawable.h>

using namespace SDL_GUI;

Debuggable::Debuggable() {
    this->_draw_debug_information = std::bind(&Debuggable::default_draw_debug_information, this,
                                              std::placeholders::_1, std::placeholders::_2,
                                              std::placeholders::_3);
}

Debuggable::~Debuggable() {
    for (const auto &[d, _]: this->_debug_information) {
        delete d;
    }
}

void Debuggable::default_draw_debug_information(SDL_Renderer *renderer, Position position,
                                                SDL_Rect parent_clip_rect) const {
    for (const auto &[wrapper, shown]: this->_debug_information) {
        if (not shown()) {
            continue;
        }
        wrapper->map([](Drawable *d) {
                d->recalculate();
            });
        wrapper->render(renderer, position, parent_clip_rect, false, true);
    }
}

void Debuggable::add_debug_drawable(Drawable *drawable, std::function<bool()> criteria) {
    this->_debug_information.emplace(drawable, criteria);
}

void Debuggable::draw_debug_information(SDL_Renderer *renderer, Position position,
                                        SDL_Rect parent_clip_rect) const {
    this->_draw_debug_information(renderer, position, parent_clip_rect);
}
