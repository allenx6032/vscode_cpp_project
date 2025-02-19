#include<SDL_GUI_rgb.h>

using namespace SDL_GUI;

/** prediefined color codes the object can be instantiated with. */
static const std::map<std::string, std::array<unsigned char, 4>> predefinitions = {
    {"red",    {255,   0,   0, 255}},
    {"green",  {  0, 255,   0, 255}},
    {"blue",   {  0,   0, 255, 255}},
    {"yellow", {255, 255,   0, 255}},
    {"white",  {255, 255, 255, 255}},
    {"black",  {  0,   0,   0, 255}},
};

RGB::RGB(std::string color) {
    if (predefinitions.find(color) == predefinitions.end()) {
        return;
    }
    std::array<unsigned char, 4> rgb_values = predefinitions.at(color);
    this->_r = rgb_values[0];
    this->_g = rgb_values[1];
    this->_b = rgb_values[2];
    this->_a = rgb_values[3];
}

RGB::operator SDL_Color() const {
    return SDL_Color{this->_r, this->_g, this->_b, this->_a};
}

void RGB::activate(SDL_Renderer *renderer) const {
    SDL_SetRenderDrawColor(renderer, this->_r, this->_g, this->_b, this->_a);
}
