#pragma once

#include <map>
#include <string>

#include <SDL.h>

namespace SDL_GUI {
/** Color Code for RGB color */
class RGB {
public:
    unsigned char _r = 0; /**< red value between 0 and 255 */
    unsigned char _g = 0; /**< green value between 0 and 255 */
    unsigned char _b = 0; /**< blue value between 0 and 255 */
    unsigned char _a = 255; /**< apha value between 0 and 255 */

    /** Default constructor */
    RGB() = default;

    /**
     * Constructor for grey color
     * @param grey value for r, g and b
     */
    RGB(unsigned char grey) : _r(grey), _g(grey), _b(grey), _a(255) {}

    /**
     * Constructor
     * @param r red value between 0 and 255
     * @param g green value between 0 and 255
     * @param b blue value between 0 and 255
     * @param a alpha value between 0 and 255
     */
    RGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : _r(r), _g(g), _b(b), _a(a) {}

    /** Constructor for predefined colors */
    RGB(std::string color);

    /** conversion operator for SDL_Color */
    operator SDL_Color() const;

    /**
     * activate color for a given alpha on a given renderer
     * @param renderer renderer to activate color on
     */
    void activate(SDL_Renderer *renderer) const;
};
}
