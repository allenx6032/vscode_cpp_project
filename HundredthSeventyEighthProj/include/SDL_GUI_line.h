#pragma once

#include "SDL_GUI_drawable.h"

namespace SDL_GUI {
/** primitive for drawing a line */
class Line : public Drawable {
protected:
    Position _begin;   /**< begin of line */
    Position _end; /**< end of line */


    unsigned _line_width = 1;

    void update_dimensions();

    virtual Drawable *clone() const override;
public:
    /**
     * Constructor
     * @param begin begin of line
     * @param end end of line relative to begin
     */
    Line(Position begin = {0,0}, Position end = {0,0});

    void draw(SDL_Renderer *renderer, Position position) const override;

    /**
     * setter for _end
     * @param position end relative to begin
     */
    void set_end_relative_to_begin(Position position);

    /**
     * setter for end in relation to the parent drawable
     * @param position end relative to parent drawable
     */
    void set_end(Position position);

    /**
     * Setter for begin
     * @param position begin
     */
    void set_begin(Position position);

    /** @copydoc Positionable::move(Position) */
    void move(Position direction) override;

    void set_line_width(unsigned width);

    /**
     * Getter for _end
     * @return this->_end
     */
    const Position end_relative_to_begin() const;

    /**
     * Getter for _end relative to parent drawable
     * @return end relative to parent drawable
     */
    const Position end_relative_to_parent() const;

    /**
     * Getter for _begin
     * @return this->_begin
     */
    const Position begin() const;
};
}
