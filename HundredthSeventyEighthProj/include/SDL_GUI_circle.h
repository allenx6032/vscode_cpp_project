#pragma once

#include "SDL_GUI_drawable.h"

namespace SDL_GUI {
/** primitive for drawing a circle */
class Circle : public Drawable {
protected:
    unsigned _radius;

    virtual Drawable *clone() const override;
public:
    Circle(Position center = {0,0}, unsigned radius = 0)
        : Drawable("Circle", center), _radius(radius)  {}

    void draw(SDL_Renderer *renderer, Position position) const override;
};
}
