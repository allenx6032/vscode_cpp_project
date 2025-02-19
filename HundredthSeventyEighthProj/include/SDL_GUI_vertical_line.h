#pragma once

#include "SDL_GUI_drawable.h"

namespace SDL_GUI {
/** primitive for drawing a vertical line */
class VerticalLine : public Drawable {
protected:
    virtual Drawable *clone() const override;
public:
    /** Constructor */
    VerticalLine() : Drawable("VerticalLine") {}

    void draw(SDL_Renderer *renderer, Position position) const override;
};
}
