#include<SDL_GUI_scrollable.h>

#include <algorithm>

using namespace SDL_GUI;

Scrollable::Scrollable(Position position) :
    Positionable(position) {}

Position Scrollable::scroll_position() const {
    return this->_scroll_position;
}

Position Scrollable::min_limit() const {
    return this->_min_limit;
}

Position Scrollable::max_limit() const {
    return this->_max_limit;
}

bool Scrollable::enforcing_limits() const {
    return this->_enforcing_limits;
}

void Scrollable::set_scroll_position(Position target) {
    if (target == this->_scroll_position) {
        return;
    }

    if (this->_scrolling_x_enabled) {
        if (this->_enforcing_limits) {
            target._x = std::max(std::min(target._x, this->_max_limit._x), this->_min_limit._x);

        }
    } else {
        target._x = this->_scroll_position._x;
    }

    if (this->_scrolling_y_enabled) {
        if (this->_enforcing_limits) {
            target._y = std::max(std::min(target._y, this->_max_limit._y), this->_min_limit._y);
        }
    } else {
        target._y = this->_scroll_position._y;
    }

    Position scrolled = target - this->_scroll_position;
    this->_scroll_position = target;
    this->hook_post_scroll(scrolled);
}

void Scrollable::set_scroll_position_x(int x) {
    this->set_scroll_position({x, this->_scroll_position._y});
}

void Scrollable::set_scroll_position_y(int y) {
    this->set_scroll_position({this->_scroll_position._x, y});
}

void Scrollable::set_enforcing_limits(bool enforcing_limits) {
    this->_enforcing_limits = enforcing_limits;
}

void Scrollable::set_min_limit(Position min) {
    this->_min_limit = min;
}

void Scrollable::set_max_limit(Position max) {
    this->_max_limit = max;
}


void Scrollable::scroll_left(int value) {
    this->set_scroll_position_x(this->_scroll_position._x - value);
}

void Scrollable::scroll_right(int value) {
    this->set_scroll_position_x(this->_scroll_position._x + value);
}

void Scrollable::scroll_up(int value) {
    this->set_scroll_position_y(this->_scroll_position._y - value);
}

void Scrollable::scroll_down(int value) {
    this->set_scroll_position_y(this->_scroll_position._y + value);
}

void Scrollable::enable_scrolling() {
    this->_scrolling_x_enabled = true;
    this->_scrolling_y_enabled = true;
}

void Scrollable::disable_scrolling() {
    this->_scrolling_x_enabled = false;
    this->_scrolling_y_enabled = false;
}

void Scrollable::enable_scrolling_x() {
    this->_scrolling_x_enabled = true;
}

void Scrollable::disable_scrolling_x() {
    this->_scrolling_x_enabled = false;
}

void Scrollable::enable_scrolling_y() {
    this->_scrolling_y_enabled = true;
}

void Scrollable::disable_scrolling_y() {
    this->_scrolling_y_enabled = false;
}

