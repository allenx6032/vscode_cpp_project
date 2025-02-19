#pragma once

#include <list>

#include "SDL_GUI_drawable.h"

namespace SDL_GUI {
class Polygon : public Drawable {
    std::list<Position> _points;

    unsigned _line_width = 1;

    virtual Drawable *clone() const override;

protected:
    Polygon(std::string type, Position position = {0,0})
        :Drawable(type, position) {}
public:
    Polygon(Position position = {0,0})
        :Polygon("Polygon", position) {}

    void add_point(Position point);

    void remove_point(Position &point);

    void remove_last_point();

    void set_line_width(unsigned width);

    void draw(SDL_Renderer *renderer, Position position) const override;

    void draw_border(SDL_Renderer *renderer, Position position) const override;
};
}
