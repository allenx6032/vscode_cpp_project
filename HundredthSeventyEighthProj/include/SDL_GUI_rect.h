#pragma once

#include "SDL_GUI_drawable.h"
#include "SDL_GUI_rgb.h"

namespace SDL_GUI {
/** primitive for drawing a rectangle */
class Rect : public Drawable {
protected:
    /**
     * Constructor
     * @param type type of drawable
     * @param position local position inside parent drawable
     * @param width width of rect
     * @param height height of rect
     */
    Rect(std::string type, Position position = {0, 0}, unsigned width = 0, unsigned height = 0);

    virtual Drawable *clone() const override;
public:
    /**
     * Constructor
     * @param position local position inside parent drawable
     * @param width width of rect
     * @param height height of rect
     */
    Rect(Position position = {0, 0}, unsigned width = 0, unsigned height = 0)
        : Rect("Rect", position, width, height) {}


    void draw(SDL_Renderer *renderer, Position position) const override;
};
}
