#include <SDL_GUI_input_controller.h>

using namespace SDL_GUI;

static std::vector<SDL_Event> _events;

void SDL_GUI::read_sdl_events() {
    SDL_Event event;
    while (0 != SDL_PollEvent(&event)) {
        _events.push_back(event);
    }
}

void SDL_GUI::clear_sdl_events() {
    _events.clear();
}

const std::vector<SDL_Event> SDL_GUI::events() {
    return _events;
}

void LowLevelInputModel::press(Uint8 button) {
    this->_button_down.insert(button);
    this->_button_pressed.insert(button);
}

void LowLevelInputModel::release(Uint8 button) {
    // if (not this->_button_pressed.contains(button)) {
    if (!( this->_button_pressed.find(button) != this->_button_pressed.end())) {
        return;
    }
    this->_button_pressed.erase(button);
    this->_button_up.insert(button);
}

void LowLevelInputModel::trigger(Uint8 button) {
    this->_button_pressed.insert(button);
}

void LowLevelInputModel::update() {
    InputModel::update();
    this->_button_down.clear();
    this->_button_up.clear();
}

const std::set<Uint8> LowLevelInputModel::button_pressed() const {
    return this->_button_pressed;
}

const std::set<Uint8> LowLevelInputModel::button_down() const {
    return this->_button_down;
}

const std::set<Uint8> LowLevelInputModel::button_up() const {
    return this->_button_up;
}

