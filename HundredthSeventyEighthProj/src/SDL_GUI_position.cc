#include<SDL_GUI_position.h>

using namespace SDL_GUI;

Position Position::operator+(const Position &p) const {
    return Position(this->_x + p._x, this->_y + p._y);
}

Position Position::operator-(const Position &p) const {
    return Position(this->_x - p._x, this->_y - p._y);
}

Position Position::operator*(const int &s) const {
    return Position(this->_x * s, this->_y * s);
}

Position Position::operator/(const int &s) const {
    return Position(this->_x / s, this->_y / s);
}

Position &Position::operator+=(const Position &p) {
    *this = *this + p;
    return *this;
}

Position &Position::operator-=(const Position &p) {
    *this = *this - p;
    return *this;
}

Position &Position::operator*=(const int &s) {
    *this = *this * s;
    return *this;
}

Position &Position::operator/=(const int &s) {
    *this = *this / s;
    return *this;
}

bool Position::operator==(const Position &p) const {
    return this->_x == p._x and this->_y == p._y;
}

std::ostream& SDL_GUI::operator<<(std::ostream& os, const Position& pos) {
    os << "(" << pos._x << "|" << pos._y << ")";
    return os;
}
