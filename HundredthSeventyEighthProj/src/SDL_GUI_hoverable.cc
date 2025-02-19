#include<SDL_GUI_hoverable.h>

using namespace SDL_GUI;

void Hoverable::set_relative_mouse_position(Position relative_mouse_position) {
    this->_relative_mouse_position = relative_mouse_position;
}

void Hoverable::set_mouse_is_inside(bool mouse_is_inside) {
    this->_mouse_is_inside = mouse_is_inside;
}

void Hoverable::set_hover_distance(int hover_distance) {
    this->_hover_distance = hover_distance;
}

Position Hoverable::relative_mouse_position() const {
    return this->_relative_mouse_position;
}

bool Hoverable::mouse_is_inside() const {
    return this->_mouse_is_inside;
}

int Hoverable::hover_distance() const {
    return this->_hover_distance;
}
