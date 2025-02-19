#include<SDL_GUI_drawable.h>

#include <sstream>

#include <SDL2_gfxPrimitives.h>

#include<SDL_GUI_text.h>
#include<SDL_GUI_wrap_rect.h>
#include <SDL_GUI_interface_model.h>

using namespace SDL_GUI;

const InterfaceModel *Drawable::_interface_model = nullptr;

Drawable::Drawable(std::string type, Position position,
                   std::function<void ()> init_debug_information_callback)
    : Scrollable(position), _type(type) {
    if (init_debug_information_callback) {
        this->_init_debug_information_callback = init_debug_information_callback;
    } else {
        this->_init_debug_information_callback =
            std::bind(&Drawable::default_init_debug_information, this);
    }
}

Drawable::~Drawable() {
    for (Drawable *child: this->_children) {
        delete child;
    }
}

void Drawable::apply_parents_absolute_position(Position parent_position) {
    Position absolute_position = this->position() + parent_position;
    this->set_absolute_position(absolute_position);
    for (Drawable *d: this->_children) {
        d->apply_parents_absolute_position(absolute_position);
    }
}

void Drawable::apply_parents_clip_rect(SDL_Rect parent_clip_rect) {
    this->_parent_clip_rect = parent_clip_rect;
    int new_x = std::max(parent_clip_rect.x, this->_absolute_position._x);
    int new_y = std::max(parent_clip_rect.y, this->_absolute_position._y);
    int new_width = std::min(parent_clip_rect.x + parent_clip_rect.w,
                             static_cast<int>(new_x + this->_width)) - new_x;
    int new_height = std::min(parent_clip_rect.y + parent_clip_rect.h,
                              static_cast<int>(new_y + this->_height)) - new_y;
    SDL_Rect clip_rect = {new_x, new_y, new_width, new_height};

    this->set_clip_rect(clip_rect);
    for (Drawable *d: this->_children) {
        d->apply_parents_clip_rect(clip_rect);
    }
}

void Drawable::set_interface_model(InterfaceModel *interface_model) {
    if (Drawable::_interface_model) {
        return;
    }
    Drawable::_interface_model = interface_model;
}

Drawable *Drawable::parent() {
    return this->_parent;
}

void Drawable::set_parent(Drawable *parent) {
    this->_parent = parent;
    this->apply_parents_absolute_position(parent->_absolute_position);
    this->apply_parents_clip_rect(parent->_clip_rect);
}

std::list<Drawable *> Drawable::children(bool reversed) {
    return reversed ? this->_children_reversed : this->_children;
}

const std::list<Drawable *> Drawable::children(bool reversed) const {
    return reversed ? this->_children_reversed : this->_children;
}

void Drawable::add_child(Drawable *child, bool is_debug_information) {
    this->_children.push_back(child);
    this->_children_reversed.push_front(child);
    if (not is_debug_information) {
        child->init_debug_information();
    }
    child->set_parent(this);
}

void Drawable::add_children(std::vector<Drawable *> children, bool is_debug_information) {
    for (Drawable *child: children) {
        this->add_child(child, is_debug_information);
    }
}

void Drawable::sort_children(std::function<bool (Drawable *, Drawable *)> f) {
    this->_children.sort(f);
    this->_children_reversed.clear();
    for (Drawable *child: this->_children) {
        this->_children_reversed.push_front(child);
    }
}

void Drawable::remove_children(std::function<bool(Drawable *)> f) {
    std::list<Drawable *> new_children;
    std::list<Drawable *> new_children_reversed;
    for (Drawable *child: this->_children) {
        if (f(child)) {
            delete child;
        } else {
            new_children.push_back(child);
            new_children_reversed.push_front(child);
        }
    }
    this->_children = new_children;
    this->_children_reversed = new_children_reversed;
}

void Drawable::remove_all_children() {
    for (Drawable *child: this->_children) {
        delete child;
    }
    this->_children.clear();
    this->_children_reversed.clear();
}

std::vector<Drawable *> Drawable::find(std::function<bool (Drawable *)> f) {
    std::vector<Drawable *> drawables;
    if (f(this)) {
        drawables.push_back(this);
    }
    for (Drawable *d: this->children(true)) {
        std::vector<Drawable *> childs_drawables = d->find(f);
        drawables.insert(drawables.end(), childs_drawables.begin(), childs_drawables.end());
    }
    return drawables;
}

std::vector<Drawable *> Drawable::find(std::string attribute) {
    return this->find(
        [attribute](Drawable *d) {
            return d->has_attribute(attribute);
        });
}

std::vector<const Drawable *> Drawable::find(std::function<bool (const Drawable *)> f) const {
    std::vector<const Drawable *> drawables;
    if (f(this)) {
        drawables.push_back(this);
    }
    for (const Drawable *d: this->children(true)) {
        std::vector<const Drawable *> childs_drawables = d->find(f);
        drawables.insert(drawables.end(), childs_drawables.begin(), childs_drawables.end());
    }
    return drawables;
}

std::vector<Drawable *> Drawable::find_bottom_up(std::function<bool (Drawable *)> f,
                                                 bool reversed) {
    std::vector<Drawable *> drawables;
    for (Drawable *d: this->children(reversed)) {
        std::vector<Drawable *> childs_drawables = d->find(f);
        drawables.insert(drawables.end(), childs_drawables.begin(), childs_drawables.end());
    }
    if (f(this)) {
        drawables.push_back(this);
    }
    return drawables;
}

std::vector<const Drawable *> Drawable::find_bottom_up(std::function<bool (const Drawable *)> f,
                                                       bool reversed) const {
    std::vector<const Drawable *> drawables;
    for (const Drawable *d: this->children(reversed)) {
        std::vector<const Drawable *> childs_drawables = d->find(f);
        drawables.insert(drawables.end(), childs_drawables.begin(), childs_drawables.end());
    }
    if (f(this)) {
        drawables.push_back(this);
    }
    return drawables;
}

Drawable *Drawable::find_first(std::function<bool (Drawable *)> f) {
    if (f(this)) {
        return this;
    }
    Drawable *found = nullptr;
    for (Drawable *d: this->_children) {
        found = d->find_first(f);
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

Drawable *Drawable::find_first(std::string attribute) {
    return this->find_first(
        [attribute](Drawable *d) {
            return d->has_attribute(attribute);
        });
}

Drawable *Drawable::find_first_bottom_up(std::function<bool (Drawable *)> f, bool reversed) {
    Drawable *found = nullptr;
    for (Drawable *d: this->children(reversed)) {
        found = d->find_first_bottom_up(f);
        if (found != nullptr) {
            return found;
        }
    }
    if (f(this)) {
        return this;
    }
    return nullptr;
}

std::vector<Drawable *> Drawable::filter(std::function<bool (Drawable *)> f) {
    std::vector<Drawable *> filtered;
    if (f(this)) {
        filtered.push_back(this);
    }
    for (Drawable *d: this->_children) {
        std::vector<Drawable *> filtered_child = d->filter(f);
        filtered.insert(filtered.end(), filtered_child.begin(), filtered_child.end());
    }
    return filtered;
}

std::vector<const Drawable *> Drawable::filter(std::function<bool (const Drawable *)> f) const {
    std::vector<const Drawable *> filtered;
    if (f(this)) {
        filtered.push_back(this);
    }
    for (const Drawable *d: this->_children) {
        std::vector<const Drawable *> filtered_child = d->filter(f);
        filtered.insert(filtered.end(), filtered_child.begin(), filtered_child.end());
    }
    return filtered;
}

void Drawable::map(std::function<void (Drawable *)> f, bool reversed) {
    f(this);
    for (Drawable *d: this->children(reversed)) {
        d->map(f, reversed);
    }
}

void Drawable::bottom_up_map(std::function<void (Drawable *)> f, bool reversed) {
    for (Drawable *d: this->children(reversed)) {
        d->map(f, reversed);
    }
    f(this);
}

void Drawable::default_init_debug_information() {
    /* Position Text */
    std::stringstream position_string;
    position_string << this->position();
    std::stringstream attribute_string;
    for (std::string attribute: this->_attributes) {
        attribute_string << attribute;
    }
    if (attribute_string.str().empty()) {
        attribute_string << "--noname--";
    }

    WrapRect *rect = new WrapRect();
    rect->_style._color = RGB(255, 255, 255, 150);
    rect->_style._has_background = true;
    this->add_debug_drawable(rect,
        [this](){
            return this->_interface_model->debug_information_drawn();
        });

    Text *position_text = new Text(InterfaceModel::font(), position_string.str());
    position_text->set_position({3,3});
    position_text->add_attribute("debug");
    Drawable *drawable = this;
    position_text->add_recalculation_callback([drawable, position_text](Drawable *){
            std::stringstream position_string;
            position_string << drawable->absolute_position();
            position_text->set_text(position_string.str());
        });
    rect->add_child(position_text, true);

    Text *attribute_text = new Text(InterfaceModel::font(), attribute_string.str());
    /* TODO: get rid of magic numbers */
    attribute_text->set_position({3,16});
    attribute_text->add_attribute("debug");
    rect->add_child(attribute_text, true);
}

void Drawable::init_debug_information() {
    if (this->_debug_information_initialised) {
        return;
    }
    this->_init_debug_information_callback();
    this->_debug_information_initialised = true;
}

void Drawable::hook_post_move(Position offset) {
    this->apply_parents_clip_rect(this->_parent_clip_rect);
    for (Drawable *child: this->_children) {
        child->map([offset](Drawable *d){
            d->move_absolute(offset);
        });
    }
}

void Drawable::hook_post_resize(unsigned width, unsigned height) {
    this->_clip_rect.w = width;
    this->_clip_rect.h = height;
    this->apply_parents_clip_rect(this->_parent_clip_rect);
}

void Drawable::hook_post_scroll(Position scroll_offset) {
    for (Drawable *child: this->_children) {
        child->move(scroll_offset);
    }
}

void Drawable::add_recalculation_callback(std::function<void(Drawable *)> callback) {
    this->_recalculation_callbacks.push_back(callback);
}

void Drawable::recalculate() {
    this->update();
    for (std::function<void(Drawable *)> callback: this->_recalculation_callbacks) {
        callback(this);
    }
}

void Drawable::render(SDL_Renderer *renderer, Position parent_position, SDL_Rect parent_clip_rect,
                      bool hidden, bool is_debug_information) const {
    this->hook_pre_render();
    Position position = parent_position + this->_position;
    if (hidden || this->is_hidden()) {
        return;
    }
    SDL_RenderSetClipRect(renderer, &parent_clip_rect);
    this->draw(renderer, position);

    SDL_Rect clip_rect = is_debug_information ? parent_clip_rect : this->_clip_rect;

    /* draw children */
    for (Drawable *child: this->_children) {
        child->render(renderer, position, clip_rect, false, is_debug_information);
    }

    SDL_RenderSetClipRect(renderer, &parent_clip_rect);
    this->draw_border(renderer, position);
    if (not is_debug_information) {
        this->draw_debug_information(renderer, position, this->_clip_rect);
    }
}

void Drawable::draw_border(SDL_Renderer *renderer, Position position) const {
    if (not this->_style._has_border) {
        return;
    }
    for (int i = 0; i < static_cast<int>(this->_style._border_width); ++i) {
        const RGB &c = this->_style._border_color;
        rectangleRGBA(renderer, position._x + i, position._y + i,
                      position._x + this->_width - i, position._y + this->_height - i,
                      c._r, c._g, c._b, c._a);
    }
}

void Drawable::show() {
    this->_style._hidden = false;
}

void Drawable::hide() {
    this->_style._hidden = true;
}

bool Drawable::is_hidden() const {
    return this->_style._hidden;
}

Drawable *Drawable::deepcopy() const {
    Drawable *new_root = this->clone();
    new_root->_children.clear();
    new_root->_children_reversed.clear();
    for (Drawable *child: this->_children) {
        new_root->add_child(child->deepcopy());
    }
    return new_root;
}

Drawable *NullDrawable::clone() const {
    return new NullDrawable(*this);
}

void NullDrawable::draw(SDL_Renderer *renderer, Position position) const {
    (void) renderer;
    (void) position;
}

