#include <SDL_GUI_command_line.h>

#include <iostream>
#include <limits>

#include <SDL_GUI_string.h>

using namespace SDL_GUI;

bool CommandLine::parse_option(std::string argument) {
    /* options start with - */
    if (argument[0] != '-') {
        return false;
    }

    /* check if given key is a shortcut */
    argument = argument.substr(1);
    bool is_shortcut = (argument[0] != '-');
    if (not is_shortcut) {
        argument = argument.substr(1);
    }

    std::vector<std::string> split = split_string(argument, "=", 1);
    /* if there is no '=' its a flag */
    if (split.size() != 2) {
        return false;
    }
    std::string key = split[0];
    std::string value = split[1];

    /* find registered option */
    for (CommandLineOption &option: this->_options) {
        if ((is_shortcut && option._shortcut == key)
            || (not is_shortcut && option._key == key)) {
            this->_parsed_options.emplace(option._name, value);
            return true;
        }
    }
    return false;
}

bool CommandLine::parse_flag(std::string argument) {
    /* flags start with - */
    if (argument[0] != '-') {
        return false;
    }

    /* check if given argument is a shortcut */
    argument = argument.substr(1);
    bool is_shortcut = (argument[0] != '-');
    if (not is_shortcut) {
        argument = argument.substr(1);
    }

    /* find registered flag */
    for (CommandLineFlag &flag: this->_flags) {
        if ((is_shortcut && flag._shortcut == argument)
            || (not is_shortcut && flag._key == argument)) {
            this->_parsed_flags.emplace(flag._name, true);
            return true;
        }
    }
    return false;
}

bool CommandLine::parse_positional(unsigned position, std::string argument) {
    if (position >= this->_positionals.size()) {
        return false;
    }
    std::string key = this->_positionals[position];
    this->_parsed_positionals.emplace(key, argument);
    return true;
}

void CommandLine::register_option(std::string key, std::string shortcut, std::string name) {
    if (name.empty()) {
        name = key;
    }
    this->_options.emplace_back(CommandLineOption{name, key, shortcut});
}

void CommandLine::register_flag(std::string key, std::string shortcut, std::string name) {
    if (name.empty()) {
        name = key;
    }
    this->_flags.emplace_back(CommandLineFlag{name, key, shortcut});
}

void CommandLine::register_positional(std::string name) {
    this->_positionals.push_back(name);
}

void CommandLine::parse(int argc, char *argv[]) {
    unsigned positional_pos = 0;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (this->parse_option(arg)) {
            continue;
        }

        if (this->parse_flag(arg)) {
            continue;
        }

        if (this->parse_positional(positional_pos, arg)) {
            positional_pos++;
        }
    }
}

std::string CommandLine::get_option(std::string key) const {
    // if (this->_parsed_options.contains(key)) {
    if (this->_parsed_options.find(key) != this->_parsed_options.end()) {
        return this->_parsed_options.at(key);
    }
    return "";
}

bool CommandLine::get_flag(std::string key) const {
    // if (this->_parsed_flags.contains(key)) {
    if (this->_parsed_flags.find(key) != this->_parsed_flags.end()) {
        return this->_parsed_flags.at(key);
    }
    return false;
}

std::string CommandLine::get_positional(std::string key) const {
    // if (this->_parsed_positionals.contains(key)) {
    if (this->_parsed_positionals.find(key) != this->_parsed_positionals.end()) {
        return this->_parsed_positionals.at(key);
    }
    return "";
}
