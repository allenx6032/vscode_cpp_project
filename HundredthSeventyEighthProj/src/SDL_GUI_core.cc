#include<SDL_GUI_core.h>

using namespace SDL_GUI;

InterfaceModel *Core::interface_model() {
    return this->_interface_model;
}

InputModel<InputValue, InputState> *Core::input_model() {
    return this->_input_model;
}
