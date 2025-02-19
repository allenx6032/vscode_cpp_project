#pragma once

#include <SDL_ttf.h>

#include "SDL_GUI_model_base.h"
#include "SDL_GUI_drawable.h"

namespace SDL_GUI {
/** Model for all the Data related to the grafical interface that actually gets rendered */
class InterfaceModel : public ModelBase {
protected:
    static TTF_Font *_font;     /**< Font to use for text */
    Drawable *_drawable_root;   /**< Tree of Drawables that get rendered */
    SDL_Renderer *_renderer;    /**< The applications renderer */
    unsigned _window_width;     /**< applications windows width */
    unsigned _window_height;    /**< applications windows height */
    Drawable *_null_drawable;   /**< drawable tombstone */

    /** flag that demetermines whether position and attributes position of drawables will be
     * shown */
    bool _debug_information_drawn = false;
public:
    /**
     * Constructor
     * @param renderer The applications renderer
     * @param window_width The applications windows width
     * @param window_height The applications windows height
     */
    InterfaceModel(SDL_Renderer *renderer, unsigned window_width, unsigned window_height);

    /** Destructor */
    ~InterfaceModel();

    /**
     * Getter for _font
     * @return this->_font
     */
    static TTF_Font *font();

    /**
     * Getter for _drawable_root
     * @return this->_drawable_root
     */
    Drawable *drawable_root();

    /** @copydoc drawable_root() */
    const Drawable *drawable_root() const;

    /**
     * Setter for _drawable_root
     * @param root drawable root
     */
    void set_drawable_root(Drawable *root);

    /**
     * Getter for _renderer
     * @return this->_renderer
     */
    SDL_Renderer *renderer();

    /**
     * Getter for _window_width
     * @return this->_window_width
     */
    unsigned window_width() const;

    /**
     * Getter for _window_height
     * @return this->_window_height
     */
    unsigned window_height() const;

    /**
     * Getter for _null_drawable
     * @return this->_null_drawable
     */
    Drawable *null_drawable() const;

    /**
     * Getter fot _debug_information_drawm
     * @return this->_debug_information_drawn
     */
    bool debug_information_drawn() const;

    /** toggle this->_draw_debug_information */
    void toggle_debug_information_drawn();

    /**
     * find all Drawables in _drawable_root with a certain attribute
     * @param attribute attribute to find
     * @return vector of found drawables
     */
    std::vector<Drawable *> find_drawables(std::string attribute);

    /** @copydoc find_drawables(std::string) */
    std::vector<const Drawable *> find_drawables(std::string attribute) const;

    /**
     * find first Drawable in _drawable_root with a certain attribute
     * @param attribute attribute to find
     * @return first drawable with the given attribute
     */
    Drawable *find_first_drawable(std::string attribute);

    /** @copydoc find_first_drawable(std::string) */
    const Drawable *find_first_drawable(std::string attribute) const;

    /**
     * get all drawables in tree whose bounding box surrounds a given position
     * This gets the first drawable the mouse hovers over
     * @param position position to look at
     * @param root drawable subtree or nullptr for whole tree
     * @return drawables at given position
     */
    std::vector<Drawable *> find_drawables_at_position(Position position, Drawable *root = nullptr);

    /** @copydoc find_drawables_at_position(Position) */
    std::vector<const Drawable *> find_drawables_at_position(Position position,
                                                             Drawable *root = nullptr) const;

    /**
     * get bottommost drawable in tree whose bounding box surrounds a given position
     * This gets the first drawable the mouse hovers over
     * @param position position to look at
     * @param root drawable subtree or nullptr for whole tree
     * @return first drawable at given position
     */
    Drawable *find_first_drawable_at_position(Position position, Drawable *root = nullptr);

    /** @copydoc find_first_drawable_at_position(Position) */
    const Drawable *find_first_drawable_at_position(Position position, Drawable *root = nullptr) const;
};
}
