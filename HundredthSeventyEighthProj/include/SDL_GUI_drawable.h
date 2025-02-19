#pragma once

#include <functional>
#include <list>

#include <SDL.h>

#include "SDL_GUI_attributable.h"
#include "SDL_GUI_debuggable.h"
#include "SDL_GUI_hoverable.h"
#include "SDL_GUI_position.h"
#include "SDL_GUI_positionable.h"
#include "SDL_GUI_scrollable.h"
#include "SDL_GUI_style.h"

namespace SDL_GUI {
class InterfaceModel;
/** base class for Objects that get rendered.  */
class Drawable : public Hoverable, public Scrollable, public Attributable,
                 public Debuggable {
    Drawable *_parent;                          /**< parent Drawable in drawable tree */
    std::list<Drawable *> _children;            /**< list of child drawables in drawable tree */
    std::list<Drawable *> _children_reversed;   /**< reversed list of childs */
protected:
    static const InterfaceModel *_interface_model;

    /** function to call for debug information initialisation */
    std::function<void ()> _init_debug_information_callback;

    /** default debug information initialisation */
    void default_init_debug_information();

    /**
     * Constructor
     * @param type name of drawable subclass
     * @param position local position inside parent drawable
     * @param init_debug_information_callback function to call for debug information initialisation
     */
    Drawable(std::string type, Position position = {0,0},
             std::function<void ()> init_debug_information_callback = nullptr);

    /** vector of callbacks for recalculation */
    std::vector<std::function<void(Drawable *)>> _recalculation_callbacks;

    /** function to call before the drawable gets rendered */
    virtual void hook_pre_render() const {}

    /** @copydoc Positionable::hook_post_move(Position) */
    void hook_post_move(Position offset) override;

    /** @copydoc Positionable::hook_post_resize(unsigned, unsigned) */
    void hook_post_resize(unsigned width, unsigned height) override;

    /** @copydoc Scrollable::hook_post_scroll(Position) */
    void hook_post_scroll(Position scroll_offset) override;

    /**
     * recalculate absolute position of this and all childs with regard to the absolute position of
     * the parent in the drawable tree
     * @param parent_position absolute position of parent
     */
    void apply_parents_absolute_position(Position parent_position);

    /**
     * recalculate the clip rect of this and all childs with regard to the clip rect of the parent
     * in the drawable tree
     * @param parent_clip_rect clip rect of parent
     */
    void apply_parents_clip_rect(SDL_Rect parent_clip_rect);

    virtual Drawable *clone() const = 0;

public:
    const std::string _type;    /**< Typename of this drawable */
    Style _style;       /**< Style */

    /** Default destructor */
    virtual ~Drawable();

    static void set_interface_model(InterfaceModel *interface_model);

    /**
     * Getter for _parent
     * @return parent
     */
    Drawable *parent();

    /**
     * Setter for _parent
     * This also updates the absolute position of this and all childs
     * @param parent Drawable to set as parent in drawable tree
     */
    void set_parent(Drawable *parent);

    /**
     * getter for the list of children
     * @param reversed flag to determine whether the list should be returned in reversed order
     * @return list of children
     */
    std::list<Drawable *> children(bool reversed = false);

    /** @copydoc children(bool) */
    const std::list<Drawable *> children(bool reversed = false) const;

    /**
     * add a given drawable as child
     * @param child drawable to add as child
     * @param is_debug_information flag that determines if the added child is debug information.
     */
    void add_child(Drawable *child, bool is_debug_information = false);

    /**
     * add a vector of drawables as children
     * @param children drawables to add as children
     * @param is_debug_information flag that determines if the added child is debug information.
     */
    void add_children(std::vector<Drawable *> children, bool is_debug_information = false);

    /**
     * change the order of all children with a sorting function
     */
    void sort_children(std::function<bool (Drawable *, Drawable *)> f);

    /**
     * remove all children that hold a given condition
     */
    void remove_children(std::function<bool(Drawable *)> f);

    /**
     * remove all the children
     */
    void remove_all_children();

    /**
     * find the all drawable DFS that hols a given condition
     * @param f condition
     * @return drawables that hold for the given condition
     */
    std::vector<Drawable *> find(std::function<bool (Drawable *)> f);

    /**
     * find the all drawable DFS that holds a given attribute
     * @param attribute attribute to filter for
     * @return drawables with given attribute
     */
    std::vector<Drawable *> find(std::string attribute);

    /** @copydoc find(std::function<bool (Drawable *)>) */
    std::vector<const Drawable *> find(std::function<bool (const Drawable *)> f) const;

    /**
     * find the all drawable DFS reversed that holds a given condition
     * @param f condition
     * @return drawables that hold for the given condition
     */
    std::vector<Drawable *> find_bottom_up(std::function<bool (Drawable *)> f,
                                           bool reversed = false);

    /** @copydoc find_bottom_up(std::function<bool (Drawable *)>) */
    std::vector<const Drawable *> find_bottom_up(std::function<bool (const Drawable *)> f,
                                                 bool reversed = false) const;

    /**
     * find the first drawable DFS that holds a given condition
     * @param f condition
     * @return first drawable that holds for the given condition
     */
    Drawable *find_first(std::function<bool (Drawable *)> f);

    /**
     * find the first drawable DFS that holds a given attribute
     * @param attribute attribute to filter for
     * @return first drawable with given attribute
     */
    Drawable *find_first(std::string attribute);

    /**
     * find the first drawable DFS reversed that holds a given condition
     * @param f condition
     * @return first drawable that holds for the given condition
     */
    Drawable *find_first_bottom_up(std::function<bool (Drawable *)> f, bool reversed = false);

    /**
     * create list of Drawables by filtering this subtree
     * @param f filter to apply
     * @return list of filtered drawables
     */
    std::vector<Drawable *> filter(std::function<bool (Drawable *)> f);

    /** @copydoc filter(std::function<bool (Drawable *)>) */
    std::vector<const Drawable *> filter(std::function<bool (const Drawable *)> f) const;

    /**
     * apply a function recursively to this and all children
     * @param f function to apply
     * @param reversed flag that determines the order of child processing. If True, the list of
     *   children gets reversed before applying f
     */
    void map(std::function<void (Drawable *)> f, bool reversed = false);

    /**
     * apply a function recursively to this and all children from bottom to topof the tree
     * @param f function to apply
     * @param reversed flag that determines the order of child processing. If True, the list of
     *   children gets reversed before applying f
     */
    void bottom_up_map(std::function<void (Drawable *)> f, bool reversed = false);

    /**
     * apply a function recursively to this and all children propagating a value
     * @tparam R Type of the propagated value
     * @param f function to apply
     * @param value initial value
     * @param reversed flag to determine the order of child processing. If true, the list of
     * children gets reversed before applying f
     */
    template<typename R>
    void reduce(std::function<R (Drawable *, R)> f, R value, bool reversed = false) {
        R this_value = f(this, value);
        for (Drawable *d: this->children(reversed)) {
            d->reduce(f, this_value, reversed);
        }
    }

    /** @copydoc reduce(std::function<R (Drawable *, R)>, R, bool reversed) */
    template<typename R>
    void reduce(std::function<R (const Drawable *, R)> f, R value, bool reversed = false) const {
        R this_value = f(this, value);
        for (const Drawable *d: this->children(reversed)) {
            d->reduce(f, this_value, reversed);
        }
    }

    /**
     * apply a function recursively reversed, from bottom to top of the tree aggregating the return
     * value
     * @tparam R type of the propagated value
     * @param f function to apply
     * @param value initial value
     * @param aggregate function to aggregate return values from all children
     * @param reversed flag to determine the order of child processing. If true, the list of
     * children gets reversed before applying f
     * @returns last aggregated propagation value
     */
    template<typename R>
    R bottom_up_reduce(std::function<R (Drawable *, R)> f, R value,
                       std::function<R (std::vector<R>)> aggregate, bool reversed = false) {
        if (this->_children.empty()) {
            return f(this, value);
        }
        std::vector<R> vec;
        for (const Drawable *d: this->children(reversed)) {
            vec.push_back(d->reduce(f, value, aggregate, reversed));
        }
        return f(this, aggregate(vec));
    }

    /** initialise debug information */
    void init_debug_information();

    /**
     * render the subtree this drawable is root of.
     * @param renderer The applications renderer
     * @param parent_position position of parent Drawable
     * @param parent_clip_rect clip rectanlge of parent
     * @param hidden flag that determines whether any parent is hidden. Rendering does not take
     * place if true.
     */
    void render(SDL_Renderer *renderer, Position parent_position, SDL_Rect parent_clip_rect,
                bool hidden, bool is_debug_information = false) const;

    /**
     * draw this Object. gets called by render()
     * @param renderer renderer to draw on
     * @param position global offset in window
     */
    virtual void draw(SDL_Renderer *renderer, Position position) const = 0;

    /**
     * draw this Objects border. If it should have one. gets called by render()
     * @param renderer renderer to draw on
     * @param position global offset in window
     */
    virtual void draw_border(SDL_Renderer *renderer, Position position) const;

    /**
     * add a callback function for data binding
     * @param callback callback function that gets executed every tick
     */
    void add_recalculation_callback(std::function<void(Drawable *)> callback);

    /** recalculate attributes of this drawable by calling all of the recalculation callbacks */
    void recalculate();

    /** Update this. This function gets called once every tick */
    virtual void update() {}

    /** Set the style's `hidden` value to false */
    void show();

    /** Set the style's `hidden` value to true */
    void hide();

    /**
     * Getter for the style's `hidden` value
     * @return this->_style._hidden
     */
    bool is_hidden() const;

    /** copy the subtree defined by this drawable */
    Drawable *deepcopy() const;
};


/** Drawable that does nothing. This is a tombstone. */
class NullDrawable : public Drawable {
protected:
    virtual Drawable *clone() const override;
public:
    /** Default destructor */
    ~NullDrawable() = default;

    /** Constructor */
    NullDrawable() : Drawable("Null") {}

    virtual void draw(SDL_Renderer *renderer, Position position) const;
};
}
