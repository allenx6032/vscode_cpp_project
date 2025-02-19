#pragma once

#include <functional>
#include <map>

#include <SDL.h>

#include "SDL_GUI_position.h"

namespace SDL_GUI {
class Drawable;
/** Abstract class for Objects that do have debugging information to draw. */
class Debuggable {
    /** function to call for drawing debug information */
    std::function<void (SDL_Renderer *, Position, SDL_Rect)> _draw_debug_information;

protected:
    /** Flag that determines whether the debug information has already been initialised */
    bool _debug_information_initialised = false;

    /** Drawables to draw as debug information */
    std::map<Drawable *, std::function<bool()>> _debug_information;

    /** default function to call for drawing debug information */
    void default_draw_debug_information(SDL_Renderer *renderer, Position position,
                                        SDL_Rect parent_clip_rect) const;

    /** Constructor */
    Debuggable();

public:
    /** Destructor */
    ~Debuggable();

    void add_debug_drawable(Drawable *drawable, std::function<bool()> criteria);

    /**
     * Draw the drawables that represent the debug informaiton
     * @param renderer the aplications renderer
     * @param position absolute position to draw debug information
     */
    void draw_debug_information(SDL_Renderer *renderer, Position position,
                                SDL_Rect parent_clip_rect) const;
};
}
