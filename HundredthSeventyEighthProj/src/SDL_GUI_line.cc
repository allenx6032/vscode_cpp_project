#include<SDL_GUI_line.h>

#include <SDL2_gfx/SDL2_gfxPrimitives.h>

using namespace SDL_GUI;

Line::Line(Position begin, Position end)
    : Drawable("Line"), _begin(begin), _end(end) {
    this->update_dimensions();
}

void Line::update_dimensions() {
    int min_x = std::min(this->_begin._x, this->_end._x);
    int max_x = std::max(this->_begin._x, this->_end._x);
    int min_y = std::min(this->_begin._y, this->_end._y);
    int max_y = std::max(this->_begin._y, this->_end._y);
    this->_position = Position(min_x, min_y);
    this->_width = max_x - min_x;
    this->_height = max_y - min_y;
}

Drawable *Line::clone() const {
    return new Line(*this);
}

void Line::draw(SDL_Renderer *renderer, Position position) const {
    position -= this->_position;
    Position begin = this->_begin + position;
    Position end = this->_end + position;
    const RGB &c = this->_style._color;
    if (this->_line_width == 1) {
        aalineRGBA(renderer, begin._x, begin._y, end._x, end._y,
                   c._r, c._g, c._b, c._a);
    } else {
        thickLineRGBA(renderer, begin._x, begin._y, end._x, end._y, this->_line_width,
                      c._r, c._g, c._b, c._a);
    }
}

void Line::set_end(Position position) {
    this->_end = position;
    this->update_dimensions();
}

void Line::set_end_relative_to_begin(Position position) {
    this->_end = this->_begin + position;
    this->update_dimensions();
}

void Line::set_begin(Position position) {
    this->_begin = position;
    this->update_dimensions();
}

void Line::move(Position direction) {
    this->_begin += direction;
    this->_end += direction;
    this->update_dimensions();
}

void Line::set_line_width(unsigned width) {
    this->_line_width = width;
}

const Position Line::end_relative_to_begin() const {
    return this->_end - this->_begin;
}

const Position Line::end_relative_to_parent() const {
    return this->_end;
}

const Position Line::begin() const {
    return this->_begin;
}
