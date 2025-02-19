#pragma once

#include <SDL_ttf.h>

#include "SDL_GUI_drawable.h"
#include "SDL_GUI_tree_builder.h"

// macOS defined OVERFLOW, but it is used here as an enum case name
#undef OVERFLOW

namespace SDL_GUI {
/** types of Drawables to build */
enum class Type {
    RECT,
    TEXT,
    VERTICAL_LINE,
    NONE,
};

/** mapping from xml type to Type */
static const std::map<std::string, Type> TYPE_MAP = {
    {"rect", Type::RECT},
    {"text", Type::TEXT},
    {"vertical-line", Type::VERTICAL_LINE},
    {"", Type::NONE},
};

/** attribute types */
enum class StyleType {
    BORDER,
    BACKGROUND,
    COLOR,
    WIDTH,
    HEIGHT,
    POSITION_X,
    POSITION_Y,
    OVERFLOW,
};

/** mapping from xml attributy to StyleType */
static const std::map<std::string, StyleType> STYLE_TYPE_MAP = {
    {"border", StyleType::BORDER},
    {"bg", StyleType::BACKGROUND},
    {"color", StyleType::COLOR},
    {"width", StyleType::WIDTH},
    {"height", StyleType::HEIGHT},
    {"x", StyleType::POSITION_X},
    {"y", StyleType::POSITION_Y},
    {"overflow", StyleType::OVERFLOW},
};

/** Treebuilder that builds a tree of drawables */
class DrawableTreeBuilder : public TreeBuilder<Drawable> {

    TTF_Font *_font;    /**< font to use on text */

    /**
     * Process the given xml attributes for a given Drawable
     * @param drawable drawable to apply style to
     * @param attributes xml attribute map
     */
    static void set_style(Drawable *drawable, std::map<std::string, std::string> attributes);

    /**
     * Set the color of a given Drawable
     * @param drawable drawable to set color for
     * @param color_value name of RGB color preset
     */
    static void set_color_of_drawable(Drawable *drawable, std::string color_value);
public:
    /**
     * Constructor
     * @param font font to use on text
     */
    DrawableTreeBuilder(TTF_Font *font): _font(font) {}

    Drawable *construct_node(std::string type, std::string value,
                             std::map<std::string, std::string> attributes) const override;
};
}
