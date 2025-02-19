#include<SDL_GUI_vertical_line.h>

using namespace SDL_GUI;

Drawable *VerticalLine::clone() const {
    return new VerticalLine(*this);
}

void VerticalLine::draw(SDL_Renderer *renderer, Position position) const {
    this->_style._color.activate(renderer);
    SDL_RenderDrawLine(renderer, position._x, position._y, position._x, position._y + this->_height);
}
