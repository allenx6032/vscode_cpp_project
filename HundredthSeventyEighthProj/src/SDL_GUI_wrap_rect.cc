#include<SDL_GUI_wrap_rect.h>

using namespace SDL_GUI;

void WrapRect::update() {
    unsigned width = 0;
    unsigned height = 0;
    for (const Drawable *d: this->children()) {
        width = std::max(width, d->position()._x + d->width());
        height = std::max(height, d->position()._y + d->height());
    }
    this->set_width(width);
    this->set_height(height);
}
