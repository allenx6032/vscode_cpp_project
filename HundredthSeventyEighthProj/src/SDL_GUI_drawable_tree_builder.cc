#include<SDL_GUI_drawable_tree_builder.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

#include<SDL_GUI_rect.h>
#include<SDL_GUI_text.h>
#include<SDL_GUI_vertical_line.h>

using namespace SDL_GUI;

void DrawableTreeBuilder::set_style(Drawable *drawable,
                                    std::map<std::string, std::string> attributes) {
    for (std::pair<std::string, std::string> attribute: attributes) {
        std::string key = attribute.first;
        std::string value = attribute.second;
        if (STYLE_TYPE_MAP.find(key) == STYLE_TYPE_MAP.end()) {
            continue;
        }
        switch (STYLE_TYPE_MAP.at(key)) {
            case StyleType::BORDER:
                drawable->_style._has_border = true;
                break;
            case StyleType::BACKGROUND:
            case StyleType::COLOR:
                DrawableTreeBuilder::set_color_of_drawable(drawable, value);
                break;
            case StyleType::WIDTH:
                drawable->set_width(std::stoi(value));
                break;
            case StyleType::HEIGHT:
                drawable->set_height(std::stoi(value));
                break;
            case StyleType::POSITION_X:
                drawable->set_x(std::stoi(value));
                break;
            case StyleType::POSITION_Y:
                drawable->set_y(std::stoi(value));
                break;
            case StyleType::OVERFLOW:
                drawable->_style._overflow = (value == "true");
            default:
                break;
        }
    }
}

void DrawableTreeBuilder::set_color_of_drawable(Drawable *drawable, std::string color_value) {
    RGB color;
    /* check for number */
    char *end;
    unsigned char value_num = static_cast<unsigned char>(std::strtol(color_value.c_str(), &end,
                                                                     10));
    if (end != color_value.c_str()) {
        color = RGB(value_num);
    } else {
        color = RGB(color_value);
    }
    drawable->_style._color = color;
    drawable->_style._has_background = true;
}


Drawable *DrawableTreeBuilder::construct_node(std::string type, std::string value,
                                              std::map<std::string, std::string> attributes) const {
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    Drawable *drawable;
    /* TODO: use attributes */
    switch(TYPE_MAP.at(type)) {
        case Type::RECT:
            drawable = new Rect();
            break;
        case Type::TEXT:
            drawable = new Text(this->_font, value);
            break;
        case Type::VERTICAL_LINE:
            drawable = new VerticalLine();
            break;
        default:
            drawable = new NullDrawable();
            break;
    }
    if (attributes.find("attributes") != attributes.end()) {
        std::stringstream ss(attributes["attributes"]);
        std::string attribute;
        while (ss >> attribute) {
            drawable->add_attribute(attribute);
        }
    }
    this->set_style(drawable, attributes);
    return drawable;
}
