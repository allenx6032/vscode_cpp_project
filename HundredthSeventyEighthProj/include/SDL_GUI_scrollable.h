#pragma once

#include <functional>

#include "SDL_GUI_position.h"
#include "SDL_GUI_positionable.h"

namespace SDL_GUI {
/**
 * Abstract class for objects that can be scrolled.
 * TODO: This should have some kind of bounds checking on the actual scrolling.
 */
class Scrollable: public Positionable {
protected:
    Position _scroll_position;  /**< current offset from the original position */

    /** flag describing whether scrolling on x axis is enabled */
    bool _scrolling_x_enabled = false;

    /** flag describing whether scrolling on y axis is enabled */
    bool _scrolling_y_enabled = false;

    /** flag describing whether scroll limits are checked to not be exceeded */
    bool _enforcing_limits = false;

    Position _min_limit;

    Position _max_limit;

    /** Constructor */
    Scrollable(Position position);

    /**
     * hook to execute after changing the scroll position
     * @param scroll_offset scrolling that happened
     */
    virtual void hook_post_scroll(Position scroll_offset) {(void)scroll_offset;};
public:
    /**
     * Getter for _scroll_position
     * @return  this->_scroll_position
     */
    Position scroll_position() const;

    Position min_limit() const;

    Position max_limit() const;

    bool enforcing_limits() const;

    /**
     * Setter for _scroll_position
     * @param offset position to set _scroll_position to
     */
    void set_scroll_position(Position offset);

    /**
     * Setter for the x value of _scroll_position
     * @param x x value of _scroll_position
     */
    void set_scroll_position_x(int x);

    /**
     * Setter for the y value of _scroll_position
     * @param y y value of _scroll_position
     */
    void set_scroll_position_y(int y);

    void set_enforcing_limits(bool enforce_limit);

    void set_min_limit(Position min);
    void set_max_limit(Position max);

    /**
     * decrease x coordinate of scroll position
     * @param value value
     */
    void scroll_left(int value = 10);

    /**
     * increase x coordinate of scroll position
     * @param value value
     */
    void scroll_right(int value = 10);

    /**
     * decrease y coordinate of scroll position
     * @param value value
     */
    void scroll_up(int value = 10);

    /**
     * increase y coordinate of scroll position
     * @param value value
     */
    void scroll_down(int value = 10);

    /** enable scrolling in x and y direction */
    void enable_scrolling();

    /** disable scrolling in x and y direction */
    void disable_scrolling();

    /** enable scrolling in x direction */
    void enable_scrolling_x();

    /** disable scrolling in x direction */
    void disable_scrolling_x();

    /** enable scrolling in y direction */
    void enable_scrolling_y();

    /** disable scrolling in x direction */
    void disable_scrolling_y();
};
}
