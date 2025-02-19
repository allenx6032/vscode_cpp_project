#include <SDL_GUI_interface_model.h>

#include <iostream>
#include <string>

#include <fontconfig/fontconfig.h>

using namespace SDL_GUI;

TTF_Font *InterfaceModel::_font;


InterfaceModel::InterfaceModel(SDL_Renderer *renderer, unsigned window_width,
                               unsigned window_height)
    : _renderer(renderer), _window_width(window_width), _window_height(window_height) {
    /* init font */
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern *pat = FcNameParse((const FcChar8 *)"");
    FcConfigSubstitute(config, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);
    char *font_file;
    FcResult result;
    FcPattern *font = FcFontMatch(config, pat, &result);
    if (font) {
        FcChar8 *file = nullptr;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
            font_file = (char *)file;
        }
    }
    std::cout << "Font: " << font_file << std::endl;
    this->_font = TTF_OpenFont(font_file, 12);
    if (!this->_font) {
        std::cerr << "TTF_Error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    /* init null_drawable */
    this->_null_drawable = new NullDrawable();

    FcPatternDestroy(font);
    FcPatternDestroy(pat);
    FcConfigDestroy(config);
}

InterfaceModel::~InterfaceModel() {
    delete this->_null_drawable;
    delete this->_drawable_root;
}

TTF_Font *InterfaceModel::font() {
    return InterfaceModel::_font;
}

Drawable *InterfaceModel::drawable_root() {
    return this->_drawable_root;
}

const Drawable *InterfaceModel::drawable_root() const {
    return this->_drawable_root;
}

void InterfaceModel::set_drawable_root(Drawable *root) {
    this->_drawable_root = root;
}

SDL_Renderer *InterfaceModel::renderer() {
    return this->_renderer;
}

unsigned InterfaceModel::window_width() const {
    return this->_window_width;
}

unsigned InterfaceModel::window_height() const {
    return this->_window_height;
}

Drawable *InterfaceModel::null_drawable() const {
    return new NullDrawable();
}

bool InterfaceModel::debug_information_drawn() const {
    return this->_debug_information_drawn;
}

void InterfaceModel::toggle_debug_information_drawn() {
    this->_debug_information_drawn = !this->_debug_information_drawn;
}

std::vector<Drawable *> InterfaceModel::find_drawables(std::string attribute) {
    return this->_drawable_root->filter([attribute](Drawable *d){
        return d->has_attribute(attribute);
    });
}

std::vector<const Drawable *> InterfaceModel::find_drawables(std::string attribute) const {
    const Drawable *drawable_root = this->_drawable_root;
    return drawable_root->filter([attribute](const Drawable *d){
        return d->has_attribute(attribute);
    });
}

Drawable * InterfaceModel::find_first_drawable(std::string attribute) {
    std::vector<Drawable *> drawables = this->find_drawables(attribute);
    if (drawables.size() < 1) {
        return nullptr;
    }
    return drawables[0];
}

const Drawable * InterfaceModel::find_first_drawable(std::string attribute) const {
    std::vector<const Drawable *> drawables = this->find_drawables(attribute);
    if (drawables.size() < 1) {
        return nullptr;
    }
    return drawables[0];
}

std::vector<Drawable *> InterfaceModel::find_drawables_at_position(Position position,
                                                                   Drawable *root) {
    if (not root) {
        root = this->_drawable_root;
    }
    return root->find_bottom_up([position](Drawable *d){
        return d->is_inside(position);
    }, true);
}

std::vector<const Drawable *> InterfaceModel::find_drawables_at_position(Position position,
                                                                         Drawable *root) const {
    const Drawable *drawable_root = root;
    if (not drawable_root) {
        drawable_root = this->_drawable_root;
    }
    return drawable_root->find_bottom_up([position](const Drawable *d){
        return d->is_inside(position);
    }, true);
}

Drawable *InterfaceModel::find_first_drawable_at_position(Position position, Drawable *root) {
    if (not root) {
        root = this->_drawable_root;
    }
    return root->find_first_bottom_up([position](Drawable *d){
        return d->is_inside(position);
    }, true);
}

const Drawable *InterfaceModel::find_first_drawable_at_position(Position position,
                                                                Drawable *root) const {
    if (not root) {
        root = this->_drawable_root;
    }
    return root->find_first_bottom_up([position](Drawable *d){
        return d->is_inside(position);
    }, true);
}
