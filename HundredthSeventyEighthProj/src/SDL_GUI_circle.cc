#include<SDL_GUI_circle.h>

#include <SDL2_gfxPrimitives.h>

using namespace SDL_GUI;

Drawable *Circle::clone() const {
    return new Circle(*this);
}

void Circle::draw(SDL_Renderer *renderer, Position position) const {
    const RGB &c = this->_style._color;
    aacircleRGBA(renderer, position._x, position._y, this->_radius, c._r, c._g, c._b, c._a);
}
