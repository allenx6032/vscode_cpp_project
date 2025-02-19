#include<SDL_GUI_polygon.h>

#include <SDL2_gfx/SDL2_gfxPrimitives.h>

using namespace SDL_GUI;

Drawable *Polygon::clone() const {
    return new Polygon(*this);
}

void Polygon::add_point(Position point) {
    this->_points.push_back(point);
}

void Polygon::remove_point(Position &point) {
    this->_points.remove(point);
}

void Polygon::remove_last_point() {
    this->_points.pop_back();
}

void Polygon::set_line_width(unsigned width) {
    this->_line_width = width;
}

void Polygon::draw(SDL_Renderer *renderer, Position position) const {
    if (this->_points.empty()) {
        return;
    }
    if (not this->_style._has_background) {
        return;
    }
    int size = this->_points.size();
    Sint16 *xs = new Sint16[size];
    Sint16 *ys = new Sint16[size];
    int i = 0;
    for (Position point: this->_points) {
        point += position;
        xs[i] = point._x;
        ys[i] = point._y;
        ++i;
    }
    const RGB &c = this->_style._color;
    filledPolygonRGBA(renderer, xs, ys, size, c._r, c._g, c._b, c._a);
    delete[] xs;
    delete[] ys;
}

void Polygon::draw_border(SDL_Renderer *renderer, Position position) const {
    if (not this->_style._has_border) {
        return;
    }
    if (this->_points.empty()) {
        return;
    }
    Position begin = position + this->_points.back();
    const RGB &c = this->_style._border_color;
    for (Position end: this->_points) {
        end += position;
        if (this->_line_width == 1) {
            aalineRGBA(renderer, begin._x, begin._y, end._x, end._y,
                    c._r, c._g, c._b, c._a);
        } else {
            thickLineRGBA(renderer, begin._x, begin._y, end._x, end._y, this->_line_width,
                        c._r, c._g, c._b, c._a);
        }
        begin = end;
    }
}
