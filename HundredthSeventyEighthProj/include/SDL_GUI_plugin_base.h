#pragma once

#include <string>
#include <vector>

#include "SDL_GUI_command_line.h"

namespace SDL_GUI {
/** Base class for all the plugins. */
class PluginBase {
protected:
    /** Plugin name */
    std::string _name = "";

    CommandLine *_command_line;

    /**
     * Constructor
     * @param name Plugin name
     */
    PluginBase(std::string name, CommandLine *command_line)
        : _name(name),
          _command_line(command_line) {}
};
}
