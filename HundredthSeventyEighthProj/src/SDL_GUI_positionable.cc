#include<SDL_GUI_positionable.h>

using namespace SDL_GUI;

Positionable::Positionable(Position position, Position absolute_position, unsigned width,
                           unsigned height) :
    _position(position),
    _absolute_position(absolute_position),
    _width(width),
    _height(height) {
    this->init();
}

void Positionable::init() {
    this->_clip_rect = {this->_position._x, this->_position._y,
                        static_cast<int>(this->_width), static_cast<int>(this->_height)};
}


void Positionable::set_position(Position position) {
    Position diff = position - this->_position;
    this->_position = position;
    this->_absolute_position += diff;
    this->hook_post_move(diff);
}

void Positionable::set_absolute_position(Position position) {
    this->_absolute_position = position;
}

void Positionable::set_clip_rect(SDL_Rect clip_rect) {
    this->_clip_rect = clip_rect;
}

void Positionable::move(Position position) {
    this->_position += position;
    this->_absolute_position += position;
    this->hook_post_move(position);
}

void Positionable::move_absolute(Position position) {
    this->_absolute_position += position;
}

void Positionable::set_x(int x) {
    int diff = x - this->_position._x;
    this->_position._x = x;
    this->_absolute_position._x += diff;
    this->hook_post_move({diff, 0});
}

void Positionable::set_absolute_x(int x) {
    this->_absolute_position._x = x;
}

void Positionable::set_y(int y) {
    int diff = y - this->_position._y;
    this->_position._y = y;
    this->_absolute_position._y += diff;
    this->hook_post_move({0, diff});
}

void Positionable::set_absolute_y(int y) {
    this->_absolute_position._y =y;
}

void Positionable::set_width(unsigned width) {
    this->_width = width;
    this->hook_post_resize(width, 0);
}

void Positionable::set_height(unsigned height) {
    this->_height = height;
    this->hook_post_resize(0, height);
}

Position Positionable::position() const {
    return this->_position;
}

Position Positionable::absolute_position() const {
    return this->_absolute_position;
}

SDL_Rect Positionable::clip_rect() const {
    return this->_clip_rect;
}

int Positionable::x() const {
    return this->_position._x;
}

int Positionable::y() const {
    return this->_position._y;
}

unsigned Positionable::width() const {
    return this->_width;
}

unsigned Positionable::height() const {
    return this->_height;
}


bool Positionable::is_inside(Position position) const {
    return position._x > this->_absolute_position._x &&
           position._y > this->_absolute_position._y &&
           position._x < this->_absolute_position._x + static_cast<int>(this->_width) &&
           position._y < this->_absolute_position._y + static_cast<int>(this->_height);
}

bool Positionable::is_inside_clip_rect(Position position) const {
    return position._x > this->_clip_rect.x &&
           position._y > this->_clip_rect.y &&
           position._x < this->_clip_rect.x + static_cast<int>(this->_clip_rect.w) &&
           position._y < this->_clip_rect.y + static_cast<int>(this->_clip_rect.h);
}
