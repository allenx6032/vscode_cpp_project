#pragma once

#include <map>
#include <string>
#include <vector>
namespace SDL_GUI {

struct CommandLineOption {
    std::string _name;
    std::string _key;
    std::string _shortcut;
};

struct CommandLineFlag {
    std::string _name;
    std::string _key;
    std::string _shortcut;
};

class CommandLine {
    std::map<std::string, std::string> _parsed_options;
    std::map<std::string, bool> _parsed_flags;
    std::map<std::string, std::string> _parsed_positionals;

    std::vector<CommandLineOption> _options;
    std::vector<CommandLineFlag> _flags;
    std::vector<std::string> _positionals;

    bool parse_option(std::string argument);
    bool parse_flag(std::string flag);
    bool parse_positional(unsigned position, std::string argument);

public:
    void register_option(std::string key, std::string shortcut = "", std::string name = "");
    void register_flag(std::string key, std::string shortcut = "", std::string name = "");
    void register_positional(std::string name);

    void parse(int argc, char *argv[]);

    std::string get_option(std::string key) const;
    bool get_flag(std::string key) const;
    std::string get_positional(std::string key) const;
};

}
