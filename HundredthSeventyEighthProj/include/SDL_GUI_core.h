#pragma once

#include "SDL_GUI_application.h"
#include "SDL_GUI_plugin_base.h"

#include "SDL_GUI_input_controller.h"
#include "SDL_GUI_interface_controller.h"
#include "SDL_GUI_interface_model.h"
#include "SDL_GUI_input_model.h"
#include "SDL_GUI_interface_view.h"

#include "SDL_GUI_input_config.h"

namespace SDL_GUI {

/** Plugin that hold the default default GUI rendering */
class Core: public PluginBase {
private:
    ApplicationBase *_application;      /**< The application */
    InterfaceModel *_interface_model;   /**< The applications interface model */
    InputModel<InputValue, InputState> *_input_model; /**< The applications input model */
public:
    /** Constructor */
    Core(CommandLine *command_line) : PluginBase("Core", command_line) {}

    /**
     * Create all the needed Models, Controllers and Views
     * @tparam Ts List of already instantiated plugin types
     * @param app The application
     * @param plugins tuple of other plugins
     * @param argc programs argc
     * @param argv[] programs argv
     */
    template <typename ... Ts>
    void init(ApplicationBase *app, std::tuple<Ts...> *plugins) {
        (void)plugins;
        this->_application = app;

        if (app->is_headless()) {
            return;
        }

        /* Models */
        this->_interface_model = new SDL_GUI::InterfaceModel(app->renderer(), app->window_width(),
                                                             app->window_height());
        app->add_model(this->_interface_model);
        Drawable::set_interface_model(this->_interface_model);

        this->_input_model = new InputModel<InputValue, InputState>(InputState::ALL);
        app->add_model(this->_input_model);

        /* Controllers */
        InputController<InputValue, InputState> *input_controller =
            new InputController<InputValue, InputState>(this->_input_model, keyboard_input_config,
                    window_event_config, mouse_input_config, InputValue::QUIT);

        app->add_controller(input_controller);

        InterfaceController *interface_controller =
            new InterfaceController("templates/main.tpl", this->_interface_model, this->_input_model);
        interface_controller->_weight = 200;
        app->add_controller(interface_controller);

        /* Views */
        InterfaceView *interface_view = new InterfaceView(app->renderer(), this->_interface_model);
        app->add_view(interface_view);
    }

    /**
     * Getter for _interface_model
     * @return this->_interface_model
     */
    InterfaceModel *interface_model();

    /**
     * Getter for _input_model
     * @return this->_input_model
     */
    InputModel<InputValue, InputState> *input_model();
};
}
